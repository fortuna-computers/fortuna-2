#include <unistd.h>
#include "imagefile.hh"

using namespace std::string_literals;

static constexpr const char *EMULATOR_IMAGE_FILENAME = "/tmp/miniz80.img",
                            *TMP_FILENAME = "/tmp/miniz80.tmp",
                            *TMP_FILE_OUTPUT = "/tmp/miniz80.txt";

ImageFile::ImageFile(size_t size_in_mb, CompilationResult const& result, bool use_in_emulator, std::string const& path)
    : filename_(use_in_emulator ? EMULATOR_IMAGE_FILENAME : path),
      use_in_emulator_(use_in_emulator)
{
    if (result.project_file.image.format != ProjectFile::Image::Format::Fat16)
        throw std::runtime_error("Sorry, only FAT32 images are supported right now.");
    
    if (result.project_file.image.format != ProjectFile::Image::Format::Fat16 && size_in_mb < 64)
        throw std::runtime_error("The minimum size for FAT32 is 64 Mb");
    
    // add boot
    create_image(size_in_mb);
    
    // add files
    for (auto const& source: result.project_file.sources) {
        if (source.add_to_image)
            add_file(*source.add_to_image, result.binaries.at(source.alias.value_or(source.source)));
    }
    
    file_.open(filename_, file_flags_);
    if (!file_.is_open())
        throw std::runtime_error("Unable to open created file image " + filename_);
}

ImageFile::~ImageFile()
{
    file_.close();
    if (use_in_emulator_)
        unlink(filename_.c_str());
}

void ImageFile::create_image(size_t size_in_mb)
{
    unlink(filename_.c_str());
    
    char buf[4096];
    snprintf(buf, sizeof buf, "mkfs.vfat -C -F16 -n MINIZ80 %s %zu",
             filename_.c_str(), size_in_mb * 1024);
    if (std::system(buf) != 0) {
        std::string err(std::istreambuf_iterator<char>(std::ifstream(TMP_FILE_OUTPUT).rdbuf()), std::istreambuf_iterator<char>());
        throw std::runtime_error("Could not create file image in " + filename_ + ": " + err);
    }
}

void ImageFile::add_file(std::string const& filename, Binary const& binary)
{
    std::string tmp_filename;
    
    std::ofstream tmp(TMP_FILENAME, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!tmp.is_open())
        throw std::runtime_error("Could not create file "s + TMP_FILENAME + " for transferring to image.");
    tmp.write((char*) binary.data.data(), (long) binary.data.size());
    tmp.close();
    
    char buf[4096];
    snprintf(buf, sizeof buf, "mcopy -i %s %s ::%s > %s 2> %s",
             filename_.c_str(), TMP_FILENAME, filename.c_str(), TMP_FILE_OUTPUT, TMP_FILE_OUTPUT);
    if (std::system(buf) != 0) {
        std::string err(std::istreambuf_iterator<char>(std::ifstream(TMP_FILE_OUTPUT).rdbuf()), std::istreambuf_iterator<char>());
        throw std::runtime_error("Could not copy file " + filename + " to image: " + err);
    }
    
    unlink(TMP_FILE_OUTPUT);
    unlink(TMP_FILENAME);
}

void ImageFile::write_block_to_image(uint32_t block, uint8_t const* data)
{
    if (fail_requests)
        return;
    
    if (block < file_size_ / 512) {
        file_.seekp(block * 512);
        file_.write((const char*) data, 512);
        file_.flush();
    }
}

void ImageFile::read_block_from_image(uint32_t block, uint8_t* data)
{
    if (fail_requests)
        return;
    
    if (block < file_size_ / 512) {
        file_.seekp(block * 512);
        file_.read((char*) data, 512);
    }
}

void ImageFile::read_block(uint32_t block, uint8_t data[512]) const
{
    if (fail_requests)
        return;
    
    if (block < file_size_ / 512) {
        file_.seekp(block * 512);
        file_.read((char*) data, 512);
    }
}

