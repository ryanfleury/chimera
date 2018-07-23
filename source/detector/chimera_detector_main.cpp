/*

Project Chimera Detector Code
..................................................
.  
.  Code for systems aboard the Project Chimera 
.  payload, flown by the University of Colorado
.  Boulder SPS Research group. Specifically, for
.  the payload's detectors.
.

Codebase Notes 
..................................................

1  Fixed-size types are standard, and have
.  short typedefs for convenience. A list
.  follows:
.  
.  Signed Integers:
.    * i8  or s8  (8-bit)
.    * i16 or s16 (16-bit)
.    * i32 or s32 (32-bit)
.    * i64 or s64 (64-bit)
.
.  Unsigned Integers:
.    * u8  (8-bit)
.    * u16 (16-bit)
.    * u32 (32-bit)
.    * u64 (64-bit)
.
.  Floating Point:
.    * r32 or f32 (float)
.    * r64 or f64 (double)
.
.  Booleans:
.    * b8  (8-bit)
.    * b16 (16-bit)
.    * b32 (32-bit)
.    * b64 (64-bit)
.
..................................................

2  Some macros are used. "it" implies an 
.  iterating value.
.  
.  * foreach(it, limit) is equivalent to:
.    for(u64 it = 0; it < limit; ++it)
.
.  * alloc_heap_memory(num_bytes) is an alias for
.    a function that heap-allocates memory
.    and returns a void * to it. This maps to the
.    C standard library's "malloc" function.
.    This is provided in the case where an 
.    alternate allocation method is required.
.
.  * free_heap_memory(ptr) is an alias for a
.    function that releases memory. This maps to
.    the C standard library's "free" function.
.    This is provided in the case where an
.    alternate memory free method is required.
.    
..................................................

/************************************************/

// Program Options


// C Standard Library Includes


// Teensyduino Includes
#include <Arduino.h>
#include "ADC.h"
#include "RingBufferDMA.h"

// Local Code
#include "chimera_shared.c"

global b32 quit = 0;

// Pin data and definitions
enum {
    
#define pin(name, number) PIN_ ## name,
#include "chimera_detector_pins.c"
#undef pin
    
    MAX_PIN
};

global
u8 pin_numbers[MAX_PIN] = {
    
#define pin(name, number) number ,
#include "chimera_detector_pins.c"
#undef pin
    
};

global
const char *pin_names[MAX_PIN] = {
    
#define pin(name, number) #name ,
#include "chimera_detector_pins.c"
#undef pin
    
};

// Main function
extern "C"
int main(void) {
    
    foreach(i, MAX_PIN) {
        pinMode(pin_numbers[i], INPUT);
    }
    
    Serial.begin(115200);
    attachInterrupt(pin_numbers[PIN_quit_signal], quit_signal_callback, RISING);
    
    ADC *adc = new ADC();
    RingBufferDMA *dma_buffer = new RingBufferDMA(buffer, buffer_size,
                                                  pin_numbers[PIN_adc_input]);
    
    while(!quit) {
        // @TODO(Ryan):
        
    }
}

void quit_signal_callback() {
    u32 quit_count = 0;
    while(digitalReadFast(pin_numbers[PIN_quit_signal])) {
        if(++quit_count > 10) {
            quit = 1;
            break;
        }
    }
}
