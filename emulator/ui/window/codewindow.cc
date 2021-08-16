#include "codewindow.hh"

#include "imgui.h"
#include "mainwindow.hh"
#include "../../emulator/emulator.hh"

void CodeWindow::draw()
{
    if (ImGui::Begin(window_title().c_str(), &visible_)) {
        draw_buttons();
        draw_code();
        draw_footer();
    }
    ImGui::End();
}

void CodeWindow::draw_buttons()
{
    Emulator& emulator = Emulator::get();
    MainWindow& main_window = MainWindow::get();
    
    if (emulator.stopped()) {
        if (ImGui::Button("Step (F7)") || ImGui::IsKeyPressed(F7, false)) {
            emulator.step();
            scroll_to_pc_ = true;
            code_model_->update();
        }
        ImGui::SameLine();
        if (ImGui::Button("Next (F8)") || ImGui::IsKeyPressed(F8, false)) {
            /*
            emulator.next();
            code_model_->update(false);
            scroll_to_pc_ = true;
             */
        }
        ImGui::SameLine();
        if (ImGui::Button("Run (F9)") || ImGui::IsKeyPressed(F9, false)) {
            /*
            emulator.continue_();
             */
        }
    } else {
        if (ImGui::Button("Stop (Ctrl+C)") || (main_window.io().KeyCtrl && ImGui::IsKeyPressed('c', false))) {
            /*
            emulator.stop();
            code_model_->update(false);
            scroll_to_pc_ = true;
             */
        }
    }
}

void CodeWindow::draw_code()
{
    if (!code_model_)
        return;
    
    Emulator& emulator = Emulator::get();
    
    // filename
    ImGui::PushID(0);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
    if (emulator.stopped())
        ImGui::Button(code_model_->file_selected() ? code_model_->file_selected()->c_str() : "No file selected");
    else
        ImGui::Button("Code in execution...");
    ImGui::PopStyleColor(3);
    ImGui::PopID();
    
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
        
        if (emulator.stopped()) {
            size_t nline = 1;
            for (auto const& line: code_model_->lines()) {
                ImGui::TableNextRow();
                
                if (line.address.has_value()) {
                    if (*line.address == emulator.pc()) {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, pc_row_color);
                        if (scroll_to_pc_) {
                            ImGui::SetScrollHereY();
                            scroll_to_pc_ = false;
                        }
                    }
                    
                    if (line.is_breakpoint)
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, bkp_cell_color, 0);
                    
                    if (nline == show_this_line_on_next_frame.value_or(-1)) {
                        ImGui::SetScrollHereY();
                        show_this_line_on_next_frame.reset();
                    }
                    
                    ImGui::TableSetColumnIndex(0);
                    char buf[5];
                    sprintf(buf, "%04X", *line.address);
                    if (ImGui::Selectable(buf)) {
                        if (line.is_breakpoint)
                            code_model_->remove_breakpoint(nline);
                        else
                            code_model_->add_breakpoint(nline);
                    }
                }
                
                ImGui::TableSetColumnIndex(1);
                char buf[30] = { 0 };
                int pos = 0;
                for (auto b: line.bytes)
                    pos += sprintf(&buf[pos], "%02X ", b);
                if (!line.bytes.empty())
                    ImGui::Text("%s", buf);
                
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", line.code.c_str());
                
                ++nline;
            }
        }
        
        ImGui::EndTable();
    }
    
}

void CodeWindow::draw_footer()
{
    MainWindow& main_window = MainWindow::get();
    Emulator& emulator = Emulator::get();
    
    if (emulator.stopped()) {
        ImGui::Text("Click on the address to set a breakpoint.");
        
        if (ImGui::Button("Soft Reset")) {
            emulator.soft_reset();
            scroll_to_pc_ = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Recompile project (Ctrl+R)") || (main_window.io().KeyCtrl && ImGui::IsKeyPressed('r', false))) {
            /*
            p().recompile_project();  // TODO ???
            update_symbol_list();
            emulator.reset();
            scroll_to_pc_ = true;
             */
        }
        ImGui::SameLine();
        if (ImGui::Button("Go to file... (F)") || ImGui::IsKeyPressed('f', false))
            on_go_to_file_();
        ImGui::SameLine();
        if (ImGui::Button("Go to symbol... (S)") || ImGui::IsKeyPressed('s', false))
            ; // show_choose_symbol = true;
        ImGui::SameLine();
        if (ImGui::Button("Advanced..."))
            ; // show_advanced_window = true;
    }
}