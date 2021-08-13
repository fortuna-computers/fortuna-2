#ifndef EMULATOR_DEMOWINDOW_HH
#define EMULATOR_DEMOWINDOW_HH

#include "../window.hh"

class DemoWindow : public Window {
public:
    void draw() override;
    std::string name() const override { return "demo_window"; }
};

#endif
