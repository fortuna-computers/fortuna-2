#include <optional>
#include "terminalwindow.hh"

#include "../../emulator/emulator.hh"
#include "imgui.h"

void TerminalWindow::draw()
{
    Terminal& terminal = emulator_.terminal();
    
    float cursor_x = 0.0;
    std::optional<float> cursor_y;
    
    ImGui::SetNextWindowSize({ static_cast<float>(terminal.columns() * 7) + 16, static_cast<float>(terminal.lines() * 18) + 32 });
    if (ImGui::Begin("Terminal", &visible_, ImGuiWindowFlags_NoResize)) {
    
        // terminal
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(100, 255, 100)));
        for (size_t i = 0; i < terminal.lines(); ++i) {
            if (i == terminal.cursor_y()) {
                cursor_x = ImGui::GetCursorPosX();
                cursor_y = ImGui::GetCursorPosY();
            }
            ImGui::Text("%s", terminal.text().at(i).c_str());
        }
        ImGui::PopStyleColor();
    
        // buttons
        if (emulator_.stopped()) {
            if ((ImGui::Button("Keypress... (F2)") || ImGui::IsKeyPressed(F2))) {
                send_keypress_modal_.set_visible(true);
            }
        }
    
        // cursor
        if (cursor_y.has_value()) {
            // draw cursor
            ImGui::SetCursorPos({ cursor_x + (terminal.cursor_x() * 7), cursor_y.value() + 2 });
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(230, 60, 60)));
            ImGui::Button(" ", { 7, 11 });
            ImGui::PopStyleColor();
        
            // redraw charcter in black
            ImGui::SetCursorPos({ cursor_x + (terminal.cursor_x() * 7), cursor_y.value() });
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 255-60, 255-60)));
            // ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(0, 0, 0)));
            ImGui::Text("%c", terminal.text().at(terminal.cursor_y()).at(terminal.cursor_x()));
            ImGui::PopStyleColor();
        }
    }
    ImGui::End();
}