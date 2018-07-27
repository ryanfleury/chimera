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
*/

/************************************************/

// Program Options
#include <SD.h>
#include <SD_t3.h>
#include <SPI.h>

// C Standard Library Includes
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

// Teensyduino Includes
#include <Arduino.h>
//#include "ADC.h"
//#include "RingBufferDMA.h"

// Local Code
#include "chimera_utilities.h"
//#include "chimera_dma_handle.h"

// Pin data and definitions
enum {
    
#define pin(name, number, mode) PIN_ ## name,
#include "chimera_detector_pins.h"
#undef pin
    
    MAX_PIN
};

global
u8 pin_numbers[MAX_PIN] = {
    
#define pin(name, number, mode) number ,
#include "chimera_detector_pins.h"
#undef pin
    
};

#define WRITE_BUFFER_SIZE   bytes(4)
#define WRITE_BUFFER_COUNT  (WRITE_BUFFER_SIZE / sizeof(i16))
global u32 write_buffer_write_pos = 0;

global
i16 write_buffer[WRITE_BUFFER_COUNT] = {0};

global
char write_filename[32] = {0};

global
u32 current_file_number = 0;

// Main function
extern "C"
int main(void) {
    
#define pin(name, number, mode) pinMode(pin_numbers[PIN_ ## name], mode);
#include "chimera_detector_pins.h"
#undef pin
    
    Serial.begin(115200);

    if(!SD.begin(BUILTIN_SDCARD)) {
        Serial.println("SD card initialization failure.");
    }
    
    while(1) {
        i16 analog_val = analogRead(pin_numbers[PIN_adc_input]);
        write_buffer[write_buffer_write_pos++] = analog_val;

        if(write_buffer_write_pos >= WRITE_BUFFER_COUNT) {
            write_buffer_write_pos = 0;
            save_write_buffer_to_disk();
        }
    }
    
}

void save_write_buffer_to_disk(void) {
    snprintf(write_filename, 32, "file%i", (int)current_file_number);
  
    File file = SD.open(write_filename, FILE_WRITE);
    if(file) {
        Serial.print("Saving to disk");
        file.write("Hello!");
        file.write((u8 *)write_buffer, sizeof(write_buffer));
        file.close();
    }

    ++current_file_number;
}

