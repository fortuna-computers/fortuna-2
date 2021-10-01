#include "post.h"

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "io.h"
#include "random.h"
#include "ram.h"
#include "sdcard.h"
#include "uart.h"
#include "z80.h"

extern volatile uint8_t buffer[512];

#define RAM_COUNT 512
#define Z80_EXPECTED_BYTE 0xaf

static void ok()
{
    uart_putstr(PSTR("OK\r\n"));
}

_Noreturn static void fail()
{
    uart_putstr(PSTR("FAIL"));
    for(;;);
}

static void post_ram()
{
    uint8_t original_seed = rnd_next();
    
    uart_putstr(PSTR("RAM "));
    
    // write data
    uint8_t my_seed = original_seed;
    for (uint16_t i = 0; i < RAM_COUNT; ++i) {
        buffer[i] = my_seed;
        my_seed = rnd_next_from(my_seed);
    }
    ram_write_buffer(RAM_COUNT);
    
    // clear buffer
    for (uint16_t i = 0; i < RAM_COUNT; ++i)
        buffer[i] = 0;
    
    // read data
    ram_read_buffer(RAM_COUNT);
    my_seed = original_seed;
    for (uint16_t i = 0; i < RAM_COUNT; ++i) {
        if (buffer[i] != my_seed) {
            fail();
        }
        my_seed = rnd_next_from(my_seed);
    }
    
    ok();
}

const uint8_t z80_post_code[] PROGMEM = {
        0x3e, Z80_EXPECTED_BYTE, // LD A, 0xAF    (8 cycles)
        /* 0xdb, 0x00,              // IN A, TLCR     (14 cycles) */
        0x21, 0x1f, 0x00,        // LD HL, 0x1F   (11 cycles)
        0x77,                    // LD (HL), A    (8 cycles)
        0x18, 0xfe,              // JR -2         (13 cycles)
};

static void post_z80_cycle()
{
    z80_single_step();
    PORTC = 0;
    uart_puthex(DDRC);
    PORTA = 0;
    uart_puthex(DDRA);
    if ((PIND & (1 << PIND6)))
        uart_putchar('W');
    if ((PIND & (1 << PIND7)))
        uart_putchar('R');
    uart_putenter();
}

static void post_z80()
{
    uart_putstr(PSTR("CPU "));
    
    // load code into Z80
    for (uint8_t i = 0; i < 0x20; ++i)
        buffer[i] = 0;
    memcpy_P((void*) buffer, z80_post_code, sizeof z80_post_code);
    ram_write_buffer(0x20);
    
    // put a random character into `TLCR` (terminal last keypress)
    uint8_t expected_byte = Z80_EXPECTED_BYTE /* rnd_next() */;
    io_set_last_char_received(expected_byte);
    
    uart_putenter();
    uart_puthex(DDRA);
    uart_puthex(DDRB);
    uart_puthex(DDRC);
    uart_puthex(DDRD);
    uart_putenter();
    uart_puthex(PINA);
    uart_puthex(PINB);
    uart_puthex(PINC);
    uart_puthex(PIND);
    uart_putenter();
    
    // run Z80 code for a few milliseconds
    z80_powerup();
    for (uint8_t i = 0; i < 8; ++i)  // ld a, 0xaf
        post_z80_cycle();
    uart_putstr(PSTR("----------\r\n"));
    /*
    for (uint8_t i = 0; i < 11; ++i)  // ld hl, 0x1f
        post_z80_cycle();
    uart_putstr(PSTR("----------\r\n"));
     */
    /*
    z80_run();
    _delay_ms(20);
    */
    z80_powerdown();

    ram_dump(0x24);
    
    // check if the given code was put into the memory position
    ram_read_buffer(0x20);
    if (buffer[0x1f] != expected_byte) {
        fail();
    }
    
    ok();
}

static void sdcard_fail()
{
    uart_puthex(sdcard_last_stage());
    uart_puthex(sdcard_last_response());
    fail();
}

static void post_sdcard()
{
    uart_putstr(PSTR("SDCARD "));
    
    if (!sdcard_initialize())
        sdcard_fail();
    
    if (!sdcard_read_page(0))
        sdcard_fail();
    
    if (buffer[510] != 0x55 || buffer[511] != 0xaa) {
        uart_putstr(PSTR("NOT BOOTABLE"));
        for(;;);
    }
    
    ok();
}

void post_run()
{
    post_ram();
    post_sdcard();
    post_z80();
}
