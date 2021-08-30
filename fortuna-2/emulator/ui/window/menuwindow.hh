#ifndef EMULATOR_MENUWINDOW_HH
#define EMULATOR_MENUWINDOW_HH

#include <functional>

#include "../window.hh"
#include "codewindow.hh"
#include "cpuwindow.hh"
#include "ramwindow.hh"
#include "terminalwindow.hh"
#include "demowindow.hh"
#include "storagewindow.hh"

struct MenuChildrenWindows {
    CodeWindow&     code_window;
    CpuWindow&      cpu_window;
    RamWindow&      ram_window;
    TerminalWindow& terminal_window;
    DemoWindow&     demo_window;
    StorageWindow&   storage_window;
};

class MenuWindow : public Window {
public:
    explicit MenuWindow(Emulator& emulator, MenuChildrenWindows const& menu_children_windows)
        : Window(emulator), menu_children_windows_(menu_children_windows) {}
    
    void draw() override;
    
    std::string name() const override { return "menu_window"; }

private:
    MenuChildrenWindows menu_children_windows_;
};

#endif
