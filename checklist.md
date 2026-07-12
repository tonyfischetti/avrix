# Becoming a better embedded developer

A personal roadmap, calibrated against this repo: the architecture,
build hygiene, and hand-written assembly here are already past hobbyist
level. Nearly every real bug found in lamp-box/avril was one of two kinds —
a **concurrency invariant** broken across the ISR/main boundary, or an
**ambient contract** (ABI, sentinel, boundary) absorbed incompletely. This
list is ordered accordingly: the first tier is habits, not knowledge, and
would have prevented essentially all of them.

## Tier 1 — habits that would have caught this codebase's bugs

### 1. The atomicity audit

- [ ] For every project, write the *ownership table* before debugging ever
      forces you to: every variable touched by an ISR, one row each —
      who writes it, who reads it, from which context, and how wide it is.
      Anything wider than 1 byte crossing a context boundary on an 8-bit
      machine is a torn read/write until proven otherwise (`lastPressed`
      was 4 bytes; the pause()/resume() handshake was 2 statements).
- [ ] Train the reflex: for any two adjacent statements touching shared
      state, ask *"what breaks if an interrupt lands exactly here?"* Do it
      mechanically for a month and it becomes automatic. Re-derive the old
      `Ticker::pause()` race from memory as the kata.
- [ ] Learn the small set of standard resolutions cold, and reach for them
      by name: single-writer/single-reader ownership, atomic claim block
      (what the debouncer does), check-under-`cli()` before commit (what
      the sleep gate does), flag-clear-before-work (what `abortTxP` does).
- [ ] Default to the narrowest possible ISR: timestamp + record + return.
      Every line added to an ISR is a line that preempts everything else
      you've written. (The lamp's ISR used to run the entire input stack;
      removing it deleted a whole bug class.)

### 2. Adversarial boundary thinking

- [ ] For every arithmetic variable, ask: what happens at 0, at MAX, and
      one step past each? (The interval underflows, the `delay <= 65534`
      wrap, `interval * 5` overflowing 16-bit int.)
- [ ] For every sentinel, ask: can a legitimate value collide with it?
      (Tick 0 vs. `lockoutStart == 0` meaning "idle".)
- [ ] For every assert, prove it *can* fire: feed it a value that should
      fail, once. (The GPIO `static_assert`s were tautologies for years —
      `x < 8 || x > 1` — and nothing ever noticed.)
- [ ] For every "this can't happen" branch, write down why not. If the
      why-not involves timing or human speed, treat it as "this happens
      monthly."

### 3. Contracts are load-bearing: ABIs, datasheets, errata

- [ ] Read the avr-gcc calling convention page top to bottom until you can
      recite the call-saved set (r2–r17, r28/r29) and the argument
      registers. Do the same for any ABI you write assembly against —
      the boring lookup is the part that bit `flood_pixels`, not the
      cycle counting, which was flawless.
- [ ] Make "verify against the disassembly" a standard step, not a
      debugging step. You already generate `main-disasm.txt` on every
      build — add the habit of actually diffing it after touching asm or
      anything timing-sensitive.
- [ ] When using a peripheral, read its *entire* datasheet chapter once —
      not just the register table. The WDRF/WDE interaction and the
      timed-sequence requirements in the watchdog are typical of the traps
      that live in the prose between the tables.
- [ ] Know that errata sheets exist and check them when hardware behaves
      impossibly. Silicon has bugs too.

### 4. Test the pure logic on the host — your home-turf advantage

- [ ] Extract state machines from hardware: the debouncer and Button are
      pure functions of `(time, pin-state)` sequences and compile fine on
      a Mac. Build a tiny host harness that feeds them synthetic edge
      streams.
- [ ] Then use what a decade of Python gives you: property-based testing
      (Hypothesis) generating random press/bounce/tap sequences with the
      invariant "every physical press eventually produces exactly one
      PRESS and one RELEASE." The quick-tap swallow bug survives about
      ten seconds of that.
- [ ] Simulate time, never sleep in tests. The tick counter is just a
      uint32 you increment.
- [ ] Aspirational: run firmware under `simavr` for whole-program checks.
      Lower priority than host-testing the logic, which is where the bugs
      actually were.

## Tier 2 — deepen the foundations

### 5. Measure power, don't reason about it

- [ ] Get current measurement capability that spans µA to mA (a Joulescope
      or the µCurrent-into-multimeter trick; even a bare multimeter in µA
      mode validates sleep states).
