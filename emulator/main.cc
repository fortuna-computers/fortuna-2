#include <iostream>
#include "ui/mainwindow.hh"
#include "ui/demowindow.hh"
#include "ui/loadprojectwindow.hh"

int main()
{
    MainWindow& window = MainWindow::get();
    
    DemoWindow demo_window;
    
    LoadProjectWindow load_project_window;
    load_project_window.on_start_executing([](std::string const& filename) {
        std::cout << filename << "\n";
    });
    load_project_window.set_visible(true);
    
    window.add_window(demo_window);
    window.add_window(load_project_window);
    
    window.run();
    
    return 0;
}
