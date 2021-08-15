#include "fileselectwindow.hh"

#include "imgui.h"
#include "../../emulator/emulator.hh"

void FileSelectWindow::draw()
{
    if (ImGui::Begin("Choose file", &show_choose_file)) {
        ImGui::Text("Choose a file to go to:");
        
        static int tbl_flags = ImGuiTableFlags_BordersOuterH
                               | ImGuiTableFlags_BordersOuterV
                               | ImGuiTableFlags_BordersInnerV
                               | ImGuiTableFlags_BordersOuter
                               | ImGuiTableFlags_RowBg
                               | ImGuiTableFlags_ScrollY
                               | ImGuiTableFlags_Resizable
                               | ImGuiTableFlags_Reorderable
                               | ImGuiTableFlags_Hideable
                               | ImGuiTableFlags_Sortable
                               | ImGuiTableFlags_SortTristate;
        
        if (ImGui::BeginTable("##file", 1, tbl_flags)) {
            if (!Emulator::get().stopped() && debug_) {
                ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
                ImGui::TableSetupColumn("File Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                
                for (auto const& [filename, _]: debug_->files) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Selectable(filename.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick) && ImGui::IsMouseDoubleClicked(0)) {
                        p().codeview().set_file(fname);
                        ImGui::SetWindowFocus("Code debugger");
                    }
                }
            }
            
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

