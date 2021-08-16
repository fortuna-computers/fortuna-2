#ifndef EMULATOR_PROPERTIES_HH
#define EMULATOR_PROPERTIES_HH

#include <string>
#include <unordered_map>

class Properties {
public:
    void        initialize(struct ImGuiContext* context);
    
    std::string property(std::string const& name) const;
    bool        property_bool(std::string const& name) const;
    void        set_property(std::string const& name, std::string const& value);
    void        set_property_bool(std::string const& name, bool value);

private:
    std::unordered_map<std::string, std::string> properties_ {};
};

#endif
