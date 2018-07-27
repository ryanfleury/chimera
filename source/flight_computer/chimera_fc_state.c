#define CHIMERA_DATA_WRITE_BUFFER_SIZE megabytes(16)

enum {
    
#define measurement(name, address) CHIMERA_MEASUREMENT_ ## name,
#include "chimera_fc_measurements.c"
#undef measurement
    
    CHIMERA_MEASUREMENT_COUNT
};

typedef struct State {
    I2CHandle i2c;
    int measurement_addresses[CHIMERA_MEASUREMENT_COUNT];
    const char *measurement_names[CHIMERA_MEASUREMENT_COUNT];
    char data_write_buffer[CHIMERA_DATA_WRITE_BUFFER_SIZE];
    u64 data_write_position;
    u64 current_file;
} State;

void write_data_write_buffer_to_disk(State *state) {
    char filename[32] = {0};
    snprintf(filename, 32, "data%i", (int)state->current_file);
    FILE *file = fopen(filename, "wb"); 
    if(file) {
        fwrite(state->data_write_buffer, 1, CHIMERA_DATA_WRITE_BUFFER_SIZE,
               file);
        fclose(file);
    }
}

void initialize_state(State **state) {
    debug_log("Attempting to allocate memory for state");
    *state = (State *)alloc_heap_memory(sizeof(State));
    if(*state) {
        debug_log("Attempting to access to i2c interface");
        (*state)->i2c = i2c_open(CHIMERA_I2C_FILENAME);
        if(!(*state)->i2c.valid) {
            error("Failed to access to i2c interface");
        }
        
#define measurement(name, address) {\
            (*state)->measurement_addresses[CHIMERA_MEASUREMENT_ ## name] = address; \
            (*state)->measurement_names[CHIMERA_MEASUREMENT_ ## name] = #name; \
        }
#include "chimera_fc_measurements.c"
#undef measurement
        
        (*state)->data_write_position = 0;
        (*state)->current_file = 0;
    }
    else {
        error("Failed to allocate enough memory for the flight computer state");
    }
}

void clean_up_state(State **state) {
    write_data_write_buffer_to_disk(*state);
    debug_log("Closing i2c handle");
    i2c_close(&(*state)->i2c);
    debug_log("Freeing state memory");
    free_heap_memory(*state);
    *state = 0;
}

int load_measurements(State *state,
                      i16 measurements[CHIMERA_MEASUREMENT_COUNT]) {
    int result = 1;
    
    // Take temperature measurement
    {
        u8 reg; 
        
        i2c_set_address(&state->i2c, CHIMERA_TEMPERATURE_I2C_ADDRESS);
        
        reg = 0x04;
        u8 msb = i2c_read_byte_data(&state->i2c, reg);
        
        reg = 0x03;
        u8 lsb = i2c_read_byte_data(&state->i2c, reg);
        
        i16 temperature = 0 | msb << 8;
        temperature |= lsb;
        
        fprintf(stderr, "%i ", (i32)temperature);
    }
    
    return result;
}

int update_state(State *state) {
    int result = 0;
    
    // @NOTE(Ryan): Assumptions:
    // 
    // * Fixed-size measurements as i16's.
    
    i16 measurements[CHIMERA_MEASUREMENT_COUNT] = {0};
    
    result = 1;
    if(load_measurements(state, measurements)) {
        
        if(state->data_write_position + sizeof(measurements) > 
           CHIMERA_DATA_WRITE_BUFFER_SIZE) {
            write_data_write_buffer_to_disk(state);
            state->data_write_position = 0;
        }
        
        // @HACK(Ryan): THIS ONLY WRITES TEMPERATURE
        foreach(i, 1) {
            *(i16 *)(state->data_write_buffer+
                     state->data_write_position) = measurements[i];
            state->data_write_position += sizeof(i16);
        }
    }
    else {
        error("Failed to load measurements");
    }
    
    return result;
}
