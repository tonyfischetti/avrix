
#define __SFR_OFFSET 0x00
#include <avr/io.h>

#define LED1    3
#define LED2    1
#define LED3    2


; ============================
; ATtiny85 vector table
; ============================
.section .vectors
.org 0x0000
    rjmp reset                ; RESET
    rjmp default_handler      ; INT0_vect
    rjmp default_handler      ; PCINT0_vect
    rjmp default_handler      ; TIMER1_COMPA_vect
    rjmp default_handler      ; TIMER1_OVF_vect
    rjmp default_handler      ; TIMER0_OVF_vect
    rjmp default_handler      ; EE_RDY_vect
    rjmp default_handler      ; ANA_COMP_vect
    rjmp default_handler      ; ADC_vect
    rjmp default_handler      ; TIMER1_COMPB_vect
    rjmp default_handler      ; TIMER0_COMPA_vect
    rjmp default_handler      ; TIMER0_COMPB_vect
    rjmp wd_handler           ; WDT_vect
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

    sbi PORTB, LED2

    ; MCUSR &= ~(1 << WDRF);
    in r16, MCUSR
    andi r16, ~(1 << WDRF)
    out MCUSR, r16

    ; WDTCR |= (1 << WDCE) | (1 << WDE);
    in r16, WDTCR
    ori r16, (1 << WDCE) | (1 << WDE)
    out WDTCR, r16


    ; WDTCR = (1 << WDIE) | prescaler_bits;
    ldi r16, (1 << WDIE) | (1 << WDP1) | (1 << WDP0)
    out WDTCR, r16

    sei

loop:
    ldi r16, (1 << SE) | (1 << SM1)
    out MCUCR, r16
    sleep
    rjmp loop


; ============================
; Interrupt Handlers
; ============================

wd_handler:
    sbi PINB, LED1 ; toggles
    sbi PINB, LED2 ; toggles
    sbi PINB, LED3 ; toggles
    reti

default_handler:
    reti

