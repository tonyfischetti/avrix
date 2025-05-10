

- `85-blinky-minimal-c/` is a blink example that uses mostly
assembly but a C scaffolding


- `85-blinky-only-asm/` same example but without the 
C scaffolding

It doesn't include the C runtime, which would otherwise
  - set up the stack
  - initialize static or global variables
  - call global C++ constructors
  - calls `main()`


- `85-blinky-asm-standalone/` like "only-asm" but goes a step further
and doesn't use the C preprocessor. So you can't include <avr/io.h> and
use things like `DDRB` and `PORTB`

It also doesn't use `-Ttext=0` and, instead, uses an explicit reset vector

