#ifndef EMULATOR_EMULATOR_HH
#define EMULATOR_EMULATOR_HH

#include <cstddef>
#include <cstdint>

class Emulator {
public:
    static Emulator& get();
    
    [[nodiscard]] bool stopped() const { return true; }
    
                  void    ram_set(uint16_t addr, uint8_t data);
    [[nodiscard]] uint8_t ram_get(uint16_t addr) const;
    
    static size_t ram_size() ;
};

#endif //EMULATOR_EMULATOR_HH
