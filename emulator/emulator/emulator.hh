#ifndef EMULATOR_EMULATOR_HH
#define EMULATOR_EMULATOR_HH

#include <cstddef>
#include <cstdint>
#include <Z80.h>

class Emulator {
public:
    static constexpr size_t MEMORY_SIZE = 32 * 1024;
    
    static Emulator& get();
    
    Emulator(Emulator const&) = delete;
    Emulator& operator=(Emulator const&) = delete;
    Emulator(Emulator&&) = delete;
    Emulator& operator=(Emulator&&) = delete;
    
    [[nodiscard]] bool stopped() const { return true; }
    
                  void    ram_set(uint16_t addr, uint8_t data);
    [[nodiscard]] uint8_t ram_get(uint16_t addr) const;
    
    [[nodiscard]] Z80 const& z80() const { return z80_; }
    [[nodiscard]] Z80& z80() { return z80_; }
    
    static size_t ram_size();

private:
    Emulator() = default;
    Z80 z80_ {};
    uint8_t ram_[MEMORY_SIZE] = { 0 };
};

#endif //EMULATOR_EMULATOR_HH
