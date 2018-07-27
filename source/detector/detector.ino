// Program Options
#define FILE_SYSTEM 3

// Libraries
#include <ADC.h>
#include <DMAChannel.h>
#include "SdFs.h"

// Standard Library Includes
#include <array>

// Local Code
#include "chimera_utilities.h"

// connect out_pins to adc pins, PWM output on out pins will be measured.

const u8 adc0_pin0 = A0;  // digital pin 33, on ADC0
const u8 adc0_pin1 = A1;  // digital pin 34, on ADC0
const u8 adc1_pin0 = A2;  // digital pin 31, on ADC1
const u8 adc1_pin1 = A3;  // digital pin 32, on ADC1

constexpr std::array<u8, 4> adc_pins = { adc0_pin0, adc0_pin1, adc1_pin0, adc1_pin1 };
constexpr std::array<u8, 4> out_pins = { 5, 6, 9, 10 };

ADC adc;
ADC_Module *adc_modules[2];
static_assert(ADC_NUM_ADCS == 2, "Two ADCs expected.");

auto& serial = Serial;

const size_t buffer_size = 30000;
std::array<std::array<volatile u16, buffer_size>,  4> buffers;

std::array<volatile u32*, 4> adc_result_registers = { &ADC0_RA, &ADC0_RB, &ADC1_RA, &ADC1_RB };
std::array<DMAChannel, 4> dma_channels;
constexpr std::array<int, 4> dma_triggers = { DMAMUX_SOURCE_FTM3_CH0, DMAMUX_SOURCE_FTM3_CH1, DMAMUX_SOURCE_FTM3_CH2, DMAMUX_SOURCE_FTM3_CH3 };

// CMSIS PDB
#define PDB_C1_EN_MASK                           0xFFu
#define PDB_C1_EN_SHIFT                          0
#define PDB_C1_EN(x)                             (((u32)(((u32)(x))<<PDB_C1_EN_SHIFT))&PDB_C1_EN_MASK)
#define PDB_C1_TOS_MASK                          0xFF00u
#define PDB_C1_TOS_SHIFT                         8
#define PDB_C1_TOS(x)                            (((u32)(((u32)(x))<<PDB_C1_TOS_SHIFT))&PDB_C1_TOS_MASK)
#define PDB_C1_BB_MASK                           0xFF0000u
#define PDB_C1_BB_SHIFT                          16
#define PDB_C1_BB(x)                             (((u32)(((u32)(x))<<PDB_C1_BB_SHIFT))&PDB_C1_BB_MASK)

global
u32 current_file = 0;

global
SdFs sd;
FsFile file;

size_t bufferWriteIndex(size_t channel) {
    uintptr_t buffer_start = uintptr_t(buffers[channel].data());
    uintptr_t dma_pos = uintptr_t(dma_channels[channel].destinationAddress());
    return (dma_pos - buffer_start) / sizeof(u16);
}

