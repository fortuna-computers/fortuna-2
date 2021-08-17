#include "loadprojectwindow.hh"
#include "../gui/gui.hh"

LoadProjectWindow::LoadProjectWindow(Emulator& emulator, Properties& properties)
    : Window(emulator), properties_(properties)
{
    file_browser_.SetTitle("title");
    file_browser_.SetTypeFilters({ ".yml", ".yaml", "" });
    file_browser_.SetCurrentTypeFilterIndex(1);
}

void LoadProjectWindow::draw()
{
    if (project_file_[0] == '\0')
        strcpy(project_file_, properties_.property("project_file").c_str());
    
    ImGui::SetNextWindowSize(ImVec2(600, 82));
    ImGui::Begin("Welcome to Mini-Z80 debugger", nullptr, ImGuiWindowFlags_NoResize);
    
    ImGui::Text("Select project file"); ImGui::SameLine();
    ImGui::InputText("##b", project_file_, sizeof project_file_); ImGui::SameLine();
    if (ImGui::Button("..."))
        file_browser_.Open();
    
    ImGui::Separator();
    
    if (ImGui::Button("Start execution (F12)") || ImGui::IsKeyPressed(F12, false)) {
        properties_.set_property("project_file", project_file_);
        on_start_executing_(project_file_);
    }
    
    ImGui::End();
    
    file_browser_.Display();
    if (file_browser_.HasSelected()) {
        strcpy(project_file_, file_browser_.GetSelected().c_str());
        file_browser_.ClearSelected();
    }
}
