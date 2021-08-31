#include <climits>
#include "storagewindow.hh"

#include "imgui.h"

void StorageWindow::draw()
{
    float h = 647;
    ImGui::SetNextWindowSize(ImVec2(590, h));
    if (ImGui::Begin("Storage", &visible_, ImGuiWindowFlags_NoResize)) {
        uint32_t block = block_number_;
        
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Block: (PgUp)");
        ImGui::SameLine();
        
        if (ImGui::Button("<") || (!ImGui::GetIO().KeyAlt && ImGui::IsKeyPressed(PageUp)))
            go_to_block_number(block_number_ - 1);
        ImGui::SameLine();
        
        char buf[9];
        snprintf(buf, sizeof buf, "%08X", block);
        ImGui::PushItemWidth(64.0);
        ImGui::InputText("##block", buf, sizeof buf, ImGuiInputTextFlags_CallbackEdit,
                         [](ImGuiInputTextCallbackData* data) {
                             auto* m = reinterpret_cast<StorageWindow*>(data->UserData);
                             unsigned long new_block = strtoul(data->Buf, nullptr, 16);
                             if (new_block != ULONG_MAX)
                                 m->go_to_block_number(new_block);
                             return 0;
                         }, this);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button(">") || (!ImGui::GetIO().KeyAlt && ImGui::IsKeyPressed(PageDown)))
            go_to_block_number(block_number_ + 1);
        ImGui::SameLine();
        ImGui::Text("(PgDown)");
        ImGui::SameLine();
        if (ImGui::Button("Reload (Ctrl+L)") || (!ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed('L')))
            load_data();
        
        draw_storage_table();
    
        ImGui::Checkbox("Fail requests", &image_file_->fail_requests);
    }
    ImGui::End();
}

void StorageWindow::draw_storage_table() const
{
    static int tbl_flags = ImGuiTableFlags_BordersOuter
                           | ImGuiTableFlags_NoBordersInBody
                           | ImGuiTableFlags_RowBg
                           | ImGuiTableFlags_ScrollY;
    static ImU32 pc_bg_color = ImGui::GetColorU32(ImVec4(0.2f, 0.6f, 0.2f, 0.65f));
    static ImU32 sp_bg_color = ImGui::GetColorU32(ImVec4(0.6f, 0.2f, 0.2f, 0.65f));
    
    ImVec2 size = ImVec2(-FLT_MIN, 566);
    if (ImGui::BeginTable("##storage", 18, tbl_flags, size)) {
        
        // headers
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed);
        for (int i = 0; i < 0x10; ++i) {
            char buf[3];
            sprintf(buf, "_%X", i);
            ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_WidthFixed);
        }
        ImGui::TableSetupColumn("ASCII", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        
        for (int line = 0; line < 0x20; ++line) {
            ImGui::TableNextRow();
            
            // address
            uint16_t addr = ((block_number_ * 2) << 8) + (line * 0x10);
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%04X:%04X : ", addr >> 8, (uint16_t) addr);
            
            // data
            std::string ascii;
            for (int i = 0; i < 0x10; ++i) {
                ImGui::TableSetColumnIndex(i + 1);
                int address = line * 0x10 + i;
                uint8_t byte = data_[address];
                bool needs_pop = false;
                if (byte == 0) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(ImColor(128, 128, 128)));
                    needs_pop = true;
                }
                ImGui::Text("%02X", byte);
                if (needs_pop)
                    ImGui::PopStyleColor();
                ascii += (byte >= 32 && byte < 127) ? (char) byte : '.';
            }
            
            // ascii
            ImGui::TableSetColumnIndex(17);
            ImGui::Text("%s", ascii.c_str());
        }
        
        ImGui::EndTable();
    }
    
}

void StorageWindow::go_to_block_number(ssize_t block)
{
    block_number_ = block & (image_file_->disk_size() / 512 - 1);
    load_data();
}

void StorageWindow::load_data()
{
    if (image_file_)
        image_file_->read_block(block_number_, data_);
}

void StorageWindow::set_image_file(ImageFile& image_file) {
    block_number_ = 0;
    image_file_ = &image_file;
    load_data();
}
