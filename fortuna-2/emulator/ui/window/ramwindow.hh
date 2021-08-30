#ifndef EMULATOR_RAMWINDOW_HH
#define EMULATOR_RAMWINDOW_HH

#include <cstdint>
#include "../window.hh"

class RamWindow : public Window {
public:
    explicit RamWindow(Emulator& emulator) : Window(emulator) {}
    
    void draw() override;
    std::string name() const override { return "ram_window"; }

private:
    void draw_memory_table() const;
    void go_to_page_number(int page);
    
    int16_t page_number_ = 0;
    
    void draw_stack();
};

#endif
