.section .bss
.global lfsr_state
lfsr_state:
    .skip 1             ; Reserve 1 byte for LFSR state (uninitialized data)

.section .text
.global start
.global init_lfsr
.global get_next_byte
.global get_next_bit

.type start, @function
start:
    ; sbi 0x17, 0        ; Set PB0 as output (DDRB)
    ; sbi 0x17, 1        ; Set PB1 as output
    ; sbi 0x17, 2        ; Set PB2 as output
    sbi 0x17, 3        ; Set PB3 as output (DDRB)
    sbi 0x17, 1        ; Set PB1 as output
    sbi 0x17, 2        ; Set PB2 as output
    ret

.type init_lfsr, @function
init_lfsr:
    sts lfsr_state, r24 ; Save seed value from r24 into lfsr_state
    ret

.type get_next_byte, @function
get_next_byte:
    clr   r17           ; r17 = result byte (cleared)
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    rcall get_next_bit
    mov r24, r17        ; Move result into return register
    ret

.type get_next_bit, @function
get_next_bit:
    clr r19             ; r19 used for feedback bit calculation
    lds r16, lfsr_state ; Load current LFSR state
    mov r18, r16        ; Copy to r18 for updating

    ; Tap positions: 0, 2, 3, 4 (X^8 + X^6 + X^5 + X^4 + 1)
    eor r19, r16        ; bit 0
    lsr r16
    lsr r16
    eor r19, r16        ; bit 2
    lsr r16
    eor r19, r16        ; bit 3
    lsr r16
    eor r19, r16        ; bit 4
    lsr r16
    lsr r16
    lsr r16             ; shift to end

    lsr r19             ; Shift feedback bit into carry
    ror r18             ; Rotate carry into MSB of new state
    rol r17             ; Rotate carry into LSB of result byte

    sts lfsr_state, r18 ; Store updated state
    ret
