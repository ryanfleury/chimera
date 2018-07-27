typedef void (*DMACallback)(void);

typedef struct DMAHandle {
    
    volatile i16 *buffer;
    u32 buffer_size;
    u32 buffer_write_pos;
    volatile u32 *adc_ra;
    DMAChannel dma_channel;
    u8 adc_num;
    
} DMAHandle;

DMAHandle dma_handle_init(volatile i16 *buffer,
                          u32 buffer_size,
                          u8 adc_num) {
    DMAHandle r = {0};
    
    r.buffer = buffer;
    r.buffer_size = buffer_size;
    r.buffer_write_pos = 0;
    r.adc_num = adc_num;
    r.adc_ra = &ADC0_RA + (u32)0x20000*adc_num;
    r.dma_channel = DMAChannel();
    
    return r;
}

void dma_handle_clean_up(DMAHandle *r) {
    r->dma_channel.detachInterrupt();
    r->dma_channel.disable();
}

void dma_handle_start(DMAHandle *r, DMACallback callback) {
    r->dma_channel.source(*r->adc_ra);
    r->dma_channel.destinationBuffer(r->buffer, r->buffer_size);
    r->dma_channel.transferSize(2);
    r->dma_channel.transferCount(r->buffer_size / sizeof(i16));
    r->dma_channel.interruptAtCompletion();

    r->dma_channel.triggerAtHardwareEvent(DMAMUX_SOURCE_ADC0);
    r->dma_channel.enable();
    r->dma_channel.attachInterrupt(callback);
}

i16 dma_handle_read(DMAHandle *r) {
    i16 result = r->buffer[r->buffer_write_pos];
    return result;
}

