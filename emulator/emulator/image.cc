#include <fstream>
#include <vector>
#include "image.hh"

static constexpr const char* EMULATOR_IMAGE_FILENAME = "/tmp/miniz80.img";

static const std::vector<uint8_t> bootsector {
        0xeb, 0x3c, 0x90,                          // 0x00 - jump instruction (TODO ?)
        'M', 'I', 'N', 'I', 'Z', '8', '0', 0x00,   // 0x03 - OEM name
        0x00, 0x02,                                // 0x0B - bytes per logical sector
        0x04,                                      // 0x0D - logical sectors per cluster
        0x04, 0x00,                                // 0x0E - logical sectors per cluster
        0x02,                                      // 0x10 - number of FATs
        0x00, 0x02,                                // 0x11 - maximum number of FAT16 root directory entries
        0x00, 0x80,                                // 0x13 - total of logical sectors
        0xf8,                                      // 0x15 - media descriptor (0xf8 = hard disk)
        0x20, 0x00,                                // 0x16 - logical sectors per FAT
        0x20, 0x00,                                // 0x18 - physical sectors per track
        0x40, 0x00,                                // 0x1A - number of heads
        0x00, 0x00, 0x00, 0x00,                    // 0x1C - hidden sectors
        0x00, 0x00, 0x00, 0x00,                    // 0x20 - total logical sectors
        0x80,                                      // 0x24 - physical drive number
        0x00,                                      // 0x25 - reserved
        0x29,                                      // 0x26 - extended boot signature
        0xfe, 0x1f, 0x62, 0x1e,                    // 0x27 - volume ID
        'M', 'I', 'N', 'I', 'Z', '8', '0', ' ', ' ', ' ', ' ',  // 0x2B - partition volume label
        'F', 'A', 'T', '1', '6', ' ', ' ', ' ',    // 0x36 - file system type
};

static void add_bootsector_to_image(std::ofstream& ofstream, std::string const& bootstrap_filename);
static void add_file_to_image(std::ofstream& ofstream, std::string const& origin_filename, std::string const& destination_filename);

std::optional<std::string> image_file_create(ProjectFile const& project_file, std::string const& sources_path, bool use_in_emulator)
{
    if (!project_file.debug || !project_file.debug->image)   // an image or debug section is not set on .yaml file
        return {};
    
    if (project_file.debug->image->format != ProjectFile::Image::Format::Fat16)
        throw std::runtime_error("Sorry, only FAT16 images are supported right now.");
    
    std::string filename = use_in_emulator ? EMULATOR_IMAGE_FILENAME : sources_path + "/" + (*project_file.debug->image).name;
    
    std::ofstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc | std::ios::ate);
    
    add_bootsector_to_image(file, sources_path + "/" + project_file.debug->rom);
    if (project_file.source_type == ProjectFile::SourceType::OS || project_file.source_type == ProjectFile::SourceType::App)
        add_file_to_image(file, sources_path + "/" + project_file.debug->os, "KERNEL.BIN");
    if (project_file.source_type == ProjectFile::SourceType::App)
        add_file_to_image(file, sources_path + "/" + project_file.debug->image->app_filename, project_file.debug->image->app_filename);
        
    file.close();
    
    return filename;
}

void add_bootsector_to_image(std::ofstream& ofstream, std::string const& bootstrap_filename)
{
    std::ifstream bootstrap_file(bootstrap_filename, std::ios::binary);
    if (!bootstrap_file.is_open())
        throw std::runtime_error("Could not open file " + bootstrap_filename + ".");
    std::vector<uint8_t> bootstrap((std::istreambuf_iterator<char>(bootstrap_file)), std::istreambuf_iterator<char>());
    bootstrap_file.close();
    if (bootstrap.size() > 450)
        throw std::runtime_error("The boot file is too large.");
    
    ofstream.write((char*) bootsector.data(), bootsector.size());
    ofstream.write((char*) bootstrap.data(), bootstrap.size());
    
    uint8_t bootsector_signature[] = { 0x55, 0xaa };
    ofstream.seekp(510);
    ofstream.write((char*) bootsector_signature, 2);
    
    uint8_t fat_signature[] = { 0xf8, 0xff, 0xff, 0xff };
    ofstream.seekp(0x800);
    ofstream.write((char*) fat_signature, 4);
    ofstream.seekp(0x4800);
    ofstream.write((char*) fat_signature, 4);
}

void
add_file_to_image(std::ofstream& ofstream, std::string const& origin_filename, std::string const& destination_filename)
{

}

