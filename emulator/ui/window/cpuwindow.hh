#ifndef EMULATOR_CPUWINDOW_HH
#define EMULATOR_CPUWINDOW_HH

#include "../window.hh"

class CpuWindow : public Window {
public:
    explicit CpuWindow(Emulator& emulator) : Window(emulator) {}
    
    void draw() override;
    
    [[nodiscard]] std::string name() const override { return "cpu_window"; }
};

#endif
