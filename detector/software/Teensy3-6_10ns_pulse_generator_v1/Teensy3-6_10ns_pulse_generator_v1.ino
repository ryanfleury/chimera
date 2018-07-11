static inline void delayNanoseconds(uint32_t usec) {
#if F_CPU == 240000000
  uint32_t n = usec * 80;
#elif F_CPU == 216000000
  uint32_t n = usec * 72;
#elif F_CPU == 192000000
  uint32_t n = usec * 64;
#elif F_CPU == 180000000
  uint32_t n = usec * 2;
#elif F_CPU == 168000000
  uint32_t n = usec * 56;
#elif F_CPU == 144000000
  uint32_t n = usec * 48;
#elif F_CPU == 120000000
  uint32_t n = usec * 40;
#elif F_CPU == 96000000
  uint32_t n = usec << 5;
#elif F_CPU == 72000000
  uint32_t n = usec * 24;
#elif F_CPU == 48000000
  uint32_t n = usec << 4;
#elif F_CPU == 24000000
  uint32_t n = usec << 3;
#elif F_CPU == 16000000
  uint32_t n = usec << 2;
#elif F_CPU == 8000000
  uint32_t n = usec << 1;
#elif F_CPU == 4000000
  uint32_t n = usec;
#elif F_CPU == 2000000
  uint32_t n = usec >> 1;
#endif
    // changed because a delay of 1 micro Sec @ 2MHz will be 0
  if (n == 0) return;
  __asm__ volatile(
    "L_%=_delayMicroseconds:"   "\n\t"
#if F_CPU < 24000000
    "nop"         "\n\t"
#endif
#ifdef KINETISL
    "sub    %0, #1"       "\n\t"
#else
    "subs   %0, #1"       "\n\t"
#endif
    "bne    L_%=_delayMicroseconds"   "\n"
    : "+r" (n) :
  );
}



void setup() {
  // put your setup code here, to run once:
  pinMode(24, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

//  digitalWriteFast(24,HIGH);
//  __asm__ volatile("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
//  digitalWriteFast(24,LOW);
//  __asm__ volatile("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
  while (1) {
    digitalWriteFast(24,HIGH);
    delayNanoseconds(1);
    digitalWriteFast(24,LOW);
    delayNanoseconds(1);
  }

}
