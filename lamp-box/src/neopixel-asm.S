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
; ---------------------------------------------------------------------------
send_byte:
    cli
    mov r16, r24

; ------- Bit 7 -------
.bit7:
    lsl r16
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
    lsl r16
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
    lsl r16
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
    lsl r16
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
    lsl r16
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
    lsl r16
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
    lsl r16
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
    lsl r16
    NEO_HIGH
    brcs .bit0_1
    NEO_LOW
    sei
    ret

.bit0_1:
    nop
    nop
    NEO_LOW
    sei
    ret


; ---------------------------------------------------------------------------
; void send_pixel(uint8_t red,
;                 uint8_t green,
;                 uint8_t blue,
;                 uint8_t white);
; ---------------------------------------------------------------------------
send_pixel:
    ; save caller's r16-17
    push r16
    push r17

    ; r24 red
    ; r22 green
    ; r20 blue
    ; r18 white

    mov r17, r24 ; r17 is red

#ifdef PIXEL_ORDER_GRBW
    SEND r22
    SEND r17
    SEND r20
    SEND r18
#elif defined(PIXEL_ORDER_RGBW)
    SEND r24
    SEND r22
    SEND r20
    SEND r18
#else
    .error "Define PIXEL_ORDER_GRBW or PIXEL_ORDER_RGBW"
#endif

    ; restore caller's r16-17 (reverse order)
    pop r17
    pop r16
    ret


; ---------------------------------------------------------------------------
; void flood_pixels(uint8_t red,
;                   uint8_t green,
;                   uint8_t blue,
;                   uint8_t white,
;                   uint8_t num_pixels);
; ---------------------------------------------------------------------------
flood_pixels:
    push  r16       ; save num_pixels (it's still available in r16, though)
    mov r2,  r24    ; = red
    mov r3,  r22    ; = green
    mov r4,  r20    ; = blue
    mov r5,  r18    ; = white

    tst   r16       ; what if it's already 0?
    breq  .done

.loop:
                    ; reload args
    mov r24, r2     ; reload red
    mov r22, r3     ; reload green
    mov r20, r4     ; reload blue
    mov r18, r5     ; reload white

    rcall send_pixel

    dec r16
    brne .loop

.done:
    ; restore num_pixels
    pop   r16
    ret
