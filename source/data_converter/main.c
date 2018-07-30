#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#define heap_alloc(s) malloc(s)
#define heap_free(p)  free(p)

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef struct DetectorData {
    u64 data_cap;
    u64 data_size;
    i16 *data;
    u64 timestamp_cap;
    u64 timestamp_size;
    u64 *timestamps;
} DetectorData;

DetectorData init_detector_data() {
    DetectorData d = {0};
    
    d.data_cap = 1024;
    d.data = heap_alloc(sizeof(i16) * d.data_cap);
    
    d.timestamp_cap = 256;
    d.timestamps = heap_alloc(sizeof(u64) * d.timestamp_cap);
    
    return d;
}

void clean_up_detector_data(DetectorData *d) {
    heap_free(d->data);
    heap_free(d->timestamps);
    d->data = 0;
    d->data_cap = 0;
    d->data_size = 0;
    d->timestamps = 0;
    d->timestamp_cap = 0;
    d->timestamp_size = 0;
}

void add_timestamp_to_detector_data(DetectorData *d, u64 timestamp) {
    if(++d->timestamp_size >= d->timestamp_cap) {
        u64 *new_timestamps = heap_alloc(sizeof(u64) * (d->timestamp_cap<<2));
        memcpy(new_timestamps, d->timestamps, sizeof(u64) * (d->timestamp_size-1));
        heap_free(d->timestamps);
        d->timestamps = new_timestamps;
    }
    d->timestamps[d->timestamp_size-1] = timestamp;
}

void add_data_to_detector_data(DetectorData *d, i16 data) {
    if(++d->data_size >= d->data_cap) {
        i16 *new_data = heap_alloc(sizeof(i16) * (d->data_cap<<2));
        memcpy(new_data, d->data, sizeof(i16) * (d->data_size-1));
        heap_free(d->data);
        d->data = new_data;
    }
    d->data[d->data_size-1] = data;
}

void add_detector_data_from_file(DetectorData *d, FILE *f) {
    u64 timestamp = 0;
    fread(&timestamp, 1, sizeof(u64), f);
    add_timestamp_to_detector_data(d, timestamp);
    
    i16 read_data = 0;
    while(1) {
        if(fread(&read_data, 1, sizeof(i16), f) != sizeof(i16)) {
            break;
        }
        else {
            add_data_to_detector_data(d, read_data);
        }
    }
}

void export_detector_data_to_csv(DetectorData *d, const char *output_filename) {
    FILE *file = fopen(output_filename, "w");
    if(file) {
        for(u64 i = 0; i < d->timestamp_size; ++i) {
            fwrite(file, "%" PRIu64 ",", d->timestamps[i]);
        }
        fclose(file);
    }
    else {
        fprintf(stderr, "ERROR: Could not open \"%s\" for writing", 
                output_filename);
    }
}

int main(int argument_count, char **arguments) {
    if(argument_count > 1 && arguments[1]) {
        char *data_directory = arguments[1];
        fprintf(stdout, "Converting data in \"%s\" from binary format to CSV.\n\n",
                data_directory);
        
        char data_filename[128] = {0};
        int data_file_counter = 0;
        
        DetectorData detector_data = init_detector_data();
        
        while(1) {
            snprintf(data_filename, 128,
                     "%s/data%i",
                     data_directory,
                     data_file_counter++);
            
            FILE *data_file = fopen(data_filename, "rb");
            if(data_file) {
                add_detector_data_from_file(&detector_data, data_file);
                fclose(data_file);
            }
            else {
                break;
            }
        }
        
        export_detector_data_to_csv(&detector_data, "detector_data.csv");
        
        clean_up_detector_data(&detector_data);
    }
    else {
        fprintf(stdout, "USAGE: %s <data directory>\n\n",
                arguments[0]);
    }
    
    return 0;
}