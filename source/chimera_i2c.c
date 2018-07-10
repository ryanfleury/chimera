typedef struct I2CHandle {
    b32 valid;
    int file_handle;
    i32 length;
    char buffer[64];
} I2CHandle;

I2CHandle i2c_open(i32 address, const char *filename) {
    I2CHandle h = {0};
    
    h.file_handle = open(filename, O_RDWR);
    if(h.file_handle >= 0) {
        h.valid = 1;
        
        int i2c_slave_address = address;
        if(ioctl(h.file_handle, 
                 I2C_SLAVE, 
                 i2c_slave_address) >= 0) {
            
            debug_log("Connected to \"%s\" successfully",
                      filename);
            
        }
        else {
            error("Failed to acquire bus access "
                  "or talk to slave");
        }
    }
    else {
        error("Failed to open \"%s\"", 
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

int i2c_write(I2CHandle *h, char *buffer) {
    int result = 0;
    
    if(buffer) {
        u32 length = 0;
        for(char *c = buffer; *c++; ++length);
        
        ssize_t bytes_written = 
            write(h->file_handle, buffer, length);
        if(bytes_written) {
            result = 1;
        }
        else {
            result = 0;
        }
    }
    
    return result;
}

int i2c_read(I2CHandle *h) {
    int result = 0;
    
    {
        ssize_t bytes_read = read(h->file_handle, h->buffer, 64);
        h->length = (i32)bytes_read;
        if(!bytes_read) {
            result = 0;
        }
        else {
            result = 1;
        }
    }
    
    return result;
}
