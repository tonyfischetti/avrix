
.section .data

.section .text
.global reverse_byte

reverse_byte:
  ldi r16, 0x0c      ; input byte
  clr r17          ; r17 = count
  ldi r18, 8       ; index

do_shift:
    lsr r16          ; shift bit into carry
    rol r17          ; shift carry left into r17
    dec r18
    brne do_shift


ret


.end

