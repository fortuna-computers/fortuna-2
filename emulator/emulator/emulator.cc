#include "emulator.hh"

#include "Z80.h"

#include <cstddef>

enum IORQ {
    I_TERMINAL      = 0x1,
    I_SD_B0         = 0x2,
    I_SD_B1         = 0x3,
    I_SD_B2         = 0x4,
    I_SD_B3         = 0x5,
    I_SD_ACTION     = 0x6,  // 0 = read, 1 = write
};

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
    switch (Port & 0xff) {
        case I_TERMINAL:
            Emulator::get().terminal().add_char(Value);
            break;
    }
}

byte InZ80(word Port)
{
    (void) Port;   // TODO
    return 0;
}

word LoopZ80(Z80 *R)
{
    static Emulator& emulator = Emulator::get();
    
    if (emulator.keyboard_interrupt()) {
        emulator.reset_keyboard_interrupt();
        return 0xcf;  // RST 08h
    }
    
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

void Emulator::keypress(uint8_t key)
{
    last_keypress_ = key;
    keyboard_interrupt_ = true;
}
