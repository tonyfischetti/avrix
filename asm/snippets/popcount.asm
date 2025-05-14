.section .data

.section .text
.global popcount

popcount:
  ldi r16, 93      ; input byte
  clr r17          ; r17 = count

shift_left_and_increment_r17_if_1:
    lsr r16          ; shift bit into carry
    brcc skip_inc    ; if carry is 0, skip increment
    inc r17          ; otherwise, add 1

skip_inc:
    tst r16          ; sets Z if r16 == 0
    brne shift_left_and_increment_r17_if_1 ; if Z == 0


ret


.end
