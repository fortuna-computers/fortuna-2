#ifndef EMULATOR_PROJECTFILE_HH
#define EMULATOR_PROJECTFILE_HH

#include <string>
#include <optional>

struct ProjectFile {
    enum class SourceType { Undefined, ROM, OS, App };
    
    struct Image {
        enum class Format { Undefined, Fat32 };
        std::string name;
        Format      format = Format::Undefined;
        std::string app_filename;
    };
    
    struct Debug {
        std::string          rom;
        std::string          os;
        std::optional<Image> image;
    };
    
    std::string          source;
    SourceType           source_type = SourceType::Undefined;
    std::optional<Debug> debug;
    
    static ProjectFile import(std::string const& contents);

private:
    void validate();
};

#endif
