#include "loadprojectwindow.hh"

void LoadProjectWindow::draw()
{
    ImGui::Begin("Welcome to Z80AW debugger", nullptr);
    
    ImGui::Text("Select project file"); ImGui::SameLine();
    ImGui::InputText("##b", project_file_, sizeof project_file_); ImGui::SameLine();
    if (ImGui::Button("..."))
        file_browser_.Open();
    
    ImGui::Separator();
    
    if (ImGui::Button("Start execution (F12)") || ImGui::IsKeyPressed(F12, false)) {
        on_start_executing_(project_file_);
    }
    
    ImGui::End();
    
    file_browser_.Display();
    if (file_browser_.HasSelected()) {
        strcpy(project_file_, file_browser_.GetSelected().c_str());
        file_browser_.ClearSelected();
    }
}

LoadProjectWindow::LoadProjectWindow()
{
    file_browser_.SetTitle("title");
    file_browser_.SetTypeFilters({ ".yaml" });
}

