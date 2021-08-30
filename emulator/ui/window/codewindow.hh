#ifndef EMULATOR_CODEWINDOW_HH
#define EMULATOR_CODEWINDOW_HH

#include <functional>
#include "../window.hh"
#include "../../compiler/debug.hh"
#include "../model/codemodel.hh"
#include "symbolselectwindow.hh"

class CodeWindow : public Window {
public:
    CodeWindow(Emulator& emulator, SymbolSelectWindow& symbol_select_window)
        : Window(emulator), symbol_select_window_(symbol_select_window) {}
    
    void draw() override;
    
    std::string name() const override { return "code"; }
    
    static std::string window_title() { return "Code"; }
    
    void set_code_model(CodeModel& code_model);
    void set_show_this_line_on_next_frame(size_t line);
    
    void on_recompile_project(std::function<void()> const& f) { on_recompile_project_ = f; }

private:
    void draw_buttons();
    void draw_files_combo();
    void draw_code();
    void draw_footer();
    
    SymbolSelectWindow& symbol_select_window_;
    
    CodeModel* code_model_ = nullptr;
    bool scroll_to_pc_ = true;
    std::optional<size_t> show_this_line_on_next_frame_ {};
    
    std::function<void()> on_recompile_project_;
    std::vector<const char*> files_;
    
    bool last_state_was_running_ = false;
};

#endif
