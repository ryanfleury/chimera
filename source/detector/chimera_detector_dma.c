
typedef struct DMAChannel {
    volatile const void * volatile SADDR;
    i16 SOFF;
    
    union { 
        u16 ATTR;
        struct { 
            u8 ATTR_DST; 
            u8 ATTR_SRC; 
        }; 
    };
    
    union { 
        u32 NBYTES; 
        u32 NBYTES_MLNO;
        u32 NBYTES_MLOFFNO; 
        u32 NBYTES_MLOFFYES; 
    };
    
    i32 SLAST;
    volatile void * volatile DADDR;
    i16 DOFF;
    union { 
        volatile u16 CITER;
        volatile u16 CITER_ELINKYES; 
        volatile u16 CITER_ELINKNO; 
    };
    
    i32 DLASTSGA;
    volatile u16 CSR;
    union { 
        volatile u16 BITER;
        volatile u16 BITER_ELINKYES; 
        volatile u16 BITER_ELINKNO; 
    };
} DMAChannel;