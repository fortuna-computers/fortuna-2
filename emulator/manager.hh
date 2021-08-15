#ifndef EMULATOR_MANAGER_HH
#define EMULATOR_MANAGER_HH

#include "compiler/debug.hh"
#include "ui/window/mainwindow.hh"
#include "ui/window/demowindow.hh"
#include "ui/window/loadprojectwindow.hh"
#include "ui/modal/messagebox.hh"
#include "ui/window/ramwindow.hh"
#include "ui/window/menuwindow.hh"
#include "ui/window/cpuwindow.hh"
#include "ui/window/terminalwindow.hh"
#include "ui/modal/sendkeypressmodal.hh"
#include "ui/window/codewindow.hh"
#include "ui/window/fileselectwindow.hh"

class Manager {
public:
    Manager();
    ~Manager();
    
    void run();
    
private:
    MainWindow&        window_;
    DemoWindow         demo_window_;
    LoadProjectWindow  load_project_window_;
    MessageBox         message_box_;
    SendKeypressModal send_keypress_window_;
    MenuWindow         menu_window_;
    RamWindow          ram_window_;
    CpuWindow          cpu_window_;
    TerminalWindow     terminal_window_;
    CodeWindow         code_window_;
    FileSelectWindow   file_select_window_;
    
    const std::vector<Window*> menu_windows_ = { &code_window_, &ram_window_, &demo_window_, &cpu_window_, &terminal_window_ };
    
    std::optional<Debug> debug {};
    
    void load_project(std::string const& project_name);
    
    void open_windows_from_last_time();
};

#endif
