#ifndef EMULATOR_EMULATOR_HH
#define EMULATOR_EMULATOR_HH

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <Z80.h>
#include <optional>
#include <fstream>
#include <unordered_set>
#include "terminal.hh"
#include "../compiler/projectfile.hh"

using namespace std::chrono;

class Emulator {
public:
    static constexpr size_t MEMORY_SIZE = 32 * 1024;
    
    Emulator();
    
    Emulator(Emulator const&) = delete;
    Emulator& operator=(Emulator const&) = delete;
    Emulator(Emulator&&) = delete;
    Emulator& operator=(Emulator&&) = delete;
    
    void initialize(std::vector<uint8_t> const& rom, ProjectFile const& project_file, std::string const& sources_path);
    
    bool stopped() const { return !continue_mode_; }
    
    bool continue_mode() const { return continue_mode_; }
    void set_continue_mode(bool continue_mode) { continue_mode_ = continue_mode; }
    
    time_point<system_clock> const& execute_until() const { return execute_until_; }
    
    void execute();
    
    void step();
    void continue_();
    void stop();
    void next();
    void soft_reset();
    
    uint16_t pc() const;
    
    void    ram_set(uint16_t addr, uint8_t data);
    uint8_t ram_get(uint16_t addr) const;
    
    Z80 const& z80() const { return z80_; }
    Z80&       z80() { return z80_; }
    
    Terminal const& terminal() const { return terminal_; }
    Terminal&       terminal() { return terminal_; }
    
    static size_t ram_size();
    
    void    keypress(uint8_t key);
    uint8_t last_keypress() const { return last_keypress_; }
    
    bool keyboard_interrupt() const { return keyboard_interrupt_; }
    void reset_keyboard_interrupt() { keyboard_interrupt_ = false; }
    
    void add_breakpoint(uint16_t addr);
    void remove_breakpoint(uint16_t addr);
    void remove_all_breakpoints();
    bool is_breakpoint(uint16_t addr) const;
    
    bool last_action_was_next() const { return last_action_was_next_; }

private:
    Terminal                          terminal_ { 25, 40 };
    Z80                               z80_ {};
    uint8_t                           ram_[MEMORY_SIZE] = { 0 };
    uint8_t                           last_keypress_ = 0;
    bool                              keyboard_interrupt_ = false;
    bool                              continue_mode_ = false;
    bool                              last_action_was_next_ = false;
    std::string                       image_filename_ = "";
    std::optional<std::ifstream>      image_ {};
    std::unordered_set<uint16_t>      breakpoints_;
    time_point<system_clock>          execute_until_;
};

#endif //EMULATOR_EMULATOR_HH
