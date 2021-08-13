#ifndef EMULATOR_TERMINALWINDOW_HH
#define EMULATOR_TERMINALWINDOW_HH

#include <functional>
#include "../window.hh"

class TerminalWindow : public Window {
public:
    void draw() override;
    
    std::string name() const override { return "terminal"; }
    
    void on_send_keypress(std::function<void()> const& f) { on_send_keypress_ = f; }
    
private:
    std::function<void()> on_send_keypress_;
};

#endif
