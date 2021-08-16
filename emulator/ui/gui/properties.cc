#include "properties.hh"

#include "imgui.h"
#include "imgui_internal.h"

void Properties::initialize(ImGuiContext* p_context)
{
    ImGuiSettingsHandler ini_handler;
    ini_handler.TypeName = "UserData";
    ini_handler.TypeHash = ImHashStr("UserData");
    ini_handler.UserData = &properties_;
    ini_handler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler* h, const char* name) -> void* {
        if (strcmp(name, "Config") == 0)
            return h->UserData;
        return nullptr;
    };
    ini_handler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line) {
        auto& props = *reinterpret_cast<std::unordered_map<std::string, std::string>*>(entry);
        std::string sline = line;
        if (sline.find('=') != std::string::npos) {
            std::string key = sline.substr(0, sline.find('='));
            std::string value = sline.substr(sline.find('=') + 1);
            props[key] = value;
        }
    };
    ini_handler.WriteAllFn = [](ImGuiContext*, ImGuiSettingsHandler* h, ImGuiTextBuffer* buf) {
        auto& props = *reinterpret_cast<std::unordered_map<std::string, std::string>*>(h->UserData);
        buf->appendf("[UserData][Config]\n");
        for (auto& [k,v] : props)
            buf->appendf((k + "=%s\n").c_str(), v.c_str());
    };
    p_context->SettingsHandlers.push_back(ini_handler);
    
    ImGui::LoadIniSettingsFromDisk("imgui.ini");
}

std::string Properties::property(std::string const& name) const
{
    auto it = properties_.find(name);
    if (it == properties_.end())
        return "";
    return it->second;
}

void Properties::set_property(std::string const& name, std::string const& value)
{
    properties_[name] = value;
}

bool Properties::property_bool(std::string const& name) const
{
    std::string p = property(name);
    if (p == "true")
        return true;
    return false;
}

void Properties::set_property_bool(std::string const& name, bool value)
{
    if (value)
        set_property(name, "true");
    else
        set_property(name, "false");
}