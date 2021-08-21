#ifndef EMULATOR_IMAGEFILE_HH
#define EMULATOR_IMAGEFILE_HH

#include <fstream>
#include "../compiler/compilationresult.hh"

class ImageFile {
public:
    ImageFile(CompilationResult const& result, bool use_in_emulator, std::string const& path="");
    ~ImageFile();
    
    ImageFile(ImageFile const&)            = delete;
    ImageFile(ImageFile&&)                 = delete;
    ImageFile& operator=(ImageFile const&) = delete;
    ImageFile& operator=(ImageFile&&)      = delete;
    
    void write_block_to_image(uint32_t block, uint8_t const* data);
    void read_block_from_image(uint32_t block, uint8_t* data);

private:
    void add_bootsector(Binary const& binary);
    void add_file(std::string const& filename, Binary const& binary);
    
    size_t        file_size_ = 16 * 1024 * 1024;
    std::string   filename_;
    std::fstream  file_;
    bool          use_in_emulator_;
    
    static constexpr std::ios_base::openmode file_flags_ = std::ios::out | std::ios::in | std::ios::binary;
};

#endif
