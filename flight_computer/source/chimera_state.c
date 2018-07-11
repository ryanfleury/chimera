#define CHIMERA_DATA_WRITE_BUFFER_SIZE megabytes(16)
#define CHIMERA_MEASUREMENT_COUNT 12

typedef struct State {
    I2CHandle i2c;
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

int load_measurements(r32 measurements[CHIMERA_MEASUREMENT_COUNT]) {
    int result = 0;
    // TODO(Ryan): Fill measurement array
    return result;
}

int update_state(State *state) {
    int result = 0;
    
    // NOTE(Ryan): Assumptions:
    // 
    // * Fixed-size measurements as r32's.
    
    r32 measurements[CHIMERA_MEASUREMENT_COUNT] = {0};
    
    if(load_measurements(measurements)) {
        result = 1;
        
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
        result = 0;
    }
    
    return result;
}
