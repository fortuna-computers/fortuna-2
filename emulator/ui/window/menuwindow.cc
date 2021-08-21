#include "menuwindow.hh"

#include "imgui.h"

void MenuWindow::draw()
{
    ImGui::SetNextWindowSize(ImVec2(116, 170));
    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoResize);
    
    ImVec2 size = { 100.0, 0.0 };
    if (ImGui::Button("Code", size))
        menu_children_windows_.code_window.set_visible(true);
    if (ImGui::Button("CPU", size))
        menu_children_windows_.cpu_window.set_visible(true);
    if (ImGui::Button("Memory", size))
        menu_children_windows_.ram_window.set_visible(true);
    if (ImGui::Button("Terminal", size))
        menu_children_windows_.terminal_window.set_visible(true);
    if (ImGui::Button("Storage", size))
        menu_children_windows_.storage_window.set_visible(true);
    if (ImGui::Button("Demo", size))
        menu_children_windows_.demo_window.set_visible(true);
    
    ImGui::End();
}
