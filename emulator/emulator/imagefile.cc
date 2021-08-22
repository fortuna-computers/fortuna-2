#include <unistd.h>
#include "imagefile.hh"

using namespace std::string_literals;

static constexpr const char *EMULATOR_IMAGE_FILENAME = "/tmp/miniz80.img",
                            *TMP_FILENAME = "/tmp/miniz80.tmp",
                            *TMP_FILE_OUTPUT = "/tmp/miniz80.txt";

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

ImageFile::ImageFile(CompilationResult const& result, bool use_in_emulator, std::string const& path)
    : filename_(use_in_emulator ? EMULATOR_IMAGE_FILENAME : path + "/" + (*result.project_file.debug->image).name),
      file_(filename_, file_flags_ | std::ios::trunc),
      use_in_emulator_(use_in_emulator)
{
    if (!file_.is_open())
        throw std::runtime_error("Unable to create image file " + filename_);
    
    if (result.project_file.debug->image->format != ProjectFile::Image::Format::Fat16)
        throw std::runtime_error("Sorry, only FAT16 images are supported right now.");
    
    // add boot
    add_bootsector(result.binaries.at(result.project_file.debug->rom));
    
    // add OS kernel
    if (result.project_file.source_type == ProjectFile::SourceType::OS || result.project_file.source_type == ProjectFile::SourceType::App)
        add_file("KERNEL.BIN", result.binaries.at(result.project_file.debug->os));
    
    // add application file
    if (result.project_file.source_type == ProjectFile::SourceType::App)
        add_file(result.project_file.debug->image->app_filename, result.binaries.at(result.project_file.source));
    
    file_.flush();
}

ImageFile::~ImageFile()
{
    file_.close();
    if (use_in_emulator_)
        unlink(filename_.c_str());
}

void ImageFile::add_bootsector(Binary const& binary)
{
    if (binary.data.size() > 450)
        throw std::runtime_error("The boot file is too large.");
    
    file_.write((char*) bootsector.data(), (long) bootsector.size());
    file_.write((char*) binary.data.data(), (long) binary.data.size());
    
    uint8_t bootsector_signature[] = { 0x55, 0xaa };
    file_.seekp(510);
    file_.write((char*) bootsector_signature, 2);
    
    uint8_t fat_signature[] = { 0xf8, 0xff, 0xff, 0xff };
    file_.seekp(0x800);
    file_.write((char*) fat_signature, 4);
    file_.seekp(0x4800);
    file_.write((char*) fat_signature, 4);
    
    file_.seekp((long) file_size_ - 1);
    file_.put(0x0);
}

void ImageFile::add_file(std::string const& filename, Binary const& binary)
{
    std::string tmp_filename;
    
    std::ofstream tmp(TMP_FILENAME, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!tmp.is_open())
        throw std::runtime_error("Could not create file "s + TMP_FILENAME + " for transferring to image.");
    tmp.write((char*) binary.data.data(), (long) binary.data.size());
    tmp.close();
    
    file_.close();
    
    char buf[4096];
    snprintf(buf, sizeof buf, "mcopy -i %s %s ::%s > %s 2> %s",
             filename_.c_str(), TMP_FILENAME, filename.c_str(), TMP_FILE_OUTPUT, TMP_FILE_OUTPUT);
    printf("%s\n", buf);
    if (std::system(buf) != 0) {
        std::string err(std::istreambuf_iterator<char>(std::ifstream(TMP_FILE_OUTPUT).rdbuf()), std::istreambuf_iterator<char>());
        throw std::runtime_error("Could not copy file " + filename + " to image: " + err);
    }
    
    file_.open(filename_, file_flags_);
    
    unlink(TMP_FILE_OUTPUT);
    unlink(TMP_FILENAME);
}

void ImageFile::write_block_to_image(uint32_t block, uint8_t const* data)
{
    if (block < file_size_ / 512) {
        file_.seekp(block * 512);
        file_.write((const char*) data, 512);
        file_.flush();
    }
}

void ImageFile::read_block_from_image(uint32_t block, uint8_t* data)
{
    if (block < file_size_ / 512) {
        file_.seekp(block * 512);
        file_.read((char*) data, 512);
    }
}

void ImageFile::read_block(uint32_t block, uint8_t data[512]) const
{
    if (block < file_size_ / 512) {
        file_.seekp(block * 512);
        file_.read((char*) data, 512);
    }
}

