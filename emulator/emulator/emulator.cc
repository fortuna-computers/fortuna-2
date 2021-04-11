#include "emulator.hh"

#include "Z80.h"

#include <cstddef>

static constexpr size_t MEMORY_SIZE = 32 * 1024;
static uint8_t ram_[MEMORY_SIZE] = { 0 };

void WrZ80(word Addr,byte Value)
{
    ram_[Addr & (MEMORY_SIZE - 1)] = Value;
}

byte RdZ80(word Addr)
{
    return ram_[Addr & (MEMORY_SIZE - 1)];
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

Emulator& Emulator::get()
{
    static Emulator emulator;
    return emulator;
}

void Emulator::ram_set(uint16_t addr, uint8_t data)
{
    WrZ80(addr, data);
}

uint8_t Emulator::ram_get(uint16_t addr) const
{
    return RdZ80(addr);
}

size_t Emulator::ram_size()
{
    return MEMORY_SIZE;
}

Z80* Emulator::z80() const
{
    return nullptr;
}
