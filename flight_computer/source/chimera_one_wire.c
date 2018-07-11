typedef struct OneWireHandle {
    b32 valid;
} OneWireHandle;

OneWireHandle one_wire_open() {
    OneWireHandle h = {0};
    
    return h;
}

void one_wire_close(OneWireHandle *h) {
    h->valid = 0;
}

int one_wire_write(OneWireHandle *h, int address, char *buffer) {
    int result = 0;
    
    return result;
}

int one_wire_read(OneWireHandle *h, int address) {
    int result = 0;
    
    return result;
}
