#ifndef EMULATOR_MANAGER_HH
#define EMULATOR_MANAGER_HH

#include "compiler/debug.hh"
#include "ui/mainwindow.hh"
#include "ui/demowindow.hh"
#include "ui/loadprojectwindow.hh"
#include "ui/messagebox.hh"

class Manager {
public:
    Manager();
    
    void run();
    
private:
    MainWindow&       window_;
    DemoWindow        demo_window_;
    LoadProjectWindow load_project_window_;
    MessageBox        message_box_;
    
    std::optional<Debug> debug {};
    
    void load_project(std::string const& project_name);
};

#endif
