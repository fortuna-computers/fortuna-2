#ifndef EMULATOR_MAINWINDOW_HH
#define EMULATOR_MAINWINDOW_HH

#include <memory>
#include <vector>
#include <unordered_map>
#include <imgui.h>
#include "window.hh"

class MainWindow {
public:
    ~MainWindow();
    
    void run();
    void add_window(Window& window) { children_.push_back(&window); }

    static MainWindow& get();
    
    void force_end_frame();
    
    std::string property(std::string const& name) const;
    void        set_property(std::string const& name, std::string const& value);
    
    ImGuiIO& io() const;
    
private:
    MainWindow();
    void on_error(int error, const char* description);
    void initialize_properties(struct ImGuiContext* p_context);
    
    struct GLFWwindow* window_;
    std::vector<Window*> children_;
    std::unordered_map<std::string, std::string> properties_ {};
    ImGuiContext* context_;
    ImGuiIO* io_;
    
    static constexpr float background_color[] = { 0.45f, 0.55f, 0.60f };
};

#endif
