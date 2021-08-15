#ifndef EMULATOR_FILESELECTWINDOW_HH
#define EMULATOR_FILESELECTWINDOW_HH

#include "../window.hh"
#include "../../compiler/debug.hh"

class FileSelectWindow : public Window {
public:
    void draw() override;
    
    std::string name() const override { return "file_select"; }
    
    void set_debug(Debug const& debug) { debug_ = debug; }

private:
    std::optional<Debug> debug_;
    
};

#endif
