#ifndef EMULATOR_CODEWINDOW_HH
#define EMULATOR_CODEWINDOW_HH

#include <functional>
#include "../window.hh"
#include "../../compiler/debug.hh"
#include "../model/codemodel.hh"

class CodeWindow : public Window {
public:
    void draw() override;
    
    std::string name() const override { return "code"; }
    
    void set_code_model(CodeModel& code_model) { code_model_ = &code_model; }
    
    void on_go_to_file(std::function<void()> const& f) { on_go_to_file_ = f; }
    
private:
    void draw_buttons();
    void draw_code();
    void draw_footer();
    
    CodeModel* code_model_ = nullptr;
    bool scroll_to_pc_ = true;
    std::optional<size_t> show_this_line_on_next_frame {};
    
    std::function<void()> on_go_to_file_;
};

#endif
