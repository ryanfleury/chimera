# PIT-eDMA-demo
An Arduino / Teensyduino sketch for Teensy 3.1 / 3.2, 3.5, 3.6 eDMA demo for generating arbitrary pattern.

The code uses DMA channel 0, and Periodic Interrupt Timer (PIT) 0.

The output port is D13 (LED_BUILTIN).

Port D3 is toggled at PIT 0 frequency.

Port D4 is toggled at the end of PATTERN_LENGTH (major loop count) transfers.

This code works on Teensy 3.6.

It has not been tested on Teensy 3.5.

**IT DOES NOT WORK ON TEENSY 3.1, THE MICRO-CONTROLLER IS BUGGY.**

On Teensy 3.1, transfers are performed continuously, independantly from the required PIT frequency (so port D4 is toggled at 150 kHz).

See Mask Set Errata for Mask 1N36B: [https://www.nxp.com/docs/en/errata/KINETIS_K_1N36B.pdf](),
erratum ID e4588: *Instead of sending a single DMA request every time the PIT expires, the first time the PIT triggers a DMA transfer the “always enabled” source will not negate its request. This results in the DMA request remaining asserted continuously after the first trigger*.
