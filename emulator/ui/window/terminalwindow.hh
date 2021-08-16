#ifndef EMULATOR_TERMINALWINDOW_HH
#define EMULATOR_TERMINALWINDOW_HH

#include <functional>
#include "../window.hh"
#include "../modal/sendkeypressmodal.hh"

class TerminalWindow : public Window {
public:
    TerminalWindow(Emulator& emulator, SendKeypressModal& send_keypress_modal)
        : Window(emulator), send_keypress_modal_(send_keypress_modal) {}
    
    void draw() override;
    
    std::string name() const override { return "terminal"; }
    
private:
    SendKeypressModal&    send_keypress_modal_;
};

#endif
