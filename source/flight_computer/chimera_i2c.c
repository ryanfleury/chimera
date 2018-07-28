typedef struct I2CHandle {
    b32 valid;
    int file_handle;
} I2CHandle;

I2CHandle i2c_open(const char *filename) {
    I2CHandle h = {0};
    
    h.file_handle = open(filename, O_RDWR);
    if(h.file_handle >= 0) {
        h.valid = 1;
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
}

i32 i2c_write(I2CHandle *h, i32 address, u8 register,
              char *buffer, i32 len) {
    i32 result = 0;
    { 
        if(len > 0) {
            if(ioctl(h->file_handle, I2C_SLAVE, address) >= 0) {
                i32 write_result;
                if((write_result = write(h->file_handle, buffer, len)) == len) {
                    result = 1;
                }
                else {
                    error("Write of %i bytes returned %i",
                          len, 
                          write_result);
                }
            }
            else {
                error("Failed to communicate with i2c device");
            }
        }
        else {
            error("Attempted write of negative bytes");
        }
    }
    return result;
}

i32 i2c_read(I2CHandle *h, i32 address, u8 register, 
             char *buffer, i32 len) {
    i32 result = 0;
    {
        if(len > 0) {
            if(ioctl(h->file_handle, I2C_SLAVE, address) >= 0) {
                i32 read_result;
                if((read_result = read(h->file_handle, buffer, len)) == len) {
                    result = 1;
                }
                else {
                    error("Read of %i bytes returned %i",
                          len,
                          read_result);
                }
            }
            else {
                error("Failed to communicate with i2c device");
            }
        }
        else {
            error("Attempted read of negative bytes");
        }
    }
    return result;
}