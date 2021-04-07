#ifndef EMULATOR_PROJECTFILE_HH
#define EMULATOR_PROJECTFILE_HH

#include <string>

struct ProjectFile {
    enum class SourceType { Boot, OS, App };
    
    struct Image {
        enum class Format { Fat32 };
        std::string name;
        Format      format;
        std::string os_filename;
        std::string app_filename;
    };
    
    struct Debug {
        std::string rom;
        std::string os;
        Image       image;
    };
    
    static ProjectFile import(std::string const& contents);
    
    std::string source;
    SourceType  source_type;
    Debug       debug;
};

#endif
