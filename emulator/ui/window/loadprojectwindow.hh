#ifndef EMULATOR_LOADPROJECTWINDOW_HH
#define EMULATOR_LOADPROJECTWINDOW_HH

#include <functional>
#include "imgui.h"
#include "imfilebrowser.h"
#include "../window.hh"

class LoadProjectWindow : public Window {
public:
    LoadProjectWindow();
    void draw() override;
    std::string name() const override { return "load_project_window"; }
    
    void on_start_executing(std::function<void(std::string const& filename)> const& f) { on_start_executing_ = f; }

private:
    char project_file_[4096] {0};
    ImGui::FileBrowser file_browser_;
    std::function<void(std::string const&)> on_start_executing_;
};

#endif
