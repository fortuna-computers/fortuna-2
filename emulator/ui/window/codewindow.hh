#ifndef EMULATOR_CODEWINDOW_HH
#define EMULATOR_CODEWINDOW_HH

#include "../window.hh"
#include "../../compiler/debug.hh"
#include "../model/codemodel.hh"

class CodeWindow : public Window {
public:
    void draw() override;
    
    std::string name() const override { return "code"; }
    
    void set_debug(Debug const& debug);

private:
    void draw_buttons();
    void draw_code();
    void draw_footer();
    
    std::optional<CodeModel> code_model_ {};
    bool scroll_to_pc_ = true;
    std::optional<size_t> show_this_line_on_next_frame {};
};

#endif