- [ ] Measure the lamp: PWR_DOWN floor, IDLE-loop average, active
      animation. Compare against the datasheet's numbers. The stuck-pending
      bug (busy-spin instead of sleep) would have been visible on a meter
      in one minute, years ago.
- [ ] Learn the full power menu beyond sleep modes: peripheral power
      reduction (`PRR` — you already use `power_adc_disable`), clock
      prescaling at runtime, BOD disable during sleep, pullup discipline.

### 6. Better visibility into running systems

- [ ] Buy a cheap 8-channel logic analyzer (~$15, sigrok/PulseView).
      Decode your own SK6812 waveform and *see* the 375/750 ns pulses you
      cycle-counted. Watch the encoder bounce for real — knowing your
      actual switch bounces for 800 µs (or 8 ms) turns debounce-window
      choice from folklore into engineering.
- [ ] Build a debug output habit for the tiny85 (no UART): a spare-pin
      toggle viewed on the analyzer is a print statement that costs one
      cycle. Instrument ISR entry/exit and sleep entry once and you'll
      never guess about timing again.
- [ ] Learn on-chip debugging on a part that supports it properly
      (debugWIRE is miserable; SWD on ARM is a revelation).

### 7. Know what your C++ costs

- [ ] For each abstraction you use, know its object-code price: templates
      (free, but code-size multiplication per instantiation), virtual
      dispatch (vtable in flash + indirect call — fine for 8 patterns,
      know why), `Callback` function pointers vs. inlined lambdas,
      float (very not free on AVR).
- [ ] Read the generated code for one nontrivial function per project.
      You have the disassembly *right there*.
- [ ] Learn the linker's view: what lives in `.text`/`.data`/`.bss`, what
      `--gc-sections` can and can't remove, how to read the map file, and
      where the stack actually is (and what happens when it meets `.bss` —
      on a 512-byte-RAM part, this is not academic).

## Tier 3 — breadth that changes how you design

### 8. Learn one RTOS, even if you never ship one

- [ ] Do a small FreeRTOS project (queues, semaphores, task priorities).
      Not because the lamp needs it, but because the vocabulary — critical
      section, priority inversion, producer/consumer queue — is the
      formalization of exactly the discipline in Tier 1, worked out by
      people over decades. You'll come back to bare-metal writing better
      superloops.

### 9. Learn one modern 32-bit platform

- [ ] Port something to a Cortex-M part (RP2040 or an STM32). What it
      teaches by contrast: NVIC (preemptive, prioritized interrupts vs.
      the AVR's flat model), DMA (an SK6812 strip driven by DMA/PIO with
      the CPU asleep — the technique that dissolves your whole
      cycle-counting problem), and real debugging.
- [ ] Then decide deliberately when the tiny85's constraints are the
      charm and when they're the wrong tool. Both answers are right for
      different projects.

### 10. Round out the hardware side

- [ ] Be able to answer from first principles: why does the LED strip
      want a bulk capacitor and a series resistor on data? When does a
      5 V strip need level shifting from a 3.3 V (or marginal-VCC
      battery) MCU? How do you size a pullup? What does brownout
      detection protect you from (hint: EEPROM/flash corruption during
      sagging batteries)?
- [ ] Implement one protocol you normally get for free — bit-bang I2C or
      SPI from the datasheet's timing diagrams. Same muscle as the SK6812
      work, applied to a bidirectional protocol.

## Reading list

- [ ] avr-libc docs for `<util/atomic.h>` and the FAQ on shared variables
      (short, canonical, directly about Tier 1)
- [ ] The avr-gcc ABI / register-usage wiki page (Tier 1, item 3)
- [ ] Jack Ganssle, *A Guide to Debouncing* — the classic empirical study;
      compare its measured bounce data with your immediate-report design
- [ ] Elecia White, *Making Embedded Systems* — the best bridge from
      "experienced programmer" to "embedded engineer"; her state-machine
      and layering chapters will feel like validation of avril's design
- [ ] The ATtiny85 datasheet, chapters 7–9 (clocks, power management,
      interrupts) read *linearly* once
- [ ] John Regehr's blog posts on `volatile` and on embedded compiler
      bugs (sobering and practical)

## The meta-item

- [ ] After every bug that survives more than a day in the field, write
      the one-paragraph post-mortem: what invariant was broken, which
      habit above would have caught it, and whether the fix enforces the
      invariant or just patches the instance. The fix list from July 2026
      (nine bugs, seven of them concurrency-or-contract) is the founding
      dataset.