void setup() {
    for(size_t i = 0; i < sizeof(adc_modules)/sizeof(adc_modules[0]); i++) adc_modules[i] = adc.adc[i];
    for(auto pin : adc_pins) pinMode(pin, INPUT);

    serial.begin(9600);
    delay(2000);
    serial.println("Starting");

    for(auto adc_module : adc_modules) {
        adc_module->setAveraging(1);
        adc_module->setResolution(12);
        adc_module->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED);
        adc_module->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);
    }
    
    // perform ADC input mux setup; the ADC library doesn't handle the B-set of registers
    // so we copy the config over
    adc.adc0->analogRead(adc0_pin1);
    ADC0_SC1B = ADC0_SC1A;
    adc.adc0->analogRead(adc0_pin0);

    adc.adc1->analogRead(adc1_pin1);
    ADC1_SC1B = ADC1_SC1A;
    adc.adc1->analogRead(adc1_pin0);

    for(auto adc_module : adc_modules) adc_module->stopPDB();
    // conversion will be triggered by PDB
    for(auto adc_module : adc_modules) adc_module->setHardwareTrigger();

    // enable PDB clock
    SIM_SCGC6 |= SIM_SCGC6_PDB;

    // Sample at 100'000 Hz
    constexpr u32 trigger_frequency = 100000;
    constexpr u32 mod = (F_BUS / trigger_frequency);
    static_assert(mod <= 0x10000, "Prescaler required.");

    // The K66 ADC conversion complete DMA triggering is completely broken for multi-channel conversions.
    // PDB DMA triggering doesn't work correctly, DMA channel linking for FTM-triggered DMA transfers
    // is also broken and doesn't correctly deassert COCO.
    // Thus, 4 timer channels are used to trigger DMA. The FTM counter at half point triggers DMA
    // for ADC A channel, the counter at MOD triggers DMA for ADC channel B.

    FTM3_SC = 0;
    FTM3_CNT = 0;
    FTM3_MOD = u16(mod - 1);

    // output compare mode, trigger DMA when counter reaches FTM3_CxV
    u32 ftm_channel_config = FTM_CSC_CHIE | FTM_CSC_DMA | FTM_CSC_MSA | FTM_CSC_ELSA;
    FTM3_C0SC = ftm_channel_config;
    FTM3_C1SC = ftm_channel_config;
    FTM3_C2SC = ftm_channel_config;
    FTM3_C3SC = ftm_channel_config;

    FTM3_C0V = u16(mod / 2 - 1);      // DMA trigger for ADC0 A
    FTM3_C1V = FTM3_MOD;                   // DMA trigger for ADC0 B
    FTM3_C2V = u16(mod / 2 - 1);      // DMA trigger for ADC1 A
    FTM3_C3V = FTM3_MOD;                   // DMA trigger for ADC1 B
    FTM3_EXTTRIG = FTM_EXTTRIG_INITTRIGEN; // external trigger at counter overflow --> trigger PDB

    PDB0_MOD = (u16)(mod-1);

    u32 pdb_ch_config = PDB_C1_EN (0b11) | // enable ADC A and B channel
                             PDB_C1_TOS(0b11) | // enables the channel delay
                             PDB_C1_BB (0b00);  // back-to-back trigger disabled
    PDB0_CH0C1 = pdb_ch_config; // ADC 0
    PDB0_CH1C1 = pdb_ch_config; // ADC 1

    // ADC0 A and ADC1 A conversions are triggered immediately, ADC0 B and ADC1 B at the half point
    PDB0_CH0DLY0 = 0;
    PDB0_CH0DLY1 = u16(mod / 2 - 1);
    PDB0_CH1DLY0 = 0;
    PDB0_CH1DLY1 = u16(mod / 2 - 1);

    const u32 pdb_base_conf = PDB_SC_TRGSEL(0b1011) |               // triggered by FTM3
                                   PDB_SC_PDBEN |                        // enable
                                   PDB_SC_PRESCALER(0) | PDB_SC_MULT(0); // count at F_BUS
                                   
    // sync buffered registers
    PDB0_SC = pdb_base_conf | PDB_SC_LDOK;
    
    for(size_t i = 0; i < 4; i++) {
        DMAChannel& dma = dma_channels[i];
        dma.source(* (u16 *)adc_result_registers[i]);
        dma.destinationBuffer(buffers[i].data(), sizeof(buffers[i]));
        dma.triggerAtHardwareEvent(dma_triggers[i]);
        dma.enable();
    }

    FTM3_SC = (FTM_SC_CLKS(1) | FTM_SC_PS(0));  // start FTM, run at F_BUS 

    // PWM output on out pins for testing purposes.
    for(auto out_pin : out_pins) analogWriteFrequency(out_pin, 5000);
    for(size_t i = 0; i < out_pins.size(); i++) analogWrite(out_pins[i], (i + 1) * 50);

    if(!sd.begin(SdSpiConfig(SDCARD_SS_PIN, DEDICATED_SPI, SD_SCK_MHZ(50)))) {
        Serial.println("ERROR: SD Card initialization failed.");
    }
    
    delay(500);
}

void loop() {
    while(bufferWriteIndex(1) >= 999);
    while(bufferWriteIndex(1) < 999);
    save_buffer_to_disk();
}

void save_buffer_to_disk() {
    char filename[32];
    snprintf(filename, 32, "data%i", (int)(current_file++));
    file.open(filename, O_WRITE | O_CREAT);
    if(file) {
        file.write((u8 *)(buffers[0].data()), sizeof(u16)*30000);
        file.close();
    }
    else {
        Serial.println("ERROR: Failed to open file.");
    }
}
