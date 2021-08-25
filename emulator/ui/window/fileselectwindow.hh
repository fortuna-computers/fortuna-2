#ifndef EMULATOR_FILESELECTWINDOW_HH
#define EMULATOR_FILESELECTWINDOW_HH

#include "../window.hh"
#include "../../compiler/debug.hh"
#include "../model/codemodel.hh"

class FileSelectWindow : public Window {
public:
    explicit FileSelectWindow(Emulator& emulator) : Window(emulator) {}
    
    void draw() override;
    
    std::string name() const override { return "file_select"; }
    static const char* title() { return "Choose file"; }
    
    void set_code_model(CodeModel& code_model) { code_model_ = &code_model; }

private:
    CodeModel* code_model_ = nullptr;
};

#endif
