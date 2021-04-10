#include "Z80.h"

#include <cstddef>
#include <cstdint>

static constexpr size_t MEMORY_SIZE = 32 * 1024;
static uint8_t ram[MEMORY_SIZE] = { 0 };

void WrZ80(word Addr,byte Value)
{
    ram[Addr & (MEMORY_SIZE - 1)] = Value;
}

byte RdZ80(word Addr)
{
    return ram[Addr & (MEMORY_SIZE - 1)];
}

void OutZ80(word Port,byte Value)
{
    (void) Port; (void) Value;  // TODO
}

byte InZ80(word Port)
{
    (void) Port;   // TODO
    return 0;
}

word LoopZ80(Z80 *R)
{
    return INT_QUIT;
}

void PatchZ80(Z80 *R) { (void) R; }
