#include "post.h"

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "random.h"
#include "ram.h"
#include "uart.h"

extern volatile uint8_t seed;
extern volatile uint8_t buffer[512];

#define RAM_COUNT 8

static void post_ram()
{
    uint8_t original_seed = seed;
    
    ram_dump(RAM_COUNT);  // TODO
    
    // TODO: uart_putstr(PSTR("RAM "));
    
    // write data
    uint8_t my_seed = original_seed;
    for (uint16_t i = 0; i < RAM_COUNT; ++i) {
        buffer[i] = i;  // TODO - my_seed;
        my_seed = rnd_next(my_seed);
    }
    ram_write_buffer(RAM_COUNT);
    
    // clear buffer
    _delay_ms(50);
    for (uint16_t i = 0; i < RAM_COUNT; ++i)
        buffer[i] = 0;
    _delay_ms(50);
    
    ram_dump(RAM_COUNT);  // TODO
    
    // read data
    ram_read_buffer(RAM_COUNT);
    my_seed = original_seed;
    for (uint16_t i = 0; i < RAM_COUNT; ++i) {
        if (buffer[i] != i /* TODO - my_seed */) {
            uart_putstr(PSTR("failed"));
            for(;;);
        }
        my_seed = rnd_next(my_seed);
    }
    uart_putstr(PSTR("OK\n"));
}

void post_run()
{
    post_ram();
}
