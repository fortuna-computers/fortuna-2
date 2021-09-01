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
    I_SD_STATUS     = 0x7,  // 0 = ok, 1 = error
};

static Emulator* emulator;

Emulator::Emulator()
{
    emulator = this;
}

void Emulator::initialize(std::vector<uint8_t> const& rom, std::optional<ImageFile>& image_file)
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
    
    image_file_ = image_file ? &image_file.value() : nullptr;
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
        case I_SD_B0:
            emulator->sdcard_register[0] = Value;
            break;
        case I_SD_B1:
            emulator->sdcard_register[1] = Value;
            break;
        case I_SD_B2:
            emulator->sdcard_register[2] = Value;
            break;
        case I_SD_B3:
            emulator->sdcard_register[3] = Value;
            break;
        case I_SD_ACTION:
            if (Value & 1)
                emulator->sdcard_write();
            else
                emulator->sdcard_read();
            break;
    }
}

byte InZ80(word Port)
{
    switch (Port & 0xff) {
        case I_TERMINAL: {
                auto key = emulator->last_keypress();
                emulator->keypress(0);  // clear last keypress
                return key;
            }
        case I_SD_STATUS:
            return emulator->sdcard_status();
    }
    return 0;
}

word LoopZ80(Z80 *R)
{
    // check if we're skipping ROM area (PC < 0x400)
    if (emulator->running_on_rom && R->PC.W >= 0x400) {
        emulator->running_on_rom = false;
        if (emulator->skip_rom_area) {
            emulator->skip_rom_area = false;
            emulator->set_continue_mode(false);
            return INT_QUIT;
        }
    }
    
    // check for interrupts
    if (emulator->keyboard_interrupt()) {
        emulator->reset_keyboard_interrupt();
        return 0x0;  // KEYBOARD_DEVICE
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

void Emulator::skip_rom()
{
    skip_rom_area = true;
    set_continue_mode(true);
    RunZ80(&z80_);
}

void Emulator::soft_reset()
{
    terminal_.reset();
    ResetZ80(&z80_);
    sdcard_register[0] = 0;
    sdcard_register[1] = 0;
    sdcard_register[2] = 0;
    sdcard_register[3] = 0;
    
    last_keypress_ = 0;
    keyboard_interrupt_ = false;
    continue_mode_ = false;
    last_action_was_next_ = false;
    running_on_rom = true;
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

void Emulator::sdcard_write() const
{
    uint32_t block = (uint32_t) sdcard_register[0]
                     | ((uint32_t) sdcard_register[1]) << 8
                     | ((uint32_t) sdcard_register[2]) << 16
                     | ((uint32_t) sdcard_register[3]) << 24;
    if (image_file_)
        image_file_->write_block_to_image(block, ram_);
}

void Emulator::sdcard_read()
{
    uint32_t block = (uint32_t) sdcard_register[0]
                     | ((uint32_t) sdcard_register[1]) << 8
                     | ((uint32_t) sdcard_register[2]) << 16
                     | ((uint32_t) sdcard_register[3]) << 24;
    if (image_file_)
        image_file_->read_block_from_image(block, ram_);
}

bool Emulator::sdcard_status() const
{
    return image_file_->fail_requests ? 1 : 0;
}
