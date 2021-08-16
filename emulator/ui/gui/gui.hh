#ifndef EMULATOR_GUI_HH
#define EMULATOR_GUI_HH

#include <memory>
#include <vector>
#include <unordered_map>
#include <imgui.h>
#include "../window.hh"

class GUI {
public:
    GUI();
    ~GUI();
    
    GUI(GUI const&) = delete;
    GUI& operator=(GUI const&) = delete;
    GUI(GUI&&) = delete;
    GUI& operator=(GUI&&) = delete;
    
    void run();
    void add_window(Window& window) { children_.push_back(&window); }

    void force_end_frame();
    
    ImGuiIO& io() const { return *io_; }
    ImGuiContext* context() const { return context_; }

private:
    void on_error(int error, const char* description);
    
    struct GLFWwindow* window_;
    std::vector<Window*> children_;
    ImGuiContext* context_;
    ImGuiIO* io_;
    
    static constexpr float background_color[] = { 0.45f, 0.55f, 0.60f };
};

#endif
