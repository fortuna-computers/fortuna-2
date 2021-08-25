#ifndef EMULATOR_SYMBOLSELECTWINDOW_HH
#define EMULATOR_SYMBOLSELECTWINDOW_HH

#include "../window.hh"
#include "../model/codemodel.hh"

class SymbolSelectWindow : public Window {
public:
    explicit SymbolSelectWindow(Emulator& emulator) : Window(emulator) {}
    
    void draw() override;
    
    std::string name() const override { return "symbol_select"; }
    static const char* title() { return "Choose symbol"; }
    
    void set_code_model(CodeModel& code_model) { code_model_ = &code_model; }
    void set_code_window(class CodeWindow* code_window) { code_window_ = code_window; }

private:
    CodeModel* code_model_ = nullptr;
    class CodeWindow* code_window_ = nullptr;
};

#endif
