#ifndef EMULATOR_DEMOWINDOW_HH
#define EMULATOR_DEMOWINDOW_HH

#include "../window.hh"

class DemoWindow : public Window {
public:
    explicit DemoWindow(Emulator& emulator) : Window(emulator) {}
    
    void draw() override;
    std::string name() const override { return "demo_window"; }
};

#endif
