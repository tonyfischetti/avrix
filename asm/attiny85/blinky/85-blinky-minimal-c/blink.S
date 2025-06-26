
#define __SFR_OFFSET 0x00
#include "avr/io.h"

; .section .text
.global start
.type start, @function
.global blink


start:
    sbi  DDRB, 3  ; set PB3 as output (set bit in IO register) - cbi clears it
    ; sbi PORTB, 3
    sbi  DDRB, 1
    ; sbi PORTB, 1
    sbi  DDRB, 2
    ; sbi PORTB, 2
    ret

blink:
  cpi  r24, 0x00         ;  compare argument (r24) to 0
  breq ledOFF           ; branch to sub ledOFF if it was equal
  sbi  PORTB, 3          ; if not, turn it on
  rcall myDelay
  ret

ledOFF:
  cbi  PORTB, 3   ; set to low
  rcall myDelay
  ret

.equ delayOuter, 125
.equ delayInner, 999
; total_cycles = delayOuter × (4 × delayInner + 5)
; 125 * (4 * 999) + 5 = 500,125 microseconds or 0.500125 seconds


; Delay for approximately delayOuter × (4 × delayInner + 5) clock cycles
; Assumes 1 MHz system clock (1 cycle = 1 µs)
; Total delay in µs = outer × (4 × inner + 5)

myDelay:
    ldi  r20, delayOuter       ; 1 cycle, only executed once (not included in per-iteration cost)
    
outerLoop:
    ldi  r30, lo8(delayInner)  ; 1 cycle → setup for inner loop (OVERHEAD #1)
    ldi  r31, hi8(delayInner)  ; 1 cycle → setup for inner loop (OVERHEAD #2)

innerLoop:
    sbiw r30, 1                ; 2 cycles → 16-bit subtract on r31:r30
    brne innerLoop             ; 2 cycles (branch taken) or 1 (if falling through) → assume 2 for all but last
                               ; → Inner loop = 4 cycles per iteration (2 + 2)

    subi r20, 1                ; 1 cycle → decrement outer loop counter (OVERHEAD #3)
    brne outerLoop             ; 2 cycles (branch taken) or 1 → assume 2 if not last (OVERHEAD #4)
                               ; → Outer loop overhead = 5 cycles total (1 + 1 + 1 + 2)
    ret                        ; 4–5 cycles depending on architecture, but not part of loop body
