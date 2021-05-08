#ifndef EMULATOR_TERMINAL_HH
#define EMULATOR_TERMINAL_HH

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

class Terminal {
public:
    Terminal(size_t lines, size_t columns);
    
    void add_char(uint16_t chr);
    
    size_t cursor_x() const { return cursor_x_; }
    size_t cursor_y() const { return cursor_y_; }
    std::vector<std::string> const& text() const { return text_; }
    
    size_t lines() const { return lines_; }
    size_t columns() const { return columns_; }
    
    void reset();
    
    static constexpr uint16_t Return = 13;
    static constexpr uint16_t Backspace = '\b';
    
private:
    size_t lines_, columns_;
    size_t cursor_x_, cursor_y_ = 0;
    std::vector<std::string> text_ {};
    
    void pull_screen_up();
};

#endif
