
#define __SFR_OFFSET 0x00
#include "avr/io.h"

; .section .text
.global flip
.global sendbyte

; ATtiny85 @ 8 MHz
; Verified timings (without external clock)
; - 0 .bit: 0.5 µs HIGH + 0.833 µs LOW
; - 1 .bit: 0.833 µs HIGH + 0.5 µs LOW
;
;   nop should be 125 ns


; will be high for 250 ns
; not 125 ns
; because sbi and cbi take two cycles
flip0:
    sbi PORTB, 0
    cbi PORTB, 0
    rjmp flip

; 458 on the dopt
flip1:
    sbi PORTB, 0
    nop
    nop
    cbi PORTB, 0
    rjmp flip

; 875 (with variation)
flip:
    sbi PORTB, 0	; 2
    nop             ; 1 3
    nop             ; 1 4
    nop             ; 1 5
    nop             ; 1 6
    nop             ; 1 7
    cbi PORTB, 0
    rjmp flip






;;;                 ; #cycles ; description
sendbyte:
    cli
    mov r16, r24    ; ?    ; move passed byte into r16

.bit7:
    lsl r16         ; 1    ; shift MS.bit into carry flag
    brcs .bit7_1    ; 1-2  ; if SREG.C is set, send 1

.bit7_0:                   ; otherwise, send 0
    sbi PORTB, 0    ; 2
    nop             ; 1
    cbi PORTB, 0    ; 2
    rjmp .bit6      ; 2

.bit7_1:
    sbi PORTB, 0
    nop             ; 1
    nop             ; 1
    nop             ; 1
    nop             ; 1
    cbi PORTB, 0    ; 2

.bit6:
    lsl r16
    brcs .bit6_1
    nop

.bit6_0:
    sbi PORTB, 0
    nop
    cbi PORTB, 0
    rjmp .bit5

.bit6_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    cbi PORTB, 0

.bit5:
    lsl r16
    brcs .bit5_1
    nop

.bit5_0:
    sbi PORTB, 0
    nop
    cbi PORTB, 0
    rjmp .bit4

.bit5_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    cbi PORTB, 0

.bit4:
    lsl r16
    brcs .bit4_1
    nop

.bit4_0:
    sbi PORTB, 0
    nop
    cbi PORTB, 0
    rjmp .bit3

.bit4_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    cbi PORTB, 0

.bit3:
    lsl r16
    brcs .bit3_1
    nop

.bit3_0:
    sbi PORTB, 0
    nop
    cbi PORTB, 0
    rjmp .bit2

.bit3_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    cbi PORTB, 0

.bit2:
    lsl r16
    brcs .bit2_1
    nop

.bit2_0:
    sbi PORTB, 0
    nop
    cbi PORTB, 0
    rjmp .bit1

.bit2_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    cbi PORTB, 0

.bit1:
    lsl r16
    brcs .bit1_1
    nop

.bit1_0:
    sbi PORTB, 0
    nop
    cbi PORTB, 0
    rjmp .bit0

.bit1_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    cbi PORTB, 0

.bit0:
    lsl r16
    brcs .bit0_1
    nop

.bit0_0:
    sbi PORTB, 0
    nop
    cbi PORTB, 0
    sei
    ret

.bit0_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    cbi PORTB, 0
    sei
    ret


