
; we need __SFR_OFFSET 0x00 because we're using avr/io.h and it defines
; macros that require its definition

; for example....
; #define DDRB _SFR_IO8(0x17)
; #define _SFR_IO8(io_addr) ((io_addr) + __SFR_OFFSET)

#define __SFR_OFFSET 0x00
#include <avr/io.h>

.section .text
.global __start

; __start is just a convention. it's only "special" because
; we place it in 0x0000

.equ PBX, 3

.equ delayOuter, 125
.equ delayInner, 999
; total_cycles = delayOuter × (4 × delayInner + 5)
; 125 * (4 * 999) + 5 = 500,125 microseconds or 0.500125 seconds

__start:
    sbi DDRB, PBX ; set as output

loop:
    sbi PINB, PBX ; toggle
    rcall delay
    rjmp loop


delay:
    ldi r20, delayOuter

outer:
    ldi r30, lo8(delayInner)
    ldi r31, hi8(delayInner)

inner:
    sbiw r30, 1
    brne inner
    subi r20, 1
    brne outer
    ret

