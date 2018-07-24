typedef void (*DMACallback)(void);

typedef struct DMARingBuffer {
    
    volatile i16 *buffer;
    u32 buffer_size;
    u32 start, end;
    volatile u32 *adc_ra;
    DMAChannel *dma_channel;
    u8 adc_num;
    
} DMARingBuffer;

DMARingBuffer dma_ring_buffer_init(volatile i16 *buffer,
                                   u32 buffer_size,
                                   u8 adc_num) {
    DMARingBuffer r = {0};
    
    r.buffer = buffer;
    r.buffer_size = buffer_size;
    r.start = 0;
    r.end = 0;
    r.adc_num = adc_num;
    r.adc_ra = &ADC0_RA + (u32)0x20000*adc_num;
    r.dma_channel = new DMAChannel();
    
    return r;
}

void dma_ring_buffer_clean_up(DMARingBuffer *r) {
    r->dma_channel->detachInterrupt();
    r->dma_channel->disable();
    delete r->dma_channel;
}

void dma_ring_buffer_start(DMARingBuffer *r, DMACallback callback) {
    r->dma_channel->source(*r->adc_ra);
    r->dma_channel->destinationCircular(r->buffer, r->buffer_size);
    r->dma_channel->transferSize(2);
    r->dma_channel->transferCount(r->buffer_size / sizeof(i16));
    r->dma_channel->interruptAtCompletion();
    r->dma_channel->triggerAtHardwareEvent(DMAMUX_SOURCE_PORTB);
    r->dma_channel->attachInterrupt(callback);
}

i16 dma_ring_buffer_read(DMARingBuffer *r) {
    i16 result = 0;
    
    if(r->end != r->start) {
        result = r->buffer[r->start & (r->buffer_size-sizeof(i16))];
        r->start = (r->start + 1) & (r->buffer_size-1);
    }
    
    return result;
}

void dma_ring_buffer_write(DMARingBuffer *r) {
    if(r->end == r->start) {
        r->start = (r->start + 1) & (r->buffer_size-1);
        r->end = (r->end + 1) & (r->buffer_size-1);
    }
    
    r->dma_channel->clearInterrupt();
}