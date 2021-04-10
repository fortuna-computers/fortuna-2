#include "ui/glfwwindow.hh"
#include "ui/demowindow.hh"

int main()
{
    DemoWindow demo_window;
    
    GLFWWindow& window = GLFWWindow::get();
    window.add_window(demo_window);
    window.run();
    
    return 0;
}
