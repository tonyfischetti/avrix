
#define __SFR_OFFSET 0x00
#include "avr/io.h"

; .section .text
.global sendbyte

; ATtiny85 @ 8 MHz
; Verified timings (without external clock)
; - 0 bit: 0.5 µs HIGH + 0.833 µs LOW
; - 1 bit: 0.833 µs HIGH + 0.5 µs LOW
;
;   nop should be 125 ns


sendbyte:
    mov r16, r24        ; move passed byte into r16
bit7:
    lsl r16             ; shift MSBit into carry flag
    brcs bit7_1         ; if SREG.C is set, send 1
bit7_0:                 ; otherwise, send 0
    sbi PORTB, 0
    nop
    nop
    cbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    rjmp bit6
bit7_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    cbi PORTB, 0
    nop
    nop

bit6:
    lsl r16
    brcs bit6_1
bit6_0:
    sbi PORTB, 0
    nop
    nop
    cbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    rjmp bit5
bit6_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    cbi PORTB, 0
    nop
    nop

bit5:
    lsl r16
    brcs bit5_1
bit5_0:
    sbi PORTB, 0
    nop
    nop
    cbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    rjmp bit4
bit5_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    cbi PORTB, 0
    nop
    nop

bit4:
    lsl r16
    brcs bit4_1
bit4_0:
    sbi PORTB, 0
    nop
    nop
    cbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    rjmp bit3
bit4_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    cbi PORTB, 0
    nop
    nop

bit3:
    lsl r16
    brcs bit3_1
bit3_0:
    sbi PORTB, 0
    nop
    nop
    cbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    rjmp bit2
bit3_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    cbi PORTB, 0
    nop
    nop

bit2:
    lsl r16
    brcs bit2_1
bit2_0:
    sbi PORTB, 0
    nop
    nop
    cbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    rjmp bit1
bit2_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    cbi PORTB, 0
    nop
    nop

bit1:
    lsl r16
    brcs bit1_1
bit1_0:
    sbi PORTB, 0
    nop
    nop
    cbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    rjmp bit0
bit1_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    cbi PORTB, 0
    nop
    nop

bit0:
    lsl r16
    brcs bit0_1
bit0_0:
    sbi PORTB, 0
    nop
    nop
    cbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    ret
bit0_1:
    sbi PORTB, 0
    nop
    nop
    nop
    nop
    nop
    cbi PORTB, 0
    nop
    nop
    ret



;; While the below is nice,
;; I was getting HIGH times of 458 and 833 (send0 and send1, respectively)
;; and LOW times of 1666 and 1160
;; this is due to the overhead with bit shifting and testing.
;; The one to use is the unrolled version above

; sendbyte:
;     mov r16, r24        ; store passed byte in r16
;     ldi r17, 8          ; r17 will hold the bit counter
; sendbyte_loop:
;     lsl r16
;     brcs sendbit1
; sendbit0:
;     ; 500 ns
;     sbi PORTB, 0
;     nop
;     nop
;     ; 833 ns
;     cbi PORTB, 0
;     nop
;     nop
;     nop
;     nop
;     nop
;     rjmp continue
; sendbit1:
;     ; 833 ns
;     sbi PORTB, 0
;     nop
;     nop
;     nop
;     nop
;     nop
;     ; 500 ns
;     cbi PORTB, 0
;     nop
;     nop
; continue:
;     dec r17
;     brne sendbyte_loop
;     ret


