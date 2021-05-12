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

void Emulator::initialize(std::vector<uint8_t> const& rom, std::optional<std::string> const& image_filename)
{
    uint16_t addr = 0;
    for (uint8_t byte: rom)
        WrZ80(addr++, byte);
    
    if (image_filename.has_value()) {
        if (image_.has_value())
            image_.value().close();
        image_ = std::ifstream { image_filename.value(), std::ios::binary };
        if (!(*image_) || image_->bad() || image_->fail())
            throw std::runtime_error("Error opening image file " + *image_filename + ".");
    }
}

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
    switch (Port & 0xff) {
        case I_TERMINAL:
            auto key = Emulator::get().last_keypress();
            Emulator::get().keypress(0);
            return key;
    }
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
