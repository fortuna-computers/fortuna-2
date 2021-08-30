#include "codewindow.hh"

#include "imgui.h"
#include "imgui_internal.h"
#include "../gui/gui.hh"

void CodeWindow::draw()
{
    if (last_state_was_running_ && !emulator_.continue_mode())
        code_model_->update(true);
    last_state_was_running_ = emulator_.continue_mode();
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 0, 250 });
    if (ImGui::Begin(window_title().c_str(), &visible_)) {
        draw_buttons();
        draw_code();
        draw_footer();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void CodeWindow::draw_buttons()
{
    if (emulator_.stopped()) {
        ImGui::PushButtonRepeat(true);
        if (ImGui::Button("Step (F7)") || ImGui::IsKeyPressed(F7, true)) {
            emulator_.step();
            scroll_to_pc_ = true;
            code_model_->update();
        }
        ImGui::SameLine();
        if (ImGui::Button("Next (F8)") || ImGui::IsKeyPressed(F8, true)) {
            emulator_.next();
            scroll_to_pc_ = true;
            last_state_was_running_ = true;
        }
        ImGui::PopButtonRepeat();
        ImGui::SameLine();
        if (ImGui::Button("Run (F9)") || ImGui::IsKeyPressed(F9, false)) {
            scroll_to_pc_ = true;
            emulator_.continue_();
            last_state_was_running_ = true;
        }
        if (emulator_.running_on_rom) {
            ImGui::SameLine();
            if (ImGui::Button("Skip ROM (F10)") || ImGui::IsKeyPressed(F10, false)) {
                emulator_.skip_rom();
                scroll_to_pc_ = true;
                last_state_was_running_ = true;
            }
        }
    } else {
        if (ImGui::Button("Stop (Ctrl+C)") || (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed('C', false))) {
            emulator_.stop();
            code_model_->update();
            scroll_to_pc_ = true;
        }
    }
}

void CodeWindow::draw_files_combo()
{
    if (emulator_.stopped()) {
        int selected_file = -1;
        auto it = std::find(files_.begin(),  files_.end(), code_model_->file_selected());
        if (it != files_.end())
            selected_file = (int) (it - files_.begin());
        
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
        
        ImGui::SetNextItemWidth(180.f);
        if (ImGui::Combo("##file", &selected_file, files_.data(), (int) files_.size()))
            code_model_->set_file(files_.at(selected_file));
        
        ImGui::PopStyleColor(6);
    } else {
        ImGui::Button("Code in execution...");
    }
}

void CodeWindow::draw_code()
{
    if (!code_model_)
        return;
    
    draw_files_combo();
    
    // table
    static int tbl_flags = ImGuiTableFlags_BordersOuterH
                           | ImGuiTableFlags_BordersOuterV
                           | ImGuiTableFlags_BordersInnerV
                           | ImGuiTableFlags_BordersOuter
                           | ImGuiTableFlags_RowBg
                           | ImGuiTableFlags_ScrollY
                           | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
    static ImU32 pc_row_color = ImGui::GetColorU32(ImVec4(0.3f, 0.7f, 0.3f, 0.65f));
    static ImU32 bkp_cell_color = ImGui::GetColorU32(ImVec4(0.8f, 0.2f, 0.2f, 0.65f));
    
    ImVec2 size = ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y - 42);
    if (ImGui::BeginTable("##code", 3, tbl_flags, size)) {
    
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Bytes", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Code", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        
        if (emulator_.stopped()) {
            size_t nline = 1;
            for (auto const& line: code_model_->lines()) {
                
                ImGui::TableNextRow();
                
                if (line.address.has_value()) {
                    if (*line.address == emulator_.pc()) {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, pc_row_color);
                        if (scroll_to_pc_) {
                            ImRect rect { ImGui::GetItemRectMin(), ImGui::GetItemRectMin() };
                            rect.Expand({ 0, 60 });
                            ImGui::ScrollToBringRectIntoView(ImGui::GetCurrentWindow(), rect);
                            scroll_to_pc_ = false;
                        }
                    }
                    
                    bool is_breakpoint = emulator_.is_breakpoint(*line.address);
                    
                    if (is_breakpoint)
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, bkp_cell_color, 0);
                    
                    if (nline == show_this_line_on_next_frame_.value_or(-1)) {
                        ImGui::SetScrollHereY();
                        show_this_line_on_next_frame_.reset();
                    }
                    
                    ImGui::TableSetColumnIndex(0);
                    char buf[5];
                    sprintf(buf, "%04X", *line.address);
                    if (ImGui::Selectable(buf)) {
                        if (is_breakpoint)
                            code_model_->remove_breakpoint(nline);
                        else
                            code_model_->add_breakpoint(nline);
                    }
                }
    
                ImGui::TableSetColumnIndex(1);
                char buf[256] = { 0 };
                int pos = 0;
    
                for (auto b: line.bytes) {
                    if (pos > sizeof(buf) - 5)
                        break;
                    pos += sprintf(&buf[pos], "%02X ", b);
                }
    
                if (!line.bytes.empty())
                    ImGui::Text("%s", buf);
    
                ImGui::TableSetColumnIndex(2);
                
                if (line.comment_start) {
                    ImGui::Text("%s", line.code.substr(0, *line.comment_start).c_str());
                    ImGui::SameLine(0, 0);
                    ImGui::TextColored(ImVec4(1.0f, 0.7f, 1.0f, 1.0f), "%s", line.code.substr(*line.comment_start).c_str());
                } else {
                    ImGui::Text("%s", line.code.c_str());
                }
                
                ++nline;
            }
        }
        
        ImGui::EndTable();
    }
    
}

void CodeWindow::draw_footer()
{
    if (emulator_.stopped()) {
        ImGui::Text("Click on the address to set a breakpoint.");
        
        if (ImGui::Button("Recompile project (Ctrl+R)") || (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed('R', false))) {
            on_recompile_project_();
        }
        ImGui::SameLine();
        if (ImGui::Button("Soft Reset")) {
            emulator_.soft_reset();
            code_model_->update();
            scroll_to_pc_ = true;
            update();
        }
        ImGui::SameLine();
        if (ImGui::Button("Go to symbol... (S)") || ImGui::IsKeyPressed('S', false))
            symbol_select_window_.set_visible(true);
    }
}

void CodeWindow::set_show_this_line_on_next_frame(size_t line)
{
    show_this_line_on_next_frame_ = line;
}

void CodeWindow::set_code_model(CodeModel& code_model)
{
    code_model_ = &code_model;
    
    files_.clear();
    for (auto const& file: code_model_->debug().files)
        files_.push_back(file.first.c_str());
    std::sort(files_.begin(),  files_.end(), [](const char* a, const char* b) { return strcmp(a, b) < 0; });
}
