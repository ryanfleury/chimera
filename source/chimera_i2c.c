typedef struct I2CHandle {
    b32 valid;
    int file_handle;
    i32 length;
    char buffer[60];
} I2CHandle;

I2CHandle i2c_open(const char *filename) {
    I2CHandle h = {0};
    
    h.file_handle = open(filename, O_RDWR);
    if(h.file_handle >= 0) {
        h.valid = 1;
        
        int i2c_slave_address = 0x5a;
        if(ioctl(h.file_handle, 
                 I2C_SLAVE, 
                 i2c_slave_address) >= 0) {
        }
        else {
            fprintf(stderr, 
                    "ERROR: Failed to acquire bus access "
                    "or talk to slave.\n\n");
        }
    }
    else {
        fprintf(stderr, "ERROR: Failed to open \"%s\".\n\n", 
                filename);
    }
    
    return h;
}

void i2c_close(I2CHandle *h) {
    close(h->file_handle);
    h->file_handle = 0;
    h->valid = 0;
    h->length = 0;
}

void i2c_write(I2CHandle *h) {
    
}

void i2c_read(I2CHandle *h) {
    
}