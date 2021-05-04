#ifndef EMULATOR_TERMINALWINDOW_HH
#define EMULATOR_TERMINALWINDOW_HH

#include "window.hh"

class TerminalWindow : public Window {
public:
    void draw() override;
    
    std::string name() const override { return "terminal"; }
    
};

#endif
