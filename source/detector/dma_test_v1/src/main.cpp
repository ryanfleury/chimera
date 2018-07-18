#include <Arduino.h>
#include <DMAChannel.h>
#define pinCount 16


uint16_t toggleData = 0x0;
bool state = 0;


DMAChannel dmachannel0;

void readADC() {
    toggleData = 1;
}

void dma_check() {
    if(dmachannel0.complete()){    
        
        Serial.println("Analog Data:");
        Serial.println("Done Transmitting");
        dmachannel0.clearComplete();
        digitalWriteFast(13,!state);
        state = !state;
        dmachannel0.enable(); 
    }
    if(dmachannel0.error()){
        Serial.println("DMA transfer error.");
        digitalWriteFast(13,0);
        dmachannel0.clearError();
    }
}

void dma_isr() {
    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
    Serial.println("dmaBuffer_isr");
    dma_check();
    // update the internal buffer positions
    dmachannel0.clearInterrupt();
}

extern "C" int main(void) {
    FMC_PFAPR |= 0x00FF0000; // Disable prefetching
    SCB_SHPR3 = 0x20200000; // Systick = priority 32 (defaults to zero)
	uint8_t portPins[pinCount] = {16, 17, 19, 18, 49, 50, 31, 32, 0, 1, 29, 30, 43, 46, 44, 45}; // Port B bits 0, 1, 2, 3, 4, 5, 10, 11, 16, 17, 18, 19, 20, 21, 22, 23
    //	uint32_t toggleData = 0x0;
	
	PORTB_PCR13 |= PORT_PCR_IRQC(1);
    
    
	dmachannel0.source(GPIOB_PDIR);
	dmachannel0.destination(toggleData);
    
	dmachannel0.transferSize(2);
	dmachannel0.transferCount(1);
    
	// dmachannel0.interruptAtCompletion();
    
	dmachannel0.triggerAtHardwareEvent(DMAMUX_SOURCE_PORTB);
    // dmachannel0.triggerContinuously();
    // dmachannel0.disableOnCompletion();
	dmachannel0.enable();
    dmachannel0.attachInterrupt(dma_isr);
	//noInterrupts();
    
	Serial.begin(115200);
	for (int i = 0; i < pinCount; i++) {
		pinMode(portPins[i],INPUT);
	}
    
    //	attachInterrupt(0, readADC, RISING);
	pinMode(13, OUTPUT);
	digitalWriteFast(13,!state);
	
	while (1) {
		// if (toggleData != 0) {
		// 	Serial.print("new data\t");
		// 	Serial.println(toggleData,BIN);
		// 	digitalWriteFast(13,!state);
		// 	state = !state;
		// 	toggleData = 0x0;
		// }
		// Serial.println("hello");
		// delay(500);
        dma_check();
        delay(100);
	}
}