#define __SFR_OFFSET 0x00
#include "avr/io.h"

; .section .text
.global send_byte
.global send_pixel_4_args


; WS2812 sendbyte routine, 8 MHz ATtiny85
; Sends r24 (byte) MSB first on PORTB.0
; Bit timing: 10 cycles per bit
;   Bit 0: HIGH 3 cycles, LOW 7
;   Bit 1: HIGH 6 cycles, LOW 4


send_byte:
    cli
    mov r16, r24

; ------- Bit 7 -------
.bit7:
    lsl r16
    sbi PORTB, 0
    brcs .bit7_1
    cbi PORTB, 0
    nop
    nop
    rjmp .bit6

.bit7_1:
    nop
    nop
    cbi PORTB, 0
    nop

; ------- Bit 6 -------
.bit6:
    lsl r16
    sbi PORTB, 0
    brcs .bit6_1
    cbi PORTB, 0
    nop
    nop
    rjmp .bit5

.bit6_1:
    nop
    nop
    cbi PORTB, 0
    nop

; ------- Bit 5 -------
.bit5:
    lsl r16
    sbi PORTB, 0
    brcs .bit5_1
    cbi PORTB, 0
    nop
    nop
    rjmp .bit4

.bit5_1:
    nop
    nop
    cbi PORTB, 0
    nop

; ------- Bit 4 -------
.bit4:
    lsl r16
    sbi PORTB, 0
    brcs .bit4_1
    cbi PORTB, 0
    nop
    nop
    rjmp .bit3

.bit4_1:
    nop
    nop
    cbi PORTB, 0
    nop

; ------- Bit 3 -------
.bit3:
    lsl r16
    sbi PORTB, 0
    brcs .bit3_1
    cbi PORTB, 0
    nop
    nop
    rjmp .bit2

.bit3_1:
    nop
    nop
    cbi PORTB, 0
    nop

; ------- Bit 2 -------
.bit2:
    lsl r16
    sbi PORTB, 0
    brcs .bit2_1
    cbi PORTB, 0
    nop
    nop
    rjmp .bit1

.bit2_1:
    nop
    nop
    cbi PORTB, 0
    nop

; ------- Bit 1 -------
.bit1:
    lsl r16
    sbi PORTB, 0
    brcs .bit1_1
    cbi PORTB, 0
    nop
    nop
    rjmp .bit0

.bit1_1:
    nop
    nop
    cbi PORTB, 0
    nop

; ------- Bit 0 -------
.bit0:
    lsl r16
    sbi PORTB, 0
    brcs .bit0_1
    cbi PORTB, 0
    sei
    ret

.bit0_1:
    nop
    nop
    cbi PORTB, 0
    sei
    ret

;  TODO  make better? common "end"?


send_pixel_4_args:
    sbi PORTB, 1   ; !!
    ;  TODO  stress that this is GRBW. re-ordering
    ; r24 red
    ; r22 green
    ; r20 blue
    ; r18 white
    mov r16, r24 ; r16 is red
    mov r17, r22 ; r17 is green
    mov r24, r17 ; r24 is now green
    mov r22, r16 ; r22 is now red
    rcall send_byte ; sending green
    mov r24, r22
    rcall send_byte ; sending red
    mov r24, r20
    rcall send_byte ; sending blue
    mov r24, r18
    rcall send_byte ; sending white
    cbi PORTB, 1
    ret

