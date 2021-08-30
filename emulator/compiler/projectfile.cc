#include "projectfile.hh"

#include <yaml-cpp/yaml.h>

ProjectFile ProjectFile::import(std::string const& contents)
{
    ProjectFile p;
    YAML::Node node = YAML::Load(contents);
    
    for (const auto &item : node["sources"]) {
        uint16_t address = 0;
        if (item["address"])
            address = item["address"].as<uint16_t>();
        Source source = {
            item["source"].as<std::string>(),
            address
        };
        if (item["alias"])
            source.alias = item["alias"].as<std::string>();
        if (item["add_to_image"])
            source.add_to_image = item["add_to_image"].as<std::string>();
        p.sources.emplace_back(std::move(source));
    }
    
    p.rom = node["rom"].as<std::string>();
    
    if (node["image"]) {
        auto image_format = node["image"]["format"].as<std::string>();
        if (image_format == "fat16")
            p.image.format = Image::Format::Fat16;
        else
            p.image.format = Image::Format::Undefined;
    }
    
    p.validate();
    
    return p;
}

void ProjectFile::validate()
{
    if (sources.empty())
        throw std::runtime_error("At least one source is required.");
    
    auto it = std::find_if(sources.begin(), sources.end(), [this](Source const& s) { return s.source == rom; });
    if (it == sources.end())
        throw std::runtime_error("'rom' value doesn't refer to one of the sources.");
    
    std::map<std::string, bool> duplicates;
    for (auto const& source: sources) {
        std::string name = source.alias.value_or(source.source);
        auto it2 = duplicates.find(name);
        if (it2 != duplicates.end())
            throw std::runtime_error("Filename '" + name + "' is repeated on the sources.");
    }
}
