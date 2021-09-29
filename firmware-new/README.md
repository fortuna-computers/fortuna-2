# Memory

Most operations happen on RAM zeropage, that goes from `0x0` to `0x1FF`. The rest of the memory
is free for applications to use.

# Initialization

On initialization, Fortuna-2 will look in the SDCard for a file called BOOT.BIN and load it
in RAM zeropage. Ideally, this boot file should copy itself into another memory location and
run from there, in order to keep the zeropage free for SD Card operations.

# I/O registers

| Register | I/O port | Description |
|----------|----------|-------------|
| `TLCR`   | `0x0`    | Terminal - last char received from serial. Reading it resets the char to `0x0`. |
| `TSO`    | `0x1`    | Terminal - serial output. Placing a char on it sends it via serial. |

# I/O calls

Output:

  - Terminal:
    - **0x1**: send char on serial

  - SD card:
    - **0x20**: when set to `0x0`, will read from SDCard, from the block set in the `SD[0:3]` registers into RAM zero page.
               When set to `0x1`, will write from RAM zeropage to that SDCard block.
    - **0x21**: Gets directory name from zeropage, and write the directory contents to zeropage. (TODO: define format)
    - **0x22**: Delete file from filename in zeropage
    - **0x23**: Open/create file from filename in zeropage using the provided index. (TODO: define format, include file position)
    - **0x24**: Move given file index to location (\* 0x200)
    - **0x25**: Read current index block
    - **0x26**: Write current index block
    - **0x27**: Release index
    - **0x28**: Rename/move file (TODO: how?)
    - **0x29**: File stat (TODO: define format)
    - **0x2a**: Get disk info (TODO: define format)
    - Set registers:
      - **0x30** to **0x33**: set registers `SD0` to `SD3` for low-level SD Card access
      - **0x34**: result of the last card operation (TODO: define codes)
  
  - Timer:
    - TBD

# Interrupts

Fortuna-2 uses Z80 [Z80 interrupt mode 2](https://z80journal.wordpress.com/2015/04/15/z80-interrupts/).
The following interrupts can happen:

  - **0x4**: data received in serial
  - **0x6**: timer event (TODO: define)

Example code to capture a keypress:

```Assembly
    include mini.z80         ; contains hardware definitions

    INTERRUPT_VECTOR = 0x100

    ;
    ; INITIALIZATION
    ;
    ld a, INTERRUPT_VECTOR / 0x100  ; set MSB of interrupt vector (register I)
    ld i, a
    im 2                     ; interrupt mode 2
    ei                       ; initialize interrupts
    jp skip_interrupt_routine

    ;
    ; KEYBOARD HANDLING ROUTINE
    ;
    org INTERRUPT_VECTOR + 4 ; interrupt routine in 0x104
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
