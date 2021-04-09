#ifndef EMULATOR_COMPILATIONRESULT_HH
#define EMULATOR_COMPILATIONRESULT_HH

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "debug.hh"

struct Binary {
    std::vector<uint8_t> data;
    uint16_t             address;
};

struct CompilationResult {
    std::optional<std::string>              error    {};
    std::string                             message  {};
    std::unordered_map<std::string, Binary> binaries {};
    Debug                                   debug    {};
};

struct SourceFile {
    std::string filename;
    uint16_t    expected_address;
    
    SourceFile(std::string const& filename, uint16_t expected_address) : filename(filename), expected_address(expected_address) {}
};

#endif
