I_TERMINAL  = 0x1
I_SD_B0     = 0x2
I_SD_B1     = 0x3
I_SD_B2     = 0x4
I_SD_B3     = 0x5
I_SD_ACTION = 0x6
I_SD_STATUS = 0x7

SD_WRITE = 1
SD_READ  = 0

; system calls
SYSCALL_VECTOR = 0x200
_PRINT         = 0
_EXIT          = 1

        macro SYS
        call    SYSCALL_VECTOR + (\0 * 3)
        endm

; vim: ts=8:sts=8:sw=8:expandtab:syntax=z80
