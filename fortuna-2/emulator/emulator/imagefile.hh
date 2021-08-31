#ifndef EMULATOR_IMAGEFILE_HH
#define EMULATOR_IMAGEFILE_HH

#include <fstream>
#include "../compiler/compilationresult.hh"

class ImageFile {
public:
    ImageFile(size_t size_in_mb, CompilationResult const& result, bool use_in_emulator, std::string const& path="");
    ~ImageFile();
    
    ImageFile(ImageFile const&)            = delete;
    ImageFile(ImageFile&&)                 = delete;
    ImageFile& operator=(ImageFile const&) = delete;
    ImageFile& operator=(ImageFile&&)      = delete;
    
    void write_block_to_image(uint32_t block, uint8_t const* data);
    void read_block_from_image(uint32_t block, uint8_t* data);
    
    void read_block(uint32_t block, uint8_t data[512]) const;
    
    size_t disk_size() const { return file_size_; }
    
    bool fail_requests = false;

private:
    void create_image(size_t i);
    void add_file(std::string const& filename, Binary const& binary);
    
    size_t                file_size_ = 16 * 1024 * 1024;
    std::string           filename_;
    mutable std::fstream  file_;
    bool                  use_in_emulator_;
    
    static constexpr std::ios_base::openmode file_flags_ = std::ios::out | std::ios::in | std::ios::binary;
};

#endif
