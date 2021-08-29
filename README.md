# Mini-Z80

**Mini-Z80** is a computer in a level similar to Commodore 64 or MSX. The specifications are:

* CPU: Z80 running at 6 Mhz
* Memory: 32 KBytes
* Storage: SD card
* Serial input/output

## Startup

1. Mini-Z80 will load the ROM from the microcontroller (up to 512 bytes), will put it in the beginning of the RAM
and then start executing.
2. ROM code will then copy itself to position `0x200` and jump there, since the first 512 bytes are reserved for exchanging
data with the SDCard.
3. The ROM will then look for the file KERNEL.BIN from the root directory in the SDCard, load it to position `0x400` and
jump there to start executing the operating system.

## Interrupts

Keyboard interrupts will put the value `0x4` on the data bus. This can be handled with 
[Z80 interrupt mode 2](https://z80journal.wordpress.com/2015/04/15/z80-interrupts/) and a interrupt vector.

Interrupts can be activated with the following code:

```Assembly
ld a, MSB_OF_INTERRUPT_VECTOR
ld i, a
im 2
ei
```

## Terminal output (video text out)

Printing text to screen is accomplished by writing the byte to be written to I/O port `0x1`. For example, the following code prints the character 'H' in the screen.

```Assembly
include mini.z80           ; contains hardware definitions

ld  a, 'H'
out (I_TERMINAL), a        ; I_TERMINAL = 0x1
```

The screen is treated as a terminal - the character is printed in the current cursor location, and the cursor is then moved forward.

## Terminal input (keyboard)

If the interrupts are activated, any keypresses will fire an interrupt and place the Z80 on the address `0x8`
 - this is the position where the keyboard handling routine should be written.

The last key pressed can be read from the terminal management unit by reading from the I/O port `0x1`.

This is an example code that intercepts a keyboard interrupt and places the last pressed key in the register `A`.

```Assembly
    include mini.z80         ; contains hardware definitions

    ;
    ; INITIALIZATION
    ;
    im  0                    ; activate interrupts
    ei
    jp  skip_interrupt_routine

    ;
    ; KEYBOARD HANDLING ROUTINE
    ;
    org INTERRUPT_ROUTINE    ; interrupt routine in 0x8
    di                       ; disable interrupts
    in  a, (I_TERMINAL)      ; place last pressed key in `A`
    ; do something with the key that was loaded...
    ei                       ; re-enable interrupts
    reti

    ; 
    ; REGULAR CODE
    ; 
skip_interrupt_routine:
    ; rest of the code...
```

## SD card I/O

The SD card is accessed always one 512-byte block each time. The code block is copied from/to the first 512 bytes of RAM.

To read or write a block on the SD card, the following procedure must be followed:

1. Execute an I/O output on ports `I_SD_B0`, `I_SD_B1`, `I_SD_B2` and `I_SD_B3` with the SDCard (4 bytes) block number.
2. Execute an I/O output on port `I_SD_ACTION` (`0x6`), passing `SD_READ` (`0`) to read data and `SD_WRITE` (`1`) to write.

Example reading data from SD block `0xAE0012`. We'll use address location `0xF000` to store the parameters.
 
```Assembly
    include mini.z80          ; contains hardware definitions

    ld  a, 0x12               ; set SD origin block
    ld  (I_SD_B0)
    ld  a, 0x0
    ld  (I_SD_B1)
    ld  a, 0xae
    ld  (I_SD_B2)
    ld  a, 0x0
    ld  (I_SD_B3)

    ld  a, 0
    ld  (I_SD_ACTION)         ; execute read
```

Writing to the SD card is similar, just use `SD_WRITE` instead of `SD_READ`.
