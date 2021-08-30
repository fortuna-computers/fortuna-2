#ifndef RAM_H_
#define RAM_H_

#include <stdbool.h>
#include <stdint.h>

extern uint8_t buffer[512];

void ram_init();

uint8_t ram_read_byte(uint16_t addr);
void    ram_write_byte(uint16_t addr, uint8_t byte);

bool    ram_write_buffer();
void    ram_read_buffer();

uint16_t ram_buffer_checksum();

void    ram_initialize_boot_sector(uint8_t* data, uint16_t size);

#endif
