#include "cpuwindow.hh"
#include "../../emulator/emulator.hh"
#include "imgui.h"

void CpuWindow::draw()
{
    Z80 const& z80 = emulator_.z80();
    
    ImGui::SetNextWindowSize({ 267, 400 });
    if (ImGui::Begin("CPU", &visible_, ImGuiWindowFlags_NoResize)) {
        ImGui::Text("Registers:");
        
        auto pair = [](size_t& i, size_t sz, std::string const& name, size_t value) {
            ImGui::TableSetColumnIndex(i++);
            ImGui::Text("%s:", name.c_str());
            ImGui::TableSetColumnIndex(i++);
            char buf[9];
            if (sz == 2)
                sprintf(buf, "%02X", (uint8_t) value);
            else if (sz == 4)
                sprintf(buf, "%04X", (uint16_t) value);
            else if (sz == 8)
                sprintf(buf, "%08X", (uint32_t) value);
            ImGui::Button(buf);
        };
        
        static int tbl_flags = ImGuiTableFlags_NoBordersInBody;
        if (ImGui::BeginTable("##cpu", 8, tbl_flags)) {
            for (int i = 0; i < 8; ++i)
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
            
            ImGui::TableNextRow();
            size_t i = 0;
            pair(i, 4, "AF", z80.AF.W );
            pair(i, 2, "A", z80.AF.W >> 8);
            pair(i, 2, "F", z80.AF.W & 0xff);
            pair(i, 4, "AF'", z80.AF1.W );
            ImGui::TableNextRow();
            i = 0;
            pair(i, 4, "BC", z80.BC.W );
            pair(i, 2, "B", z80.BC.W >> 8);
            pair(i, 2, "C", z80.BC.W & 0xff);
            pair(i, 4, "BC'", z80.BC1.W );
            ImGui::TableNextRow();
            i = 0;
            pair(i, 4, "DE", z80.DE.W );
            pair(i, 2, "D", z80.DE.W >> 8);
            pair(i, 2, "E", z80.DE.W & 0xff);
            pair(i, 4, "DE'", z80.DE1.W );
            ImGui::TableNextRow();
            i = 0;
            pair(i, 4, "HL", z80.HL.W );
            pair(i, 2, "H", z80.HL.W >> 8);
            pair(i, 2, "L", z80.HL.W & 0xff);
            pair(i, 4, "HL'", z80.HL1.W );
            ImGui::TableNextRow();
            i = 0;
            pair(i, 4, "IX", z80.IX.W );
            pair(i, 2, "I", z80.I );
            pair(i, 2, "R", z80.R );
            pair(i, 4, "IY", z80.IY.W );
            ImGui::TableNextRow();
            i = 0;
            pair(i, 4, "PC", z80.PC.W);
            i = 6;
            pair(i, 4, "SP", z80.SP.W );
            ImGui::EndTable();
        }
        bool v = z80.IFF & IFF_HALT;
        ImGui::Checkbox("HALT", &v);
        
        uint8_t f = z80.AF.W & 0xff;
        ImGui::Separator();
        ImGui::Text("Flags:");
        bool s = (f >> 7) & 1; ImGui::Checkbox("Sign", &s);
        bool z = (f >> 6) & 1; ImGui::Checkbox("Zero", &z);
        bool h = (f >> 4) & 1; ImGui::Checkbox("Half carry", &h);
        bool pv = (f >> 2) & 1; ImGui::Checkbox("Parity / Overflow (V)", &pv);
        bool n = (f >> 1) & 1; ImGui::Checkbox("Subtract (N)", &n);
        bool c = (f >> 0) & 1; ImGui::Checkbox("Carry", &c);
        
        ImGui::Separator();
        ImGui::Text("SD Card register: ");
        ImGui::SameLine();
        char buf[16];
        sprintf(buf, "0x%02X%02X%02X%02X",
                emulator_.sdcard_register[3], emulator_.sdcard_register[2], emulator_.sdcard_register[1], emulator_.sdcard_register[0]);
        ImGui::Button(buf);
    }
    ImGui::End();

}