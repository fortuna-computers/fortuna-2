#ifndef EMULATOR_SENDKEYPRESSWINDOW_HH
#define EMULATOR_SENDKEYPRESSWINDOW_HH

#include <vector>
#include "window.hh"

class SendKeypressWindow : public Window {
public:
    void draw() override;
    
    std::string name() const override { return "sendkeypress"; }

private:
    bool do_keypress();
    static std::vector<uint8_t> translate_keypress(int key, bool ctrl, bool shift, bool alt, bool super) ;
};

#endif
