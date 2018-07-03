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
#define CHIMERA_ENVIRONMENTAL_CHIP  1
#define CHIMERA_SENSOR_CHIP         0

// C Standard Library Includes
#include <stdlib.h>
#include <stdint.h>

// Local Code
#include "chimera_utilities.c"

#if CHIMERA_ENVIRONMENTAL_CHIP
#include "chimera_chip_environmental.c"
#elif CHIMERA_SENSOR_CHIP
#include "chimera_chip_sensor.c"
#endif

#include "chimera_state.c"

int main(int argc, char **argv) {
    printf("Hello, World!\n");
    return 0;
}
