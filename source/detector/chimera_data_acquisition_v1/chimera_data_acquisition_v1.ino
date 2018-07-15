#include "RingBufferDMA.h"


#define pinCount 16
uint8_t portPins[pinCount] = {16, 17, 19, 18, 49, 50, 31, 32, 0, 1, 29, 30, 43, 46, 44, 45}; // Port B bits 0, 1, 2, 3, 4, 5, 10, 11, 16, 17, 18, 19, 20, 21, 22, 23

uint16_t data = 0;

PORTA_PCR13 |= PORT_PCR_IRQC(1);

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < pinCount; i++) {
    pinMode(portPins[i],INPUT);
  }
  Serial.begin(115200);
  attachInterrupt(0, readADC, RISING);
}

void readADC() {
  data |= digitalReadFast(portPins[0]);
  data |= digitalReadFast(portPins[1]) << 1;
  data |= digitalReadFast(portPins[2]) << 2;
  data |= digitalReadFast(portPins[3]) << 3;
  data |= digitalReadFast(portPins[4]) << 4;
  data |= digitalReadFast(portPins[5]) << 5;
  data |= digitalReadFast(portPins[6]) << 6;
  data |= digitalReadFast(portPins[7]) << 7;
  data |= digitalReadFast(portPins[8]) << 8;
  data |= digitalReadFast(portPins[9]) << 9;
  data |= digitalReadFast(portPins[10]) << 10;
  data |= digitalReadFast(portPins[11]) << 11;
  data |= digitalReadFast(portPins[12]) << 12;
  data |= digitalReadFast(portPins[13]) << 13;
  data |= digitalReadFast(portPins[14]) << 14;
  data |= digitalReadFast(portPins[15]) << 15;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (data != 0) {
  Serial.print("new data\t");
  Serial.println(data,BIN);
  data=0; 
  }
}
