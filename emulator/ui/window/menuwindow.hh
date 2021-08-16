#ifndef EMULATOR_MENUWINDOW_HH
#define EMULATOR_MENUWINDOW_HH

#include <functional>

#include "../window.hh"

class MenuWindow : public Window {
public:
    explicit MenuWindow(Emulator& emulator) : Window(emulator) {}
    
    void draw() override;
    void on_open_code(std::function<void()> const& f) { on_open_code_ = f; }
    void on_open_cpu(std::function<void()> const& f) { on_open_cpu_ = f; }
    void on_open_memory(std::function<void()> const& f) { on_open_memory_ = f; }
    void on_open_terminal(std::function<void()> const& f) { on_open_terminal_ = f; }
    void on_open_storage(std::function<void()> const& f) { on_open_storage_ = f; }
    void on_open_demo(std::function<void()> const& f) { on_open_demo_ = f; }
    
    std::string name() const override { return "menu_window"; }

private:
    std::function<void()> on_open_code_;
    std::function<void()> on_open_cpu_;
    std::function<void()> on_open_memory_;
    std::function<void()> on_open_terminal_;
    std::function<void()> on_open_storage_;
    std::function<void()> on_open_demo_;
};

#endif
