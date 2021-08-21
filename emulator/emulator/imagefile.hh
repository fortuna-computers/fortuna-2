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

private:
    void add_bootsector(Binary const& binary);
    void add_file(std::string const& filename, Binary const& binary);
    
    std::string   filename_;
    std::ofstream file_;
    bool          use_in_emulator_;
};

#endif
