#ifndef EMULATOR_COMPILER_HH
#define EMULATOR_COMPILER_HH

#include <string>
#include <filesystem>
#include "compilationresult.hh"
#include "projectfile.hh"

class Compiler {
public:
    CompilationResult compile_from_project_file(std::string const& project_filename);

private:
    struct ProjectPath {
        std::string path;
        std::string basename;
    };
    using Filenames = std::unordered_map<size_t, std::string>;

    static ProjectFile             load_project_file(std::string const& filename);
    static std::string             compiler_full_path();
    static std::string             execute_compiler(std::string const& path, std::string const& filename);
    static void                    load_binary(std::string const& path, std::string const& file, ProjectFile::Source const& source, CompilationResult& result);
    static Filenames               find_filenames(std::string const& path, CompilationResult& result, ProjectFile::Source const& source);
    static void                    load_listing(std::string const& path, ProjectFile::Source const& source, Filenames const& filenames, CompilationResult& result);
    static void                    read_regular_line(Filenames const& filenames, std::string const& line, CompilationResult& result, size_t& file_number, size_t& file_line);
    static size_t                  read_address(ProjectFile::Source const& source, Filenames const& filenames, std::string const& line, CompilationResult& result, size_t file_number, size_t file_line);
    static void                    cleanup(std::filesystem::path const& path);
    
    static constexpr uint16_t ROM_LOCATION = 0x0,
                              OS_LOCATION = 0x200;
};


#endif
