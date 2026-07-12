# lamp-box

Firmware for a battery-friendly RGBW mood lamp: an ATtiny85 driving an
SK6812 strip, controlled entirely by one clickable rotary encoder. Built on
[avril](../avril/README.md), this repo's AVR HAL.

- [Hardware](#hardware)
- [Controls](#controls)
- [Building and flashing](#building-and-flashing)
- [Firmware architecture](#firmware-architecture)
  - [The main loop](#the-main-loop)
  - [Input event flow](#input-event-flow)
  - [The abort/blank mechanism](#the-abortblank-mechanism)
  - [Power strategy](#power-strategy)
- [The NeoPixel driver](#the-neopixel-driver)
- [Patterns](#patterns)
  - [Adding a pattern](#adding-a-pattern)
- [Compile-time configuration](#compile-time-configuration)

## Hardware

| MCU pin (DIP) | Port | Function |
|---|---|---|
| 5 | PB0 | SK6812 data out (`NEO`) |
| 6 | PB1 | status LED (reserved, currently unused) |
| 7 | PB2 | encoder CLK |
| 2 | PB3 | encoder DT |
| 3 | PB4 | encoder push button |

- **MCU**: ATtiny85 @ 8 MHz internal RC oscillator (`lfuse 0xE2` — see
  `make fuses`). The NeoPixel bit-banging is cycle-counted for exactly this
  clock; changing it means redoing `neopixel-asm.S`.
- **LEDs**: SK6812 RGBW, arranged as rows of 8. The framebuffer is sized by
  `TOTAL_ROWS` in `main.cpp` — note its slightly tricky meaning: it counts
  the *states* of the row selector including "0 rows / off", so
  `TOTAL_ROWS = 5` means **4 physical rows, 32 pixels, 128 channels**.
- **Encoder**: common KY-040-style clickable quadrature encoder, everything
  on internal pullups, so the knob wires straight to the chip. All three
  inputs are on PORTB (they must be: the single `PCINT0_vect` services them).

## Controls

One knob does everything. The grammar:

| Gesture | Action |
|---|---|
| rotate | adjust the current pattern (brightness or speed, pattern-defined) |
| click (press + release) | step the pattern's row count: 1 → 2 → 3 → 4 → 0 (off) → 1 … |
| press **+** rotate | next / previous pattern |
| long press (1 s) | next pattern (one-handed alternative) |

Two deliberate subtleties: a long press suppresses the release that follows
it (so it doesn't also step the row count), and so does a pressed-rotate —
one physical gesture, one logical action. Both suppressions come from avril,
not from code here.

## Building and flashing

Requires avr-gcc (developed against 14.x) and avrdude, with `avril` checked
out as a sibling directory (it is, if you cloned the parent repo with
submodules).

```sh
make            # build/main.hex, plus a disassembly in build/main-disasm.txt
make flash      # avrdude via usbtiny ISP
make fuses      # one-time: 8 MHz internal oscillator (lfuse 0xE2)
make clean
```

The build is warning-clean under an aggressive flag set (`-Wall -Wextra
-Wpedantic -Wshadow -Weffc++ -Wsign-conversion ...` — see the Makefile) and
links with LTO + `--gc-sections`. Current footprint: ~4.5 KB flash, ~210
bytes of static RAM (the 128-byte framebuffer being the biggest item) —
comfortable on the tiny85's 8 KB / 512 B.

`build/main-disasm.txt` is generated on every build on purpose: with
hand-written assembly and cycle-counted timing in the loop, being one
`grep` away from what the compiler actually emitted has paid for itself
repeatedly.

## Firmware architecture

### The main loop

Everything happens in one loop in `main.cpp`; there is no scheduler and no
dynamic allocation. Patterns are static objects implementing a tiny
polymorphic interface (`Pattern`), and the loop's contract with them is a
single method:

```cpp
uint16_t delayTimeP = CURRENT_PATTERN->tick();   // draw one frame,
latch();                                         // then: how long until the
                                                 // next one? (0 = "I'm static")
```

That return value drives the whole power strategy:

- **`tick()` returned N > 0** (animated pattern): the loop paces the next
  frame by sleeping in `SLEEP_MODE_IDLE`, waking on each 1 ms Ticker
  interrupt to poll the encoder, until N ticks have elapsed. CPU clock off
  for the overwhelming majority of every frame delay.
- **`tick()` returned 0** (static pattern, or a pattern showing 0 rows):
  nothing will change until the human touches the knob, so the loop drops
  into `SLEEP_MODE_PWR_DOWN` — microamp territory. The Ticker is paused
  first (Timer0 stops in power-down anyway; pausing makes it explicit and
  compensable), and only a pin-change interrupt can wake the chip.

The power-down gate is done under `cli()` and re-checked there: deciding to
sleep and going to sleep must be atomic, or an input event can land in the
gap and get slept through. The IDLE path needs no such ceremony — the next
tick wakes it within a millisecond no matter what.

### Input event flow

avril's concurrency contract, followed to the letter:

```
encoder edge ──► ISR(PCINT0_vect):  resume ticker, timestamp, notify. that's it.
                        │
                        ▼
main loop ──► checkPeripherals() ──► knob.process() ──► callbacks:
                  (≥ 1x per ms,          nextPattern() / previousPattern()
                   every context)        CURRENT_PATTERN->onCW()/onCCW()/onRelease()
```

The ISR never runs pattern code or state machines — it stamps the edge and
returns. All callbacks execute in main-loop context, which is why
`currentPatternIndex` and all pattern state need no `volatile` and no
atomics. The only ISR/main shared state is `abortTxP` (a volatile flag,
below) and the debouncer internals (which handle their own atomicity).

### The abort/blank mechanism

`abortTxP` is set by the ISR on *every* input edge and consumed at the top
of the loop:

```cpp
if (abortTxP) {
    abortTxP = false;          // clear FIRST: a change arriving mid-flood
    flood_pixels(0,0,0,0, N);  // re-raises the flag and gets its own blank
    latch();                   // next lap, instead of being erased
}
```

It solves two problems with one flag. First, an interrupt that fires while a
frame is mid-transmission stretches the data line's quiet gap; if that gap
exceeds the SK6812 latch time, the strip displays a torn frame — so any
input invalidates the frame in flight and forces a clean redraw. Second, a
pattern *change* needs a blank slate: patterns are allowed to assume the
strip is dark outside the pixels they explicitly paint (NCC-1701 paints one
row and nothing else), and the blanking flood is what makes that assumption
true.

### Power strategy

Summarized, since the pieces are described above:

| Situation | Mode | Wake source | Order of magnitude |
|---|---|---|---|
| animating, between frames | `IDLE` | Timer0 tick (1 ms) or pin change | ~1–2 mA (MCU) |
| static display / off rows | `PWR_DOWN` | pin change only | ~µA (MCU) |
| debounce in flight or button held | awake | — | brief, bounded |

The honest caveat: when pixels are lit, LED current dwarfs all of this. The
sleep discipline pays off when the lamp is dark or dim — which, for a lamp
that spends most of its life as a nightlight showing one warm row, is most
of the time.

## The NeoPixel driver

`src/neopixel-asm.S` bit-bangs the SK6812 protocol on PB0. Hand-written
assembly, because at 8 MHz there is no slack for a compiler's opinion:
each bit is exactly **10 cycles = 1.25 µs**, with the high-time encoding
the bit value:

| bit | high | low | spec (SK6812) |
|---|---|---|---|
| 0 | 3 cycles = 375 ns | 7 cycles | T0H = 300 ± 150 ns |
| 1 | 6 cycles = 750 ns | 4 cycles | T1H = 600 ± 150 ns |

Three entry points, exposed to C++ via `neopixel.hpp`:

```cpp
send_byte(b);                       // one byte, MSB first
send_pixel(r, g, b, w);             // one pixel, channel order per PIXEL_ORDER_*
flood_pixels(r, g, b, w, n);        // n identical pixels (blanks, solid fills)
latch();                            // 80 µs low: commit the frame (SK6812 spec)
```

Design/discipline notes, learned the hard way:

- **Interrupts are disabled per byte, not per frame.** `send_byte` snapshots
  SREG, runs `cli` for its 80 cycles (10 µs), and restores the caller's
  interrupt state on exit. So the ticker keeps ticking and the encoder stays
  live during a frame, at the cost of small inter-byte gaps — which are µs,
  far below the 80 µs latch threshold. A torn frame from a *long* ISR is
  handled by the abort mechanism rather than by locking the world.
- **The routines are strictly ABI-clean**: scratch registers are drawn only
  from the avr-gcc call-clobbered set (r18–r27, r30/r31), so no push/pop
  overhead and no possibility of corrupting compiler-allocated state. The
  comments in the `.S` file document which registers each routine uses;
  keep that contract if you touch it.
- Frames are *transmitted*, not stored, except where a pattern needs
  read-modify-write: `pixelArray` (128 bytes, GRBW channel-major) exists for
  the framebuffer patterns (Candle, NCC-1701); the solid-color patterns
  skip RAM entirely and `flood_pixels` straight to the wire.

## Patterns

| Pattern | What it does | rotate adjusts |
|---|---|---|
| `NCC_1701Pattern` | two-dot symmetric scanner, warm white (yes, like the nacelles) | brightness |
| `CandlePattern` | LFSR-driven flicker with jumping seed and random dropouts | flicker speed |
| `WarmLightPattern` | pure white channel | brightness |
| `WarmColorPattern2` / `3` | warm white mixes (two color temperatures) | brightness |
| `SpectrumPattern` | continuous RGB crossfade | fade speed |
| `ChooseAColorPattern` | crossfade you drive manually — rotate to walk the hue wheel | hue |
| `BisexualSwitchPattern` | pink / purple / blue color cycling | switch speed |
| `WarmStrobePattern` | white strobe | strobe speed |

All row-based patterns share the click-to-cycle-rows behavior
(`numRows = (numRows + 1) % TOTAL_ROWS`), and all encoder-adjusted intervals
are clamped so they can't underflow to 0 (which would read as "static,
power down" and freeze the animation) or wrap to 65535.

A note on the candle, the most algorithmically interesting one: each frame
draws every pixel from a weighted 4-bucket distribution (55% base flame /
27% + 12% dimmer / 6% out) using the 8-bit LFSR, occasionally blacks out a
random 3-pixel run for a "gutter" effect, and — the good trick —
**increments the LFSR seed every frame**. A fixed 255-state LFSR loops
visibly; the jumping seed turns a toy generator into flicker that never
noticeably repeats.

### Adding a pattern

1. Create `include/patterns/MyPattern.hpp` implementing `Pattern`:

```cpp
#pragma once
#include <stdint.h>
#include "neopixel.hpp"
#include "Pattern.hpp"

template<uint8_t TOTAL_ROWS>
struct MyPattern final : Pattern {
    uint16_t tick() override {
        uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };
        if (totalPixels == 0) { flood_pixels(0,0,0,0, TOTAL_ROWS*8); return 0; }
        flood_pixels(/* ... */, totalPixels);
        return 33;                       // ms until next frame; 0 = static
    }
    void onCW() override  { /* clamped! */ }
    void onCCW() override { /* clamped! */ }
    void onRelease() override { numRows = (numRows + 1) % TOTAL_ROWS; }
  private:
    uint8_t numRows { 1 };
};
```

2. Add it to `patterns.hpp`, instantiate it in `main.cpp`, and append the
   pointer to `patternList`. Order in the list is the cycling order.

Rules a pattern must respect: `tick()` draws **one** frame and returns
promptly (the loop, not the pattern, owns time); returning 0 means "put the
MCU into power-down until input" — only do it when genuinely static; don't
assume pixels you didn't paint are anything in particular *except* after a
pattern switch, when the blanking flood guarantees dark; clamp anything the
encoder adjusts.

## Compile-time configuration

All in the Makefile / `main.cpp`:

| Knob | Where | Meaning |
|---|---|---|
| `MCU`, `F_CPU` | Makefile | attiny85 @ 8 MHz; the asm depends on this clock |
| `NEO_PORT`, `NEO_BIT` | Makefile | where the strip's data line lives (PORTB, bit 0) |
| `PIXEL_ORDER_GRBW` / `_RGBW` | Makefile | channel order on the wire, per your strip variant |
| `TOTAL_ROWS` | `main.cpp` | row-selector states *including off*: N ⇒ N−1 physical rows |
| `LATCH_TIME_US` | `neopixel.hpp` | 80 µs, per SK6812 datasheet |
| encoder/button pins & timings | `main.cpp` | template args to `RotaryEncoderWithButton` |
