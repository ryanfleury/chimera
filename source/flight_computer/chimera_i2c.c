typedef struct I2CHandle {
    b32 valid;
    int file_handle;
    i32 length;
    char buffer[64];
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
    h->length = 0;
}

i32 i2c_set_address(I2CHandle *h, u8 address) {
    i32 result = ioctl(h->file_handle, I2C_SLAVE, address);
    return result;
}

i32 i2c_access(I2CHandle *h, char read_write, u8 command, int size,
               union i2c_smbus_data *data) {
    struct i2c_smbus_ioctl_data args;
    
    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    
    return ioctl(h->file_handle, I2C_SMBUS, &args);
}

i32 i2c_read_byte_data(I2CHandle *h, u8 command) {
    union i2c_smbus_data data;
    if(i2c_smbus_access(h, I2C_SMBUS_READ, command,
                        I2C_SMBUS_BYTE_DATA, &data)) {
        return -1;
    }
    else {
        return 0x0FF & data.byte;
    }
}