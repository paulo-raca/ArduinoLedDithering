LED Dither
==========

Controlling LED brightness via PWM is awesome, but not available on every pin.

This hack implements controllable brightness by quickly performing a CPU-controlled blinking.

Since your CPU is probably busy doing something else, timming will probably not be accurate,
so this library will also use error propagation to compensate and create a mostly-seamless brightness adjustment.
