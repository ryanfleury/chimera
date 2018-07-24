#include <Arduino.h>
#include <DMAChannel.h>
#define pinCount 16

uint16_t toggleData = 0x0;
bool state = 0;

DMAChannel dmachannel0;

void dma_check() {
  if(dmachannel0.complete()){
    Serial.println("DMA completed");
    dmachannel0.clearComplete();
    digitalWriteFast(13,!state);
    state = !state;
    dmachannel0.enable(); 
  }
  if(dmachannel0.error()){
    Serial.println("DMA error");
    digitalWriteFast(13,0);
    dmachannel0.clearError();
  }
}

void dma_isr() {
    Serial.println("DMA ISR triggered");
    dma_check();
    dmachannel0.clearInterrupt();
}

void setup()   {                
//  FMC_PFAPR |= 0x00FF0000; // Disable prefetching
//  SCB_SHPR3 = 0x20200000; // Systick = priority 32 (defaults to zero)
  uint8_t portPins[pinCount] = {16, 17, 19, 18, 49, 50, 31, 32, 0, 1, 29, 30, 43, 46, 44, 45}; // Port B bits 0, 1, 2, 3, 4, 5, 10, 11, 16, 17, 18, 19, 20, 21, 22, 23
  
  PORTB_PCR13 |= PORT_PCR_IRQC(1);
  
  dmachannel0.source(GPIOB_PDIR);
  dmachannel0.destination(toggleData);

  dmachannel0.transferSize(2);
  dmachannel0.transferCount(1);
  
  dmachannel0.interruptAtCompletion();

  dmachannel0.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTB);
  dmachannel0.enable();
  dmachannel0.attachInterrupt(dma_isr);

  Serial.begin(115200);
  for (int i = 0; i < pinCount; i++) {
    pinMode(portPins[i],INPUT);
  }
  
  pinMode(13, OUTPUT);
  digitalWriteFast(13,!state);
}

void loop() {
    dma_check();
    delay(100);
}
