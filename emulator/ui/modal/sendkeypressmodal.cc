#include <imgui.h>
#include <GLFW/glfw3.h>
#include "sendkeypressmodal.hh"
#include "../../emulator/emulator.hh"
#include "../gui/gui.hh"

void SendKeypressModal::draw()
{
    if (ImGui::BeginPopupModal("Keypress", &visible_, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Press a key to send to the Z80 computer.");
        ImGui::Separator();
        if (ImGui::Button("Cancel"))
            visible_ = false;
        ImGui::EndPopup();
        
        if (do_keypress())
            visible_ = false;
    }
    
    if (visible_)
        ImGui::OpenPopup("Keypress");
}

bool SendKeypressModal::do_keypress()
{
    ImGuiIO const& io = ImGui::GetIO();
    
    bool press = false;
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) {
        if (ImGui::IsKeyPressed(i)) {
            for (uint8_t k: translate_keypress(i, io.KeyCtrl, io.KeyShift, io.KeyAlt, io.KeySuper))
                emulator_.keypress(k);
            press = true;
        }
    }
    return press;
}

std::vector<uint8_t> SendKeypressModal::translate_keypress(int key, bool ctrl, bool shift, bool alt, bool super)
{
    // std::cout << "Keypress: " << key << std::endl;
    
    if (ctrl || alt || super)   // not supported yet
        return {};
    if (key >= 'A' && key <= 'Z') {
        return { shift ? (uint8_t) key : (uint8_t) (key + 32) };
    } else if (key < 127) {
        return { (uint8_t) key };
    } else switch(key) {
            case GLFW_KEY_ENTER: return { '\r', '\n' };
            case GLFW_KEY_BACKSPACE: return { '\b' };
            default: return {};
        }
}