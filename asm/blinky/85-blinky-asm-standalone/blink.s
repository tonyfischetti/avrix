
.section .vectors
.org 0x0000
    rjmp reset

.section .text
.global reset

.equ DDRB, 0x17
.equ PINB, 0x16
.equ PBX, 3

.equ delayOuter, 125
.equ delayInner, 999
; total_cycles = delayOuter × (4 × delayInner + 5)
; 125 * (4 * 999) + 5 = 500,125 microseconds or 0.500125 seconds


reset:
    sbi DDRB, PBX ; PB3 as output

loop:
    sbi PINB, PBX ; toggles
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

