#include "menuwindow.hh"

#include "imgui.h"

void MenuWindow::draw()
{
    ImGui::SetNextWindowSize(ImVec2(116, 170));
    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoResize);
    
    ImVec2 size = { 100.0, 0.0 };
    if (ImGui::Button("Code", size) && on_open_code_)
        on_open_code_();
    if (ImGui::Button("CPU", size) && on_open_cpu_)
        on_open_cpu_();
    if (ImGui::Button("Memory", size) && on_open_memory_)
        on_open_memory_();
    if (ImGui::Button("Terminal", size) && on_open_terminal_)
        on_open_terminal_();
    if (ImGui::Button("Storage", size) && on_open_storage_)
        on_open_storage_();
    if (ImGui::Button("Demo", size) && on_open_demo_)
        on_open_demo_();
    
    ImGui::End();
}
