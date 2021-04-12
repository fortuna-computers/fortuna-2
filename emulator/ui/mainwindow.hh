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
    
    MainWindow(MainWindow const&) = delete;
    MainWindow& operator=(MainWindow const&) = delete;
    MainWindow(MainWindow&&) = delete;
    MainWindow& operator=(MainWindow&&) = delete;
    
    void run();
    void add_window(Window& window) { children_.push_back(&window); }

    static MainWindow& get();
    
    void force_end_frame();
    
    std::string property(std::string const& name) const;
    bool        property_bool(std::string const& name) const;
    void        set_property(std::string const& name, std::string const& value);
    void        set_property_bool(std::string const& name, bool value);
    
    ImGuiIO& io() const { return *io_; }
    
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
