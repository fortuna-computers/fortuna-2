#include "emulator.hh"

#include "Z80.h"
#include "image.hh"

#include <cstddef>

enum IORQ {
    I_TERMINAL      = 0x1,
    I_SD_B0         = 0x2,
    I_SD_B1         = 0x3,
    I_SD_B2         = 0x4,
    I_SD_B3         = 0x5,
    I_SD_ACTION     = 0x6,  // 0 = read, 1 = write
};

static Emulator* emulator;

Emulator::Emulator()
{
    emulator = this;
}

void Emulator::initialize(std::vector<uint8_t> const& rom, ProjectFile const& project_file, std::string const& sources_path)
{
    // reset CPU
    soft_reset();
    
    // reset RAM
    for (uint8_t& addr : ram_)
        addr = 0;
    
    // load ROM into memory
    uint16_t addr = 0;
    for (uint8_t byte: rom)
        ram_[addr++] = byte;
    
    image_filename_ = image_file_create(project_file, sources_path, true).value_or(nullptr);
}


void WrZ80(word Addr,byte Value)
{
    emulator->ram_set(Addr, Value);
}

byte RdZ80(word Addr)
{
    return emulator->ram_get(Addr);
}

void OutZ80(word Port,byte Value)
{
    switch (Port & 0xff) {
        case I_TERMINAL:
            emulator->terminal().add_char(Value);
            break;
    }
}

byte InZ80(word Port)
{
    switch (Port & 0xff) {
        case I_TERMINAL:
            auto key = emulator->last_keypress();
            emulator->keypress(0);
            return key;
    }
    return 0;
}

word LoopZ80(Z80 *R)
{
    // check for interrupts
    if (emulator->keyboard_interrupt()) {
        emulator->reset_keyboard_interrupt();
        return 0xcf;  // RST 08h
    }
    
    if (emulator->continue_mode()) {
        // check for breakpoints
        if (emulator->is_breakpoint(R->PC.W)) {
            emulator->set_continue_mode(false);
            if (emulator->last_action_was_next())
                emulator->remove_breakpoint(R->PC.W);
            return INT_QUIT;
        }
        
        // continue execution for allotted time
        auto now = system_clock::now();
        if (now < emulator->execute_until())
            return INT_NONE;
    }
    
    // it is just a single step
    return INT_QUIT;
}

void PatchZ80(Z80 *R) { (void) R; }

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
    if (key != 0)
        keyboard_interrupt_ = true;
}

void Emulator::step()
{
    RunZ80(&z80_);
}

void Emulator::continue_()
{
    continue_mode_ = true;
}

void Emulator::stop()
{
    continue_mode_ = false;
}

void Emulator::next()
{
    switch (ram_get(z80_.PC.W)) {
        case 0xcd:  // CALL
        case 0xdc:
        case 0xfc:
        case 0xd4:
        case 0xc4:
        case 0xf4:
        case 0xec:
        case 0xe4:
        case 0xcc:
            add_breakpoint(z80_.PC.W + 3);
            set_continue_mode(true);
            last_action_was_next_ = true;
            break;
    }
    RunZ80(&z80_);
}

void Emulator::soft_reset()
{
    terminal_.reset();
    ResetZ80(&z80_);
}

uint16_t Emulator::pc() const
{
    return z80_.PC.W;
}

void Emulator::execute()
{
    if (continue_mode_) {
        execute_until_ = system_clock::now() + milliseconds(8);
        RunZ80(&z80_);
    }
}

void Emulator::add_breakpoint(uint16_t addr)
{
    breakpoints_.insert(addr);
}

void Emulator::remove_breakpoint(uint16_t addr)
{
    breakpoints_.erase(addr);
    last_action_was_next_ = false;
}

void Emulator::remove_all_breakpoints()
{
    breakpoints_.clear();
}

bool Emulator::is_breakpoint(uint16_t addr) const
{
    return breakpoints_.find(addr) != breakpoints_.end();
}
