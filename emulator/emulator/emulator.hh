#ifndef EMULATOR_EMULATOR_HH
#define EMULATOR_EMULATOR_HH

#include <cstddef>
#include <cstdint>

class Emulator {
public:
    static Emulator& get();
    
    Emulator(Emulator const&) = delete;
    Emulator& operator=(Emulator const&) = delete;
    Emulator(Emulator&&) = delete;
    Emulator& operator=(Emulator&&) = delete;
    
    [[nodiscard]] bool stopped() const { return true; }
    
                  void    ram_set(uint16_t addr, uint8_t data);
    [[nodiscard]] uint8_t ram_get(uint16_t addr) const;
    
    static size_t ram_size() ;

private:
    Emulator() = default;
};

#endif //EMULATOR_EMULATOR_HH
