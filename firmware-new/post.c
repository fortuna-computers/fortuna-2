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

const uint8_t z80_post_code[] PROGMEM = {
    0xdb, 0x00,         // IN A, TLCR   -  load last pressed key into A
    0x21, 0x1f, 0x00,   // LD HL, 0x1E0 -  store last pressed key in memory position 0x1F
    0x77,               // LD (HL), A
    0x76,               // HALT         -  halt the CPU
};

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

static void post_z80()
{
    uart_putstr(PSTR("CPU "));
    
    // load code into Z80
    memcpy_P((void*) buffer, z80_post_code, sizeof z80_post_code);
    ram_write_buffer(sizeof z80_post_code);
    
    // put a random character into `TLCR` (terminal last keypress)
    uint8_t expected_byte = rnd_next();
    io_set_last_char_received(expected_byte);
    
    // run Z80 code for a few milliseconds
    z80_powerup();
    _delay_ms(20);
    
    // check if the given code was put into the memory position
    ram_read_buffer(0x20);
    if (buffer[0x1f] != expected_byte) {
        fail();
    }
    
    z80_powerdown();
    
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
    post_z80();
    post_sdcard();
}
