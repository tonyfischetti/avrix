
#define __SFR_OFFSET 0x00
#include "avr/io.h"

; .section .text
.global flip
.global sendbyte

; ATtiny85 @ 8 MHz
; Verified timings (without external clock)
; - 0 bit: 0.5 µs HIGH + 0.833 µs LOW
; - 1 bit: 0.833 µs HIGH + 0.5 µs LOW
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
    mov r16, r24    ; ?       ; move passed byte into r16

bit7:
    lsl r16         ; 1    ; shift MSBit into carry flag
    brcs bit7_1     ; 1-2  ; if SREG.C is set, send 1

bit7_0:                    ; otherwise, send 0
    sbi PORTB, 0    ; 2
    nop             ; 1
    cbi PORTB, 0    ; 2
    rjmp bit6       ; 2

bit7_1:
    sbi PORTB, 0
    nop             ; 1
    nop             ; 1
    nop             ; 1
    nop             ; 1
    cbi PORTB, 0    ; 2

bit6:
    lsl r16         ; 1
    brcs bit6_1     ; 1-2
    nop

bit6_0:
    sbi PORTB, 0    ; 2
    nop             ; 1
    cbi PORTB, 0    ; 2
    rjmp bit5       ; 2

bit6_1:
    sbi PORTB, 0    ; 2
    nop             ; 1
    nop             ; 1
    nop             ; 1
    nop             ; 1
    cbi PORTB, 0    ; 2

bit5:
    lsl r16         ; 1
    brcs bit5_1     ; 1-2
    nop

bit5_0:
    sbi PORTB, 0    ; 2
    nop             ; 1
    cbi PORTB, 0    ; 2
    rjmp bit4       ; 2

bit5_1:
    sbi PORTB, 0    ; 2
    nop             ; 1
    nop             ; 1
    nop             ; 1
    nop             ; 1
    cbi PORTB, 0    ; 2

bit4:
    lsl r16         ; 1
    brcs bit4_1     ; 1-2
    nop

bit4_0:
    sbi PORTB, 0    ; 2
    nop             ; 1
    cbi PORTB, 0    ; 2
    rjmp bit3       ; 2

bit4_1:
    sbi PORTB, 0    ; 2
    nop             ; 1
    nop             ; 1
    nop             ; 1
    nop             ; 1
    cbi PORTB, 0    ; 2

bit3:
    lsl r16         ; 1
    brcs bit3_1     ; 1-2
    nop

bit3_0:
    sbi PORTB, 0    ; 2
    nop             ; 1
    cbi PORTB, 0    ; 2
    rjmp bit2       ; 2

bit3_1:
    sbi PORTB, 0    ; 2
    nop             ; 1
    nop             ; 1
    nop             ; 1
    nop             ; 1
    cbi PORTB, 0    ; 2

bit2:
    lsl r16         ; 1
    brcs bit2_1     ; 1-2
    nop

bit2_0:
    sbi PORTB, 0    ; 2
    nop             ; 1
    cbi PORTB, 0    ; 2
    rjmp bit1       ; 2

bit2_1:
    sbi PORTB, 0    ; 2
    nop             ; 1
    nop             ; 1
    nop             ; 1
    nop             ; 1
    cbi PORTB, 0    ; 2

bit1:
    lsl r16         ; 1
    brcs bit1_1     ; 1-2
    nop

bit1_0:
    sbi PORTB, 0    ; 2
    nop             ; 1
    cbi PORTB, 0    ; 2
    rjmp bit0       ; 2

bit1_1:
    sbi PORTB, 0    ; 2
    nop             ; 1
    nop             ; 1
    nop             ; 1
    nop             ; 1
    cbi PORTB, 0    ; 2

bit0:
    lsl r16         ; 1
    brcs bit0_1     ; 1-2
    nop

bit0_0:
    sbi PORTB, 0    ; 2
    nop             ; 1
    cbi PORTB, 0    ; 2
    sei
    ret

bit0_1:
    sbi PORTB, 0    ; 2
    nop             ; 1
    nop             ; 1
    nop             ; 1
    nop             ; 1
    cbi PORTB, 0    ; 2
    sei
    ret



;; While the below is nice,
;; I was getting HIGH times of 458 and 833 (send0 and send1, respectively)
;; and LOW times of 1666 and 1160
;; this is due to the overhead with bit shifting and testing.
;; The one to use is the unrolled version above

; sendbyte:
;     mov r16, r24        ; store passed byte in r16
;     ldi r17, 8          ; r17 will hold the bit counter

; sendbyte_loop:       ; 3 LOW
;     lsl r16
;     brcs sendbit1
;     nop

; sendbit0:
;     sbi PORTB, 0     ; 5 LOW
;     nop
;     cbi PORTB, 0     ; 3 HIGH!
;     rjmp continue    ; 2 LOW

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

; continue:         ; 3
;     dec r17
;     brne sendbyte_loop
;     ret


