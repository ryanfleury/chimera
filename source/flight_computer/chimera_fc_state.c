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
} State;

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
    }
    else {
        error("Failed to allocate enough memory for the flight computer state");
    }
}

void clean_up_state(State **state) {
    // TODO(Ryan): Data writing
    // write_data_write_buffer_to_disk(*state);
    debug_log("Closing i2c handle");
    i2c_close(&(*state)->i2c);
    debug_log("Freeing state memory");
    free_heap_memory(*state);
    *state = 0;
}

int load_measurements(State *state,
                      r32 measurements[CHIMERA_MEASUREMENT_COUNT]) {
    int result = 1;
    
    foreach(i, CHIMERA_MEASUREMENT_COUNT) {
        if(i2c_read(&state->i2c, 
                    state->measurement_addresses[i])) {
            
        }
        else {
            result = 0;
            error("Failed to take measurement #%i, \"%s\" (%i)",
                  (int)i, 
                  state->measurement_names[i],
                  errno);
        }
    }
    
    return result;
}

int update_state(State *state) {
    int result = 0;
    
    // NOTE(Ryan): Assumptions:
    // 
    // * Fixed-size measurements as r32's.
    
    r32 measurements[CHIMERA_MEASUREMENT_COUNT] = {0};
    
    result = 1;
    if(load_measurements(state, measurements)) {
        
        if(state->data_write_position + sizeof(measurements) > 
           CHIMERA_DATA_WRITE_BUFFER_SIZE) {
            // TODO(Ryan): Data writing
            // write_data_write_buffer_to_disk(state);
            state->data_write_position = 0;
        }
        
        foreach(i, CHIMERA_MEASUREMENT_COUNT) {
            *(r32 *)(state->data_write_buffer+
                     state->data_write_position) = measurements[i];
            state->data_write_position += sizeof(r32);
        }
    }
    else {
        error("Failed to load measurements");
    }
    
    return result;
}
