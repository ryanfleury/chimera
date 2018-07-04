/*

Project Chimera Sensor Code
..................................................
.  
.  Code for systems aboard the Project Chimera 
.  payload, flown by the University of Colorado
.  Boulder SPS Research group.
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
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Local Code
#include "chimera_utilities.c"
#include "chimera_debug.c"
#include "chimera_i2c.c"
#include "chimera_chip_environmental.c"
#include "chimera_state.c"

int main(int argc, char **argv) {
    debug_log("PROJECT CHIMERA ENVIRONMENTAL SENSOR");
    
    debug_log("Initializing program state...");
    block(
        State *state = 0; initialize_state(&state),
        clean_up_state(&state)
        ) {
        
        if(state) {
            debug_log("Trying to access i2c interface...");
            block(I2CHandle i2c = i2c_open("/dev/i2c-1"), 
                  i2c_close(&i2c)) {
                
                if(i2c.valid) {
                    
                }
                else {
                    error("I2C handle invalid");
                }
            }
        }
        else {
            error("Program state initialization failed");
        }
    }
    
    return 0;
}
