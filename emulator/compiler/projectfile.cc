#include "projectfile.hh"

#include <yaml-cpp/yaml.h>

ProjectFile ProjectFile::import(std::string const& contents)
{
    ProjectFile p;
    YAML::Node node = YAML::Load(contents);
    if (node["source"])
        p.source = node["source"].as<std::string>();
    if (node["type"]) {
        auto type = node["type"].as<std::string>();
        if (type == "rom")
            p.source_type = SourceType::ROM;
        else if (type == "os")
            p.source_type = SourceType::OS;
        else if (type == "app")
            p.source_type = SourceType::App;
        else
            throw std::runtime_error("Invalid application type '" + type + "'");
    }
    
    if (node["debug"]) {
        p.debug = Debug();
        YAML::Node debug = node["debug"];
        if (debug["rom"])
            p.debug->rom = debug["rom"].as<std::string>();
        if (debug["os"])
            p.debug->os = debug["os"].as<std::string>();
        if (debug["app_address"])
            p.debug->app_address = debug["app_address"].as<uint16_t>();
        
        if (debug["image"]) {
            p.debug->image = Image();
            YAML::Node image = debug["image"];
            if (image["name"])
                p.debug->image->name = image["name"].as<std::string>();
            if (image["app_filename"])
                p.debug->image->app_filename = image["app_filename"].as<std::string>();
            if (image["format"]) {
                auto format = image["format"].as<std::string>();
                if (format == "fat32")
                    p.debug->image->format = Image::Format::Fat32;
                else
                    throw std::runtime_error("Invalid format '" + format + "'");
            }
        }
    }
    
    p.validate();
    
    return p;
}

void ProjectFile::validate()
{
    if (source.empty())
        throw std::runtime_error("The property 'source' is required.");
    if (source_type == SourceType::Undefined)
        throw std::runtime_error("The property 'type' is required.");
    if (debug.has_value()) {
        if (source_type == SourceType::App || source_type == SourceType::OS) {
            if (debug->rom.empty())
                throw std::runtime_error("The property 'debug.rom' is required for this type of source if debug is enabled.");
        }
        if (source_type == SourceType::App) {
            if (debug->os.empty())
                throw std::runtime_error("The property 'debug.os' is required for this type of source if debug is enabled.");
            if (debug->app_address == NO_ADDRESS)
                throw std::runtime_error("The property 'debug.app_address' is required for this type of source if debug is enabled.");
        }
        if (debug->image.has_value()) {
            if (debug->image->name.empty())
                throw std::runtime_error("The property 'debug.image.name' is required if an image is to be built.");
            if (debug->image->format == Image::Format::Undefined)
                throw std::runtime_error("The property 'debug.image.format' is required if an image is to be built.");
            if (source_type == SourceType::App && debug->image->app_filename.empty())
                throw std::runtime_error("The property 'debug.image.app_filename' is required if an image is to be built.");
        }
    }
}
