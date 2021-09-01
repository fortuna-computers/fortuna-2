#include "symbolselectwindow.hh"

#include "imgui.h"
#include "codewindow.hh"

void SymbolSelectWindow::draw()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 100, 250 });
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f, 0.5f });
    if (ImGui::BeginPopupModal(title(), &visible_)) {
        ImGui::Text("Choose a symbol to go to:");
        
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
        
        if (ImGui::BeginTable("##file", 2, tbl_flags)) {
            if (emulator_.stopped()) {
                ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
                ImGui::TableSetupColumn("Symbol", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                
                for (auto const& [name, location]: code_model_->debug().symbols) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    int sel_flags = ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns;
                    if (ImGui::Selectable(name.c_str(), false, sel_flags) && ImGui::IsMouseDoubleClicked(0)) {
                        auto oaddr = code_model_->go_to_symbol(name);
                        if (oaddr)
                            code_window_->set_show_this_line_on_next_frame(*oaddr);
                        ImGui::SetWindowFocus(CodeWindow::window_title().c_str());
                        visible_ = false;
                    }
                    ImGui::TableSetColumnIndex(1);
                    
                    auto it = code_model_->debug().location.find(location);
                    if (it != code_model_->debug().location.end())
                        ImGui::Text("%s:%zu", it->second.filename.c_str(), it->second.line);
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
    
    if (visible_)
        ImGui::OpenPopup(title());
}
