#ifndef EMULATOR_PROJECTFILE_HH
#define EMULATOR_PROJECTFILE_HH

#include <string>
#include <optional>
#include <vector>

struct ProjectFile {
    struct Image {
        enum class Format { Undefined, Fat16 };
        Format      format = Format::Undefined;
    };
    
    struct Source {
        std::string                source;
        uint16_t                   address;
        std::optional<std::string> alias;
        std::optional<std::string> add_to_image;
    };
    
    std::vector<Source> sources;
    std::string         rom;
    Image               image;
    
    static ProjectFile import(std::string const& contents);

private:
    void validate();
    
    static constexpr uint16_t NO_ADDRESS = 0xffff;
};

#endif
