#include <iostream>
#include "ui/glfwwindow.hh"
#include "ui/demowindow.hh"
#include "ui/loadprojectwindow.hh"

int main()
{
    DemoWindow demo_window;
    LoadProjectWindow load_project_window;
    load_project_window.on_start_executing([](std::string const& filename) {
        std::cout << filename << "\n";
    });
    
    GLFWWindow& window = GLFWWindow::get();
    window.add_window(demo_window);
    window.add_window(load_project_window);
    window.run();
    
    return 0;
}
