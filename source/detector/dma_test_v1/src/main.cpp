// #include "RingBufferDMA.h"
#include <DMAChannel.h>
#include <Arduino.h>

#define pinCount 16
uint8_t portPins[pinCount] = {16, 17, 19, 18, 49, 50, 31, 32, 0, 1, 29, 30, 43, 46, 44, 45}; // Port B bits 0, 1, 2, 3, 4, 5, 10, 11, 16, 17, 18, 19, 20, 21, 22, 23

uint16_t data = 0;

DMAChannel dmachannel0;

uint32_t toggleData = 0x0;

void readADC() {
	toggleData = 1;
}

extern "C" int main(void) {
  //delay(10000);
  Serial.begin(115200);
  Serial.println("hello");
  bool state = 0;
  for (int i = 0; i < pinCount; i++) {
    pinMode(portPins[i],INPUT);
  }
  
  //SCB_SHPR3 = 0x20200000;  // Systick = priority 32 (defaults to zero)
  //PORTA_PCR0 |= PORT_PCR_IRQC(1); // Trigger DMA request on rising edge
  attachInterrupt(0, readADC, RISING);
  
  pinMode(13, OUTPUT);
  digitalWriteFast(13,!state);
  state = !state;

  //dmachannel0.source(GPIOB_PDIR);
  //dmachannel0.destination(toggleData);
  
  //dmachannel0.transferSize(4);
  //dmachannel0.transferCount(1);
  
  //dmachannel0.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTB);
  //dmachannel0.enable();
  
  //noInterrupts();

  while (1) {
    if (toggleData != 0) {
      //Serial.print("new data\t");
      //Serial.println(toggleData,BIN);
      digitalWriteFast(13,!state);
      state = !state;
      toggleData = 0x0;
    }
  }
}
