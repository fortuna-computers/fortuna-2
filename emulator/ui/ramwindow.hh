#ifndef EMULATOR_RAMWINDOW_HH
#define EMULATOR_RAMWINDOW_HH

#include <cstdint>
#include "window.hh"

class RamWindow : public Window {
public:
    void draw() override;

private:
    void draw_memory_table();
    void go_to_page_number(int page);
    
    int16_t page_number_ = 0;
    
    void draw_stack();
};

#endif
