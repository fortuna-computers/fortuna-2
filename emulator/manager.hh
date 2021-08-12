#ifndef EMULATOR_MANAGER_HH
#define EMULATOR_MANAGER_HH

#include "compiler/debug.hh"
#include "ui/mainwindow.hh"
#include "ui/demowindow.hh"
#include "ui/loadprojectwindow.hh"
#include "ui/messagebox.hh"
#include "ui/ramwindow.hh"
#include "ui/menuwindow.hh"
#include "ui/cpuwindow.hh"
#include "ui/terminalwindow.hh"
#include "ui/sendkeypresswindow.hh"
#include "ui/codewindow.hh"

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
    SendKeypressWindow send_keypress_window_;
    MenuWindow         menu_window_;
    RamWindow          ram_window_;
    CpuWindow          cpu_window_;
    TerminalWindow     terminal_window_;
    CodeWindow         code_window_;
    
    const std::vector<Window*> menu_windows_ = { &code_window_, &ram_window_, &demo_window_, &cpu_window_, &terminal_window_ };
    
    std::optional<Debug> debug {};
    
    void load_project(std::string const& project_name);
    
    void open_windows_from_last_time();
};

#endif
