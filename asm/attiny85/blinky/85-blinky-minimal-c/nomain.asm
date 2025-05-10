

; !


; ldi - load immediate
; loads value in registry
; ldi r16, 0x2f
; ldi r17, 0x01
;
; add - add two register and put sum in first register
; add r16, r17  ; now r16 is 0x30
; sub - subtraction
; mul
; and
; xor
; 
; mov - copy data to, from registers
; mov r17, r18
; 
; inc - increment
; inc r19
; 
; com - one's complement
; com r20 ; 0xFF
; 
; 
; lds - load direct from data space
; lds r16, 0x00 ; load into r16 what's stored at 0x00 from SRAM
; 
; in - similar to lds
; in is faster
; two byte instruction (lds is 4)
; in r21, 0x01
; 
; 
; sts - stored direct from dataspace
; sts 0x00, r17 ; puts the value of r17 into SRAM location 0x00
; 
; out 0x3e, r21
; 
; 
; rcall, cpi, ror, nop, ret, breq, break, sleep, wdr, clc, or
; 
; 
; 
; 


;; memory map
; 0x00 - r0
; 0x0F - r15
; 0x10 - r16
; 0x1F - r31

start:
    inc r16
    rjmp start




