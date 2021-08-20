#ifndef EMULATOR_IMAGE_HH
#define EMULATOR_IMAGE_HH

#include <string>

#include "../compiler/projectfile.hh"

std::optional<std::string> image_file_create(ProjectFile const& file, std::string const& sources_path, bool use_in_emulator);

#endif //EMULATOR_IMAGE_HH
