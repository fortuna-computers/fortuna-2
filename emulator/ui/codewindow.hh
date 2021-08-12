#ifndef EMULATOR_CODEWINDOW_HH
#define EMULATOR_CODEWINDOW_HH

#include "window.hh"

class CodeWindow : public Window {
public:
    void draw() override;
    
    std::string name() const override { return "code"; }

private:
    void draw_buttons();
    void draw_code();
    void draw_footer();
};

#endif
