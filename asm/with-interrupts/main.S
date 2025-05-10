
#define __SFR_OFFSET 0x00
#include <avr/io.h>

#define LED1    3
#define LED2    1
#define LED3    2
#define SW      4

#define delayOuter 126
#define delayInner 999


; ============================
; Vector Table (ATtiny85)
; ============================
.section .vectors
.org 0x0000
    rjmp reset                ; RESET
    rjmp default_handler      ; INT0_vect
    rjmp pcint_handler        ; PCINT0_vect
    rjmp default_handler      ; TIMER1_COMPA_vect
    rjmp default_handler      ; TIMER1_OVF_vect
    rjmp default_handler      ; TIMER0_OVF_vect
    rjmp default_handler      ; EE_RDY_vect
    rjmp default_handler      ; ANA_COMP_vect
    rjmp default_handler      ; ADC_vect
    rjmp default_handler      ; TIMER1_COMPB_vect
    rjmp default_handler      ; TIMER0_COMPA_vect
    rjmp default_handler      ; TIMER0_COMPB_vect
    rjmp default_handler      ; WDT_vect
    rjmp default_handler      ; USI_START_vect
    rjmp default_handler      ; USI_OVF_vect


; ============================
; Main Program
; ============================
.section .text
.global reset
reset:
    sbi DDRB, LED1
    sbi DDRB, LED2
    sbi DDRB, LED3
    cbi DDRB, SW 
    sbi PORTB, SW
    ; pin change interrupt on PB4
    ldi r16, (1 << PCIE)
    out GIMSK, r16
    ldi r16, (1 << PCINT4)
    out PCMSK, r16
    sei

loop:
    sbi PINB, LED1
    rcall delay
    rjmp loop

; ============================
; Dummy Interrupt Handlers
; ============================

pcint_handler:
    sbi PINB, LED2
    reti

default_handler:
    ; Catch-all: just return
    reti

; ============================
; Crude Delay Loop (~visible blink)
; ============================
delay:
    ldi r20, delayOuter

outer:
    ldi r30, lo8(delayInner)
    ldi r31, hi8(delayInner)

inner:
    sbiw r30, 1
    brne inner
    dec r20
    brne outer
    ret

