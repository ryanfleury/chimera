//——————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  This eDMA demo shows how using DMA for outputing periodic signal
//  The output port is D13 (LED_BUILTIN), i.e. PTC5
//  Port D3 is toggled at PIT0 frequency
//  Port D4 is toggled at the end of PATTERN_LENGTH (major loop count) transfers
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————
// This code works on Teensy 3.6
// It has not been tested on Teensy 3.5
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————
// IT DOES NOT WORK ON TEENSY 3.1, THE MICRO-CONTROLLER IS BUGGY
//    The symptom is D4 blinks at 150 kHz, independantly from PATTERN_FREQUENCY 
//    See Mask Set Errata for Mask 1N36B: https://www.nxp.com/docs/en/errata/KINETIS_K_1N36B.pdf
//    Erratum ID e4588: Instead of sending a single DMA request every time the PIT expires, the first
//    time the PIT triggers a DMA transfer the “always enabled” source will not negate its request.
//    This results in the DMA request remaining asserted continuously after the first trigger.
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————

static const size_t PATTERN_FREQUENCY = 6000 ; // In Hertz
// So D3 frequency is PATTERN_FREQUENCY / 2
// So D4 frequency is PATTERN_FREQUENCY / 2 / PATTERN_LENGTH

static const size_t PATTERN_LENGTH = 2;
const bool kPATTERN [PATTERN_LENGTH] = { // Morse code example: output "S S S ..."
  true, false
} ;
//--- Caution: last kPATTERN element should be "false"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————
// eDMA module cannot access GPIO from aliased bit-band regions.
// eDMA module can access:
//    - GPIOx_PDOR (but it changes all bits of the given GPIO x)
//    - GPIOx_PSOR (it sets selected bits of the given GPIO x, but cannot reset them)
//    - GPIOx_PCOR (it resets selected bits of the given GPIO x, but cannot set them)
//    - GPIOx_PTOR (it toggles selected bits of the given GPIO x)
// So we use GPIOC_PTOR

// The output port is D13 (LED_BUILTIN): D13 is PORTC #5
// For toggling D13, without any change on other GPIOC bits: "GPIOC_PTOR = (1 << 5) ;"
// "GPIOC_PTOR = 0 ;" is a nop
// So we need to transform the kPATTERN boolean array to the gOutputPattern uint32_t array, that will
// contains (1 << 5) for toggling PTC5 port, and 0 for not changing it: the values
// are not the kPATTERN values, but the toggles we need to output the given pattern.
// Building the gOutputPattern array is done at the beginning of the setup function.

uint32_t gOutputPattern [PATTERN_LENGTH] ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————

void setup () {
//   Serial.begin (9600) ;
//--- Transform pattern for encoding level changes
  bool current = false ;
  for (size_t i=0 ; i<PATTERN_LENGTH ; i++) {
    const bool b = kPATTERN [i] ;
    gOutputPattern [i] = (current == b) ? 0 : (1 << 5) ;
    current = b ;
  }

//--- Optional, for toggling D3 when PIT 0 is running (see loop function)
  pinMode (3, OUTPUT) ; 

//--- Optional, for toggling D4 at the end of PATTERN_LENGTH (major loop count) transfers (see loop function)
  pinMode (4, OUTPUT) ; 

 //--- Set up LED_BUILTIN digital output at LOW level
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, LOW) ;

//--- Power on DMA_MUX, PIT, DMA
  SIM_SCGC6 |= SIM_SCGC6_DMAMUX | SIM_SCGC6_PIT ;
  SIM_SCGC7 |= SIM_SCGC7_DMA ;

//--- Enable PIT module
  PIT_MCR = 0 ;

//--- Disable PIT0
  PIT_TCTRL0 = 0 ;

//--- PIT0 clock frequency is F_BUS (in Hertz): PIT_LDVAL0 sets the request period of DMA 0
  PIT_LDVAL0 = ((F_BUS) / PATTERN_FREQUENCY) - 1 ;

//--- Clear PIT 0 interrupt flag
  PIT_TFLG0 = 1 ;

//--- Disable DMA Channel 0: it is required for configuring it
  DMAMUX0_CHCFG0 = 0 ;

//--- Initial source address: the gOutputPattern array address
  DMA_TCD0_SADDR = gOutputPattern ;

//--- After every request, source address is incremented by 4
  DMA_TCD0_SOFF = sizeof (uint32_t) ;

//--- At the end of a major loop, source address is decremented to gOutputPattern array address
  DMA_TCD0_SLAST = - sizeof (uint32_t) * PATTERN_LENGTH ; // - transfert_size * major_loop_count

//--- Destination address: the address of the GPIOC_PTOR control register
  DMA_TCD0_DADDR = & GPIOC_PTOR ;

//--- After every request, destination address is not modified
  DMA_TCD0_DOFF = 0 ;

//--- At the end of a major loop, destination address is not modified
  DMA_TCD0_DLASTSGA = 0 ;

//--- Set major loop count
  DMA_TCD0_CITER_ELINKNO = PATTERN_LENGTH ;
  DMA_TCD0_BITER_ELINKNO = PATTERN_LENGTH ;

//--- Source and destination are 32-bit
  DMA_TCD0_ATTR = 
    DMA_TCD_ATTR_SSIZE (DMA_TCD_ATTR_SIZE_32BIT) // Transfer source size is 32-bit
  | DMA_TCD_ATTR_DSIZE (DMA_TCD_ATTR_SIZE_32BIT) // Transfer destination size is 32-bit
  ;

//--- Number of bytes to transfer per request: 4
  DMA_TCD0_NBYTES_MLNO = sizeof (uint32_t) ;

//--- Set repetition and DMA interrupt
// if DREQ bit is set, the DMA performs PATTERN_LENGTH transfers and stops)
// Otherwise, the DMA performs PATTERN_LENGTH transfers repetitively
  DMA_TCD0_CSR =
    DMA_TCD_CSR_INTMAJOR // Generate an interrupt at the end of major loop count transfert (for toggling D4)
   // | DMA_TCD_CSR_DREQ // For setting DREQ bit
  ;

//--- Configure DMA Channel
  DMAMUX0_CHCFG0 =
      DMAMUX_ENABLE // DMA Channel 0 is enabled
    | DMAMUX_TRIG   // DMA Channel 0 is triggered by PIT 0
    | DMAMUX_SOURCE_ALWAYS0 // Trigger is always active
  ;

//--- Start DMA Channel 0
  DMA_SERQ = 0 ;

//--- Enable PIT 0 : it starts counting
  PIT_TCTRL0 = PIT_TCTRL_TEN ;
}

//—————————————————————————————————————————————————————————————————————————————————————————————————————————————

void loop () {
//--- Toggle D3 on PIT 0 count underflow
  if (PIT_TFLG0) {
    PIT_TFLG0 = 1 ;
    digitalWrite (3, !digitalRead (3)) ;
    Serial.println (DMA_ES, HEX) ;
  }
  delayMicroseconds(1000);
////--- Toggle D4 on major loop count (end of pattern)
//  if ((DMA_INT & 1) != 0) {
//    DMA_CINT = 0 ;
//    digitalWrite (4, !digitalRead (4)) ;
//  }
}

//—————————————————————————————————————————————————————————————————————————————————————————————————————————————

