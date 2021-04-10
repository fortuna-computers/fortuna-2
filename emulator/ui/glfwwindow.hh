#ifndef EMULATOR_GLFWWINDOW_HH
#define EMULATOR_GLFWWINDOW_HH

#include <memory>
#include <vector>
#include "window.hh"

class GLFWWindow {
public:
    ~GLFWWindow();
    
    void run();
    void add_window(Window& window) { children_.push_back(&window); }

    static GLFWWindow& get();
    
private:
    GLFWWindow();
    void on_error(int error, const char* description);
    
    struct GLFWwindow* window_;
    std::vector<Window*> children_;
    
    static constexpr float background_color[] = { 0.45f, 0.55f, 0.60f };
};

#endif
