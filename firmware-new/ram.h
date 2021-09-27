#ifndef RAM_H_
#define RAM_H_

#include <stdint.h>

void ram_init();

void ram_write_buffer(uint16_t until);
void ram_read_buffer(uint16_t until);

#endif
