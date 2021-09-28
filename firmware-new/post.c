#include "post.h"

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "random.h"
#include "ram.h"
#include "uart.h"

extern volatile uint8_t seed;
extern volatile uint8_t buffer[512];

#define RAM_COUNT 32

static void post_ram()
{
    const uint16_t addr = 0;
    uint8_t data = seed;
    uart_puthex(data);
    ram_write_byte(addr, data);

    for (int i = 0; i < 8; ++i)
        uart_puthex(ram_read_byte(addr));
    uart_putenter();


    for (uint8_t i = 0; i < 32; ++i)
        buffer[i] = seed + i;
    
    for (uint8_t i = 0; i < 32; ++i)
        uart_puthex(buffer[i]);
    uart_putenter();
    uart_putstr(PSTR("* * *\r\n"));
    
    ram_write_buffer(32);
    
    for (uint8_t i = 0; i < 8; ++i)
        ram_dump(RAM_COUNT);
}

/*
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
    _delay_ms(50);
    for (uint16_t i = 0; i < RAM_COUNT; ++i)
        buffer[i] = 0;
    _delay_ms(50);
    
    // read data
    ram_read_buffer(RAM_COUNT);
    my_seed = original_seed;
    for (uint16_t i = 0; i < RAM_COUNT; ++i) {
        if (buffer[i] != my_seed) {
            uart_putstr(PSTR("failed"));
            for(;;);
        }
        my_seed = rnd_next(my_seed);
    }
    uart_putstr(PSTR("OK\n"));
}
*/

void post_run()
{
    post_ram();
}
