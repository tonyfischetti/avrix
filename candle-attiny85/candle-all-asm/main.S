#define __SFR_OFFSET 0x00
#include <avr/io.h>

; ============================
; Constants
; ============================

.set LED1, 3       ; PB3
.set LED2, 1       ; PB1
.set LED3, 2       ; PB2

; ============================
; LFSR State Storage
; ============================
.section .bss
.global lfsr_state
lfsr_state:
    .skip 1         ; Reserve 1 byte for state

; ============================
; Interrupt Vector Table
; ============================
.section .vectors
.org 0x0000
    rjmp reset               ; RESET
    rjmp default_handler     ; INT0_vect
    rjmp default_handler     ; PCINT0_vect
    rjmp default_handler     ; TIMER1_COMPA_vect
    rjmp default_handler     ; TIMER1_OVF_vect
    rjmp default_handler     ; TIMER0_OVF_vect
    rjmp default_handler     ; EE_RDY_vect
    rjmp default_handler     ; ANA_COMP_vect
    rjmp default_handler     ; ADC_vect
    rjmp default_handler     ; TIMER1_COMPB_vect
    rjmp default_handler     ; TIMER0_COMPA_vect
    rjmp default_handler     ; TIMER0_COMPB_vect
    rjmp wd_handler          ; WDT_vect
    rjmp default_handler     ; USI_START_vect
    rjmp default_handler     ; USI_OVF_vect

; ============================
; Main Program
; ============================
.section .text
.global reset
.type reset, @function

reset:
    ; Setup stack
    ldi r16, 0xFF   ; RAMEND = 0xFF (512 - 1)
    out SPL, r16

    ; Set PB1–PB3 as output (LEDs)
    sbi DDRB, LED1
    sbi DDRB, LED2
    sbi DDRB, LED3

    ; Seed LFSR with a non-zero value
    ldi r16, 93
    sts lfsr_state, r16

    ; --- Watchdog Setup ---
    cli
    in r16, MCUSR
    andi r16, 0xF7           ; Clear WDRF
    out MCUSR, r16

    ; Enable timed sequence
    ldi r16, (1 << WDCE) | (1 << WDE)
    out WDTCR, r16

    ; Set interrupt mode only, prescaler ~0.25s (WDP1 | WDP0)
    ldi r16, (1 << WDIE) | (1 << WDP1) | (1 << WDP0)
    ; ldi r16, (1 << WDIE) | (1 << WDP3)
    out WDTCR, r16
    sei

main_loop:
    ldi r16, (1 << SE) | (1 << SM1)  ; Power-down sleep mode
    out MCUCR, r16
    sleep
    rjmp main_loop



; ============================
; Watchdog Interrupt Handler
; ============================
.type wd_handler, @function
wd_handler:
    rcall get_next_byte
    mov r16, r24        ; r16 = random byte

    in r17, PORTB
    ; clear LED1–3 bits
    andi r17, ~( (1 << LED1) | (1 << LED2) | (1 << LED3) )

    ; --- LED1: if (r & 0x03)
    mov r18, r16
    andi r18, 0x03
    tst r18
    breq skip_led1
    ori r17, (1 << LED1)
skip_led1:

    ; --- LED2: if (r & (0x03 << 2))
    mov r18, r16
    andi r18, 0x0C
    tst r18
    breq skip_led2
    ori r17, (1 << LED2)
skip_led2:

    ; --- LED3: if ((r & 0x30) || !(r & 0x0F))
    mov r18, r16
    andi r18, 0x30
    tst r18
    brne set_led3

    mov r18, r16
    andi r18, 0x0F
    tst r18
    breq set_led3
    rjmp skip_led3

set_led3:
    ori r17, (1 << LED3)
skip_led3:

    out PORTB, r17
    reti





; ============================
; Default ISR
; ============================
default_handler:
    reti

; ============================
; LFSR: 8-bit, X^8 + X^6 + X^5 + X^4 + 1
; ============================

get_next_byte:
    clr r17                 ; result byte
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    mov r24, r17
    ret

.type get_next_bit, @function
get_next_bit:
    clr r19
    lds r16, lfsr_state
    mov r18, r16

    ; XOR taps: 0, 2, 3, 4
    eor r19, r16
    lsr r16
    lsr r16
    eor r19, r16
    lsr r16
    eor r19, r16
    lsr r16
    eor r19, r16
    lsr r16
    lsr r16
    lsr r16

    lsr r19
    ror r18
    rol r17
    sts lfsr_state, r18
    ret
