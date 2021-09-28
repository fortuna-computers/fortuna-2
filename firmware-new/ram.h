#ifndef RAM_H_
#define RAM_H_

#include <stdint.h>

void ram_init();

void ram_write_buffer(uint16_t until);
void ram_read_buffer(uint16_t until);

void    ram_write_byte(uint16_t addr, uint8_t data);
uint8_t ram_read_byte(uint16_t addr);

void ram_dump(uint16_t until);

#endif
