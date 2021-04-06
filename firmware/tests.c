#include "tests.h"

#ifdef RUN_TESTS

#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "io.h"
#include "ram.h"
#include "sdcard.h"
#include "serial.h"
#include "z80.h"

static void create_seed()
{
    // create a seed
    eeprom_busy_wait();
    uint16_t seed = eeprom_read_word((uint16_t*) 0);
    srandom(seed);
    uint16_t next = random();
    printf_P(PSTR("Seed is %d. Next is %d.\n"), seed, next);

    eeprom_write_word((uint16_t*) 0, next);
}

static int free_ram()
{
    extern int __heap_start, *__brkval;
    volatile int v;
    int free_ = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    return free_;
}

static void test_byte(uint16_t addr)
{
    uint8_t data = random();
    ram_write_byte(addr, data);
    printf_P(PSTR("[%03X] = written %02X, read "), addr, data);
    uint8_t data2 = ram_read_byte(addr);
    printf_P(PSTR("%s%02X\n" RESET), (data == data2) ? GRN : RED, data2);
}

void run_tests()
{
    z80_powerup();
    z80_release_bus();

    printf_P(PSTR("Free RAM in microcontroller: %d bytes.\n"), free_ram());
    create_seed();
    printf_P(PSTR("Writing bytes...\n"));

    for (size_t i = 0; i < 5; ++i) {
        uint16_t addr = 0x107; // random() & 0x1ff;
        test_byte(addr);
    }

    // write buffer
    int next_seed = random();
    srandom(next_seed);
    for (size_t i = 0; i < 512; ++i) {
        uint8_t data = random();
        buffer[i] = data;
    }
    printf_P(PSTR("Writing buffer... "));
    for (size_t i = 0; i < 512; ++i)
        printf_P(PSTR("%02X "), buffer[i]);
    bool success = ram_write_buffer();
    putchar('\n');
    printf_P(PSTR("Buffer checksum: %04X\n"), ram_buffer_checksum());

    // read buffer
    printf_P(PSTR("Reading buffer... "));
    for (size_t i = 0; i < 512; ++i)
        buffer[i] = 0;
    ram_read_buffer();
    srandom(next_seed);
    for (size_t i = 0; i < 512; ++i) {
        uint8_t expected = random();
        printf_P(PSTR("%s%02X " RESET), (buffer[i] == expected) ? GRN : RED, buffer[i]);
    }
    putchar('\n');
    if (!success)
        printf_P(PSTR(RED "Writing failed!\n" RESET));
    printf_P(PSTR("Buffer checksum: %04X\n"), ram_buffer_checksum());

    // sdcard read
    sdcard_initialize();
    printf_P(PSTR("SDCard initialization: %02X %02X\n"), sdcard_last_stage(), sdcard_last_response());

    printf_P(PSTR("SD read page 0: "));
    if (!sdcard_read_page(0))
        printf_P(PSTR("failed: %02X %02X.\n"), sdcard_last_stage(), sdcard_last_response());
    for (size_t i = 0; i < 512; ++i)
        printf_P(PSTR("%02X"), buffer[i]);
    putchar('\n');

    // sdcard write
    uint8_t sdi = random();
    for (size_t i = 0; i < 512; ++i) {
        buffer[i] = sdi + i;
    }
    sdcard_write_page(1);
    printf_P(PSTR("SD write page 1 starting at %02X: %02X %02X\n"), sdi, sdcard_last_stage(), sdcard_last_response());
    
    printf_P(PSTR("SD read page 1: "));
    if (!sdcard_read_page(1))
        printf_P(PSTR("failed: %02X %02X.\n"), sdcard_last_stage(), sdcard_last_response());
    for (size_t i = 0; i < 512; ++i)
        printf_P(PSTR("%02X"), buffer[i]);
    putchar('\n');

    z80_request_bus();

    for(;;);
}

#endif
