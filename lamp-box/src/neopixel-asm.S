#define __SFR_OFFSET 0x00
#include "avr/io.h"


; #ifndef NEO_PORT
; #define NEO_PORT PORTB
; #endif

#ifndef NEO_PORT
.error "Must define NEO_PORT (the port that the neopixels are connected to"
#endif

#ifndef NEO_BIT
.error "Must define NEO_BIT (the but on NEO_PORT that corresponds to the pin that the neopixels are connected to"
#endif


; .section .text
.global send_byte
.global send_pixel
.global flood_pixels


; WS2812 sendbyte routine, 8 MHz ATtiny85
; Sends r24 (byte) MSB first on PORTB.0
; Bit timing: 10 cycles per bit
;   Bit 0: HIGH 3 cycles, LOW 7
;   Bit 1: HIGH 6 cycles, LOW 4



; --------------------------------------------------------------------------
; Macros
; --------------------------------------------------------------------------
.macro  SEND reg
    ; .ifc    \reg,"r24" ; don't bother moving r24 to r24
    ;     rcall send_byte
    ; .else
        mov   r24, \reg
        rcall send_byte
    ; .endif
.endm

.macro  NEO_HIGH
    sbi NEO_PORT, NEO_BIT
.endm

.macro  NEO_LOW
    cbi NEO_PORT, NEO_BIT
.endm


; ---------------------------------------------------------------------------
; void send_byte(uint8_t b);
;
; Clobbers r19 and r25 only (both call-clobbered per the avr-gcc ABI).
; Restores the caller's interrupt flag on exit (via SREG) instead of
; unconditionally enabling interrupts.
; ---------------------------------------------------------------------------
send_byte:
    in  r25, SREG
    cli
    mov r19, r24

; ------- Bit 7 -------
.bit7:
    lsl r19
    NEO_HIGH
    brcs .bit7_1
    NEO_LOW
    nop
    nop
    rjmp .bit6

.bit7_1:
    nop
    nop
    NEO_LOW
    nop

; ------- Bit 6 -------
.bit6:
    lsl r19
    NEO_HIGH
    brcs .bit6_1
    NEO_LOW
    nop
    nop
    rjmp .bit5

.bit6_1:
    nop
    nop
    NEO_LOW
    nop

; ------- Bit 5 -------
.bit5:
    lsl r19
    NEO_HIGH
    brcs .bit5_1
    NEO_LOW
    nop
    nop
    rjmp .bit4

.bit5_1:
    nop
    nop
    NEO_LOW
    nop

; ------- Bit 4 -------
.bit4:
    lsl r19
    NEO_HIGH
    brcs .bit4_1
    NEO_LOW
    nop
    nop
    rjmp .bit3

.bit4_1:
    nop
    nop
    NEO_LOW
    nop

; ------- Bit 3 -------
.bit3:
    lsl r19
    NEO_HIGH
    brcs .bit3_1
    NEO_LOW
    nop
    nop
    rjmp .bit2

.bit3_1:
    nop
    nop
    NEO_LOW
    nop

; ------- Bit 2 -------
.bit2:
    lsl r19
    NEO_HIGH
    brcs .bit2_1
    NEO_LOW
    nop
    nop
    rjmp .bit1

.bit2_1:
    nop
    nop
    NEO_LOW
    nop

; ------- Bit 1 -------
.bit1:
    lsl r19
    NEO_HIGH
    brcs .bit1_1
    NEO_LOW
    nop
    nop
    rjmp .bit0

.bit1_1:
    nop
    nop
    NEO_LOW
    nop

; ------- Bit 0 -------
.bit0:
    lsl r19
    NEO_HIGH
    brcs .bit0_1
    NEO_LOW
    out SREG, r25
    ret

.bit0_1:
    nop
    nop
    NEO_LOW
    out SREG, r25
    ret


; ---------------------------------------------------------------------------
; void send_pixel(uint8_t red,
;                 uint8_t green,
;                 uint8_t blue,
;                 uint8_t white);
; ---------------------------------------------------------------------------
send_pixel:
    ; r24 red
    ; r22 green
    ; r20 blue
    ; r18 white

    ; send_byte only clobbers r19/r25, so r22/r20/r18 survive as-is;
    ; red just needs to move out of r24 (send_byte's argument register)
    mov r21, r24 ; r21 is red

#ifdef PIXEL_ORDER_GRBW
    SEND r22
    SEND r21
    SEND r20
    SEND r18
#elif defined(PIXEL_ORDER_RGBW)
    SEND r21
    SEND r22
    SEND r20
    SEND r18
#else
    .error "Define PIXEL_ORDER_GRBW or PIXEL_ORDER_RGBW"
#endif

    ret


; ---------------------------------------------------------------------------
; void flood_pixels(uint8_t red,
;                   uint8_t green,
;                   uint8_t blue,
;                   uint8_t white,
;                   uint8_t num_pixels);
; ---------------------------------------------------------------------------
flood_pixels:
    ; num_pixels arrives in r16, which is call-saved: copy it (and the
    ; colors) into call-clobbered registers so nothing needs push/pop
    mov r23, r24    ; = red
    mov r26, r22    ; = green
    mov r27, r20    ; = blue
    mov r30, r18    ; = white
    mov r31, r16    ; = num_pixels (loop counter)

    tst   r31       ; what if it's already 0?
    breq  .done

.loop:
                    ; reload args
    mov r24, r23    ; reload red
    mov r22, r26    ; reload green
    mov r20, r27    ; reload blue
    mov r18, r30    ; reload white

    rcall send_pixel

    dec r31
    brne .loop

.done:
    ret
