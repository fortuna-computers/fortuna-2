#include "emulator.hh"

#include "Z80.h"

#include <cstddef>

void WrZ80(word Addr,byte Value)
{
    Emulator::get().ram_set(Addr, Value);
}

byte RdZ80(word Addr)
{
    return Emulator::get().ram_get(Addr);
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
    ram_[addr & (MEMORY_SIZE - 1)] = data;
}

uint8_t Emulator::ram_get(uint16_t addr) const
{
    return ram_[addr & (MEMORY_SIZE - 1)];
}

size_t Emulator::ram_size()
{
    return MEMORY_SIZE;
}