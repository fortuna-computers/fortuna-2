#ifndef EMULATOR_EMULATOR_HH
#define EMULATOR_EMULATOR_HH

#include <cstddef>
#include <cstdint>
#include <Z80.h>
#include "terminal.hh"

class Emulator {
public:
    static constexpr size_t MEMORY_SIZE = 32 * 1024;
    
    static Emulator& get();
    
    Emulator(Emulator const&) = delete;
    Emulator& operator=(Emulator const&) = delete;
    Emulator(Emulator&&) = delete;
    Emulator& operator=(Emulator&&) = delete;
    
    bool stopped() const { return true; }
    
    void    ram_set(uint16_t addr, uint8_t data);
    uint8_t ram_get(uint16_t addr) const;
    
    Z80 const& z80() const { return z80_; }
    Z80& z80() { return z80_; }
    
    Terminal const& terminal() const { return terminal_; }
    Terminal& terminal() { return terminal_; }
    
    static size_t ram_size();

private:
    Emulator() = default;
    Terminal terminal_ { 40, 25 };
    Z80 z80_ {};
    uint8_t ram_[MEMORY_SIZE] = { 0 };
};

#endif //EMULATOR_EMULATOR_HH
