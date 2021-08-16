#ifndef EMULATOR_FILESELECTWINDOW_HH
#define EMULATOR_FILESELECTWINDOW_HH

#include "../window.hh"
#include "../../compiler/debug.hh"
#include "../model/codemodel.hh"
#include "codewindow.hh"

class FileSelectWindow : public Window {
public:
    void draw() override;
    
    std::string name() const override { return "file_select"; }
    
    void set_code_model(CodeModel& code_model) { code_model_ = &code_model; }

private:
    CodeModel* code_model_ = nullptr;
};

#endif
