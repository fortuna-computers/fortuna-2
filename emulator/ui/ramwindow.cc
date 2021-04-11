#include "ramwindow.hh"

#include <cstdio>
#include "imgui.h"
#include "../emulator/emulator.hh"
#include "mainwindow.hh"

void RamWindow::draw()
{
    Emulator& emulator = Emulator::get();
    MainWindow main_window = MainWindow::get();
    
    float h = 370;
    ImGui::SetNextWindowSize(ImVec2(560, h));
    if (ImGui::Begin("Memory", nullptr, ImGuiWindowFlags_NoResize)) {
        uint8_t page = page_number_;
        
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Page: (PgUp)");
        ImGui::SameLine();
        
        if (ImGui::Button("<") || (!main_window.io().KeyCtrl && ImGui::IsKeyPressed(PageUp)))
            go_to_page_number(page_number_ - 1);
        ImGui::SameLine();
        
        char buf[3];
        snprintf(buf, 3, "%02X", page);
        ImGui::PushItemWidth(24.0);
        ImGui::InputText("##page", buf, sizeof buf, ImGuiInputTextFlags_CallbackEdit,
                         [](ImGuiInputTextCallbackData* data) {
                            /*
                             auto* m = reinterpret_cast<MemoryView*>(data->UserData);
                             unsigned long new_page = strtoul(data->Buf, nullptr, 16);
                             if (new_page != ULONG_MAX)
                                 m->go_to_page(new_page);
                             */
                             return 0;
                         }, nullptr /* &m */);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button(">") || (!main_window.io().KeyCtrl && ImGui::IsKeyPressed(PageDown)))
            go_to_page_number(page_number_ + 1);
        ImGui::SameLine();
        ImGui::Text("(PgDown)");
        
        draw_memory_table();
        
        char stack[512] = "Stack: ";
        draw_stack();
        ImGui::Text("%s", stack);
    }
    ImGui::End();

}

void RamWindow::draw_memory_table()
{

}

void RamWindow::draw_stack()
{
    /*
    int n = strlen(stack);
    for (uint8_t byte: m.stack())
        n += sprintf(&stack[n], "%02X ", byte);
    */
}

void RamWindow::go_to_page_number(int page)
{
    page_number_ = page & (Emulator::ram_size() - 1);
}

