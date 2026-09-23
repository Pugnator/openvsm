; A minimal polled UART test for the 6850 on this board.
;
; The point is to take everything else out of the picture: no interrupts, no
; BASIC, no RAM beyond the stack. If the ACIA is wired and enabled, this puts
; characters on the wire. If it is not, the log says exactly how far it got.
;
; Ports
;   80  ACIA control on write, status on read
;   81  ACIA data
;   70  progress marker    AA = started, EE = gave up waiting for the transmitter
;   71  ACIA status as read straight after initialisation
;   72  every byte handed to the transmitter
;   73  ACIA status at the moment the transmitter timed out
;   74  every byte received
;
; The 7x ports go nowhere on the board. They exist so the model's log shows
; the sequence even when nothing reaches the terminal. They must keep A7 low:
; the ACIA is selected by A7 with IORQ, so any port from 80 to FF lands in
; it, with A0 choosing between the control and data registers. An earlier
; version of this file used ports F0 to F4, and every progress marker was
; quietly rewriting the ACIA's control register with whatever byte had just
; been sent, changing the word format and divider character by character.
;
; Settings:
;
;   CPU clock          100 kHz
;   ACIA clock         19200 Hz
;   Virtual terminal   1200 baud, 8 bits, no parity, 1 stop
;
; Control word 0x15 is divide by 16 per the datasheet, so 19200 / 16 = 1200.
; Measured with uartrate.bin: 8263 us per character, which is 1210 baud.

                ORG     0x0000

start:          di
                ld      sp, 0xA000
                ld      a, 0xAA
                out     [0x70], a               ; we are alive

                ld      a, 0x03                 ; master reset, required after
                out     [0x80], a               ; power-up before anything else

                ld      a, 0x15                 ; 8N1, divide by 16 -> 1200 baud from 19200
                out     [0x80], a

                in      a, [0x80]               ; what does it say for itself?
                out     [0x71], a

                ld      a, 0x55                 ; blind write, ignoring the status,
                out     [0x81], a               ; so the line moves even if TDRE sticks

msgloop:        ld      hl, msg
nextch:         ld      a, [hl]
                or      a
                jr      z, rxpoll
                call    putc
                inc     hl
                jr      nextch

rxpoll:         in      a, [0x80]
                bit     0, a                    ; receive register full?
                jr      z, msgloop
                in      a, [0x81]
                out     [0x74], a
                call    putc                    ; echo it back
                jr      rxpoll

; Send the byte in A, giving up rather than hanging if the transmitter never
; reports itself empty. A hang would look identical to a dead ACIA in the log.
putc:           push    af
                ld      bc, 0x0400              ; ~0.5 s of polling at 100 kHz, then quit
pwait:          in      a, [0x80]
                bit     1, a                    ; transmit register empty?
                jr      nz, pready
                dec     bc
                ld      a, b
                or      c
                jr      nz, pwait
                in      a, [0x80]               ; timed out - report and stop
                out     [0x73], a
                ld      a, 0xEE
                out     [0x70], a
                halt
pready:         pop     af
                out     [0x81], a
                out     [0x72], a
                ret

msg:            db      "UART TEST", 13, 10, 0
