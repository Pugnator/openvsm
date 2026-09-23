; Real arithmetic, with the working values living in RAM rather than in
; registers, checked against answers that are known independently.
;
; A pattern test proves the memory array. This proves the machine: every
; result below depends on the ALU, the carry flag, 16-bit adds, the stack, and
; a read-modify-write through RAM on every single iteration. Get any one of
; them wrong and the totals do not come out - there is no way to be accidentally
; right about 34600.
;
;     zasm -s mathtest.asm -o mathtest.bin
;
; Ports (A7 low: the ACIA on this board answers any port from 80 to FF):
;
;   70 <- AA   starting          70 <- 55   every test passed
;   76 <- n    test n started    77 <- n    test n passed
;
;   On the first wrong answer, five values and a halt:
;     71 <- n    which test      72 <- xx   expected, low byte
;     73 <- xx   got, low        74 <- xx   expected, high
;     75 <- xx   got, high
;
; The three sums:
;
;   1  1+2+...+255           = 32640  = 7F80   accumulated 16-bit in RAM,
;                                              255 read-modify-writes, ADC
;   2  Fibonacci F(23)       = 28657  = 6FF1   two 16-bit values in RAM,
;                                              ADD HL,DE, 23 iterations
;   3  200 x 173             = 34600  = 8728   shift-and-add, product in RAM,
;                                              PUSH/POP round the RAM access

PORT_STATE      equ     0x70
PORT_FAILNO     equ     0x71
PORT_EXP_LO     equ     0x72
PORT_GOT_LO     equ     0x73
PORT_EXP_HI     equ     0x74
PORT_GOT_HI     equ     0x75
PORT_RUNNING    equ     0x76
PORT_PASSED     equ     0x77

ACC             equ     0x8100          ; test 1 accumulator
FIB_A           equ     0x8110          ; test 2, F(n)
FIB_B           equ     0x8112          ; test 2, F(n+1)
PROD            equ     0x8120          ; test 3 product
TESTNO          equ     0x81FF          ; which test is running
STACK_TOP       equ     0xA000

                ORG     0x0000

start:          di
                ld      sp, STACK_TOP
                ld      a, 0xAA
                out     [PORT_STATE], a

; --------------------------------------------------------------------------
; 1. Sum 1..255 into a 16-bit accumulator that lives in RAM.
;    Every iteration loads it, adds with carry, and stores it back, so the
;    total is only right if 255 read-modify-write pairs all worked.
; --------------------------------------------------------------------------
                ld      a, 1
                ld      [TESTNO], a
                out     [PORT_RUNNING], a

                ld      hl, 0
                ld      [ACC], hl
                ld      b, 255
t1_loop:        ld      hl, [ACC]
                ld      a, l
                add     a, b
                ld      l, a
                ld      a, h
                adc     a, 0            ; the carry must survive the LD
                ld      h, a
                ld      [ACC], hl
                djnz    t1_loop

                ld      de, ACC
                ld      bc, 32640       ; 7F80
                call    check16

                ld      a, 1
                out     [PORT_PASSED], a

; --------------------------------------------------------------------------
; 2. Fibonacci, both running values held in RAM.
;    a=0, b=1; 23 times: t=a+b, a=b, b=t. Ends with a=F(23), b=F(24).
; --------------------------------------------------------------------------
                ld      a, 2
                ld      [TESTNO], a
                out     [PORT_RUNNING], a

                ld      hl, 0
                ld      [FIB_A], hl
                ld      hl, 1
                ld      [FIB_B], hl
                ld      b, 23
t2_loop:        ld      hl, [FIB_A]
                ld      de, [FIB_B]
                add     hl, de          ; hl = a + b
                ld      [FIB_A], de     ; a = b
                ld      [FIB_B], hl     ; b = t
                djnz    t2_loop

                ld      de, FIB_A
                ld      bc, 28657       ; 6FF1
                call    check16

                ld      a, 2
                out     [PORT_PASSED], a

; --------------------------------------------------------------------------
; 3. 200 x 173 by shift-and-add, product in RAM.
;    The multiplier is carried in A across a PUSH/POP either side of the RAM
;    access, so the stack is in the loop as well as the arithmetic.
; --------------------------------------------------------------------------
                ld      a, 3
                ld      [TESTNO], a
                out     [PORT_RUNNING], a

                ld      hl, 0
                ld      [PROD], hl
                ld      de, 200         ; multiplicand, doubled each pass
                ld      a, 173          ; multiplier, shifted out bit by bit
                ld      b, 8
t3_loop:        srl     a               ; bit 0 into carry, 0 into bit 7
                push    af
                jr      nc, t3_skip
                ld      hl, [PROD]
                add     hl, de
                ld      [PROD], hl
t3_skip:        sla     e               ; de = de * 2
                rl      d
                pop     af              ; flags and multiplier back
                djnz    t3_loop

                ld      de, PROD
                ld      bc, 34600       ; 8728
                call    check16

                ld      a, 3
                out     [PORT_PASSED], a

; --------------------------------------------------------------------------
                ld      a, 0x55
                out     [PORT_STATE], a
                halt

; --------------------------------------------------------------------------
; check16 - the 16-bit value at [DE] must equal BC, or report and stop.
; Reached by CALL, so a working stack is a precondition of every test passing.
; --------------------------------------------------------------------------
check16:        ld      a, [de]
                ld      l, a            ; got, low
                inc     de
                ld      a, [de]
                ld      h, a            ; got, high

                ld      a, l
                cp      c
                jr      nz, check_bad
                ld      a, h
                cp      b
                jr      nz, check_bad
                ret

check_bad:      ld      a, [TESTNO]
                out     [PORT_FAILNO], a
                ld      a, c
                out     [PORT_EXP_LO], a
                ld      a, l
                out     [PORT_GOT_LO], a
                ld      a, b
                out     [PORT_EXP_HI], a
                ld      a, h
                out     [PORT_GOT_HI], a
                halt
