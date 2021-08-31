#ifndef EMULATOR_MANAGER_HH
#define EMULATOR_MANAGER_HH

#include "compiler/debug.hh"
#include "ui/gui/gui.hh"
#include "ui/window/demowindow.hh"
#include "ui/window/loadprojectwindow.hh"
#include "ui/modal/messagebox.hh"
#include "ui/window/ramwindow.hh"
#include "ui/window/menuwindow.hh"
#include "ui/window/cpuwindow.hh"
#include "ui/window/terminalwindow.hh"
#include "ui/modal/sendkeypressmodal.hh"
#include "ui/window/codewindow.hh"
#include "ui/window/symbolselectwindow.hh"
#include "ui/window/storagewindow.hh"

class Manager {
public:
    Manager();
    ~Manager();
    
    void run(std::optional<std::string> const& project_to_load);
    
private:
    Properties         properties_;
    GUI                gui_;
    
    Emulator           emulator_;
    
    DemoWindow         demo_window_;
    LoadProjectWindow  load_project_window_;
    MessageBox         message_box_;
    SendKeypressModal  send_keypress_window_;
    RamWindow          ram_window_;
    CpuWindow          cpu_window_;
    TerminalWindow     terminal_window_;
    SymbolSelectWindow symbol_select_window_;
    CodeWindow         code_window_;
    StorageWindow      storage_window_;
    
    MenuWindow         menu_window_;
    
    const std::vector<Window*> menu_windows_ = { &code_window_, &ram_window_, &demo_window_, &cpu_window_, &terminal_window_, &storage_window_ },
                               other_windows_ = { &load_project_window_, &menu_window_, &message_box_, &send_keypress_window_, &symbol_select_window_ };
    
    std::optional<CodeModel> code_model_ {};
    
    std::string              last_project_loaded_;
    
    std::optional<ImageFile> image_file;
    
    void open_windows_from_last_time();
    bool load_project(std::string const& project_name);
};

#endif
