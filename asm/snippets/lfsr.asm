.section .data

.section .text
.global get_next_byte

; uses r16 hold state
; r17 holds the next random byte
; r18 is clobbered
; r19 is clobbered


init_lfsr:
    ldi r16, 93 ; starting seed / state of lfsr

get_next_byte:
    clr r17     ; final rand byte
    call get_next_bit
    call get_next_bit
    call get_next_bit
    call get_next_bit
    call get_next_bit
    call get_next_bit
    call get_next_bit
    call get_next_bit
    ret

get_next_bit:
    clr r19     ; for construction of next bit
    mov r18, r16
    eor r19, r16 ; r16 >> 0
    lsr r16      ; r16 >> 1   
    lsr r16      ; r16 >> 2
    eor r19, r16 ;
    lsr r16      ; r16 >> 3
    eor r19, r16 ;
    lsr r16      ; r16 >> 4
    eor r19, r16 ;
    lsr r16      ; r16 >> 5
    lsr r16      ; r16 >> 6
    lsr r16      ; r16 >> 7
    lsr r19      ; carry flag is the fresh bit!
    ror r18      ; carry flag is the random bit and r18 is new state
    rol r17    
    mov r16, r18 ; update state
    ret


ret

.end

