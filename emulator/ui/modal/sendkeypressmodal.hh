#ifndef EMULATOR_SENDKEYPRESSMODAL_HH
#define EMULATOR_SENDKEYPRESSMODAL_HH

#include <vector>
#include "../window.hh"

class SendKeypressModal : public Window {
public:
    explicit SendKeypressModal(Emulator& emulator) : Window(emulator) {}
    
    void draw() override;
    
    std::string name() const override { return "sendkeypress"; }

private:
    bool do_keypress();
    static std::vector<uint8_t> translate_keypress(int key, bool ctrl, bool shift, bool alt, bool super) ;
};

#endif
