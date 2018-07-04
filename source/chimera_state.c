#define CHIMERA_DATA_WRITE_BUFFER_SIZE megabytes(16)

typedef struct State {
    char data_write_buffer[CHIMERA_DATA_WRITE_BUFFER_SIZE];
    u64 data_write_position;
    
    union {
        struct {
            
        };
        struct {
            
        };
    };
} State;

void initialize_state(State **state) {
    *state = (State *)alloc_heap_memory(sizeof(State));
    (*state)->data_write_position = 0;
}

void update_state(State *state) {
    // NOTE(Ryan): Assumptions:
    // 
    // * Fixed-size measurements as r32's.
    
    r32 measurements[CHIMERA_MEASUREMENT_COUNT] = {0};
    
    load_measurements(measurements);
    
    if(state->data_write_position + sizeof(measurements) > 
       CHIMERA_DATA_WRITE_BUFFER_SIZE) {
        // TODO(Ryan): Data writing
        // write_data_write_buffer_to_disk(state);
        state->data_write_position = 0;
    }
    
    foreach(i, CHIMERA_MEASUREMENT_COUNT) {
        (r32 *)(state->data_write_buffer+
                state->data_write_position) = measurements[i];
        state->data_write_position += sizeof(r32);
    }
}

void clean_up_state(State **state) {
    // TODO(Ryan): Data writing
    // write_data_write_buffer_to_disk(*state);
    
    free_heap_memory(*state);
    *state = 0;
}
