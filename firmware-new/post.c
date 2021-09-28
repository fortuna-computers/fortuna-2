#include "post.h"

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "random.h"
#include "ram.h"
#include "sdcard.h"
#include "uart.h"

extern volatile uint8_t seed;
extern volatile uint8_t buffer[512];

#define RAM_COUNT 512

static void post_ram()
{
    uint8_t original_seed = seed;
    
    uart_putstr(PSTR("RAM "));
    
    // write data
    uint8_t my_seed = original_seed;
    for (uint16_t i = 0; i < RAM_COUNT; ++i) {
        buffer[i] = my_seed;
        my_seed = rnd_next(my_seed);
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
            uart_putstr(PSTR("FAIL"));
            for(;;);
        }
        my_seed = rnd_next(my_seed);
    }
    uart_putstr(PSTR("OK\r\n"));
}

static void sdcard_fail()
{
    uart_putstr(PSTR("FAIL "));
    uart_puthex(sdcard_last_stage());
    uart_puthex(sdcard_last_response());
    for(;;);
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
    
    uart_putstr(PSTR("OK\r\n"));
}

void post_run()
{
    post_ram();
    post_sdcard();
}
