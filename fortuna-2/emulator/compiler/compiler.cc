#include "compiler.hh"
#include "projectfile.hh"

#include <filesystem>
#include <fstream>
#include <regex>
#include <climits>

namespace fs = std::filesystem;

CompilationResult Compiler::compile_from_project_file(std::string const& project_filename)
{
    ProjectFile project_file = load_project_file(project_filename);
    
    CompilationResult result;
    result.project_file = project_file;
    for (auto const& source: project_file.sources) {
        auto sources_path = fs::path(project_filename).parent_path();
        try {
            result.message += execute_compiler(sources_path, source.source);
        } catch (std::runtime_error& e) {
            result.error = e.what();
            break;
        }
        auto alias = source.alias.value_or(source.source);
        load_binary(sources_path, alias, source, result);
        auto filenames = find_filenames(sources_path, result, source);
        load_listing(sources_path, source, filenames, result);
        cleanup(sources_path);
    }
    return result;
}

ProjectFile Compiler::load_project_file(std::string const& filename)
{
    std::ifstream t(filename);
    std::string contents((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();
    
    return ProjectFile::import(contents);
}

std::string Compiler::compiler_full_path()
{
    return "/usr/local/bin/vasmz80_oldstyle";   // TODO
}

std::string Compiler::execute_compiler(std::string const& path, std::string const& filename)
{
    char buffer[1024];
    
    std::string commandline = "cd " + path + " && " + compiler_full_path() + " -chklabels -L listing.txt -Llo -nosym -x -Fbin -o rom.bin " + filename + " 2>&1";
    
    FILE* pipe = popen(commandline.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("Could not open executable " + compiler_full_path() + "!");
    
    std::string result;
    while (fgets(buffer, sizeof buffer, pipe) != nullptr)
        result += buffer;
    if (pclose(pipe) != 0)
        throw std::runtime_error(result);
    return result;
}

void Compiler::load_binary(std::string const& path, std::string const& file, ProjectFile::Source const& source, CompilationResult& result)
{
    std::ifstream instream(path + "/rom.bin", std::ios::in | std::ios::binary);
    if (instream.fail())
        throw std::runtime_error("File rom.bin could not be open.");
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
    
    Binary& binary = result.binaries[file] = {};
    if (binary.data.size() < data.size())
        binary.data.resize(data.size());
    for (size_t i = 0; i < data.size(); ++i)
        binary.data[i] = data[i];
    binary.address = source.address;
}

Compiler::Filenames Compiler::find_filenames(std::string const& path, CompilationResult& result, ProjectFile::Source const& source)
{
    std::ifstream f(path + "/listing.txt");
    if (f.fail())
        throw std::runtime_error("File listing.txt does not exist or could not be opened.\n");
    
    enum Section { Sources, Other };
    Section section = Other;
    
    std::string line;
    std::unordered_map<size_t, std::string> ret;
    while (std::getline(f, line)) {
        if (section == Other) {
            if (line == "Sources:")
                section = Sources;
        } else {
            if (line[0] == 'F') {
                std::string file_number_s = line.substr(1, 2);
                size_t file_number = strtoul(file_number_s.c_str(), nullptr, 10);
                std::string filename = line.substr(5);
                if (filename == source.source && source.alias)  // replace file per alias
                    filename = *source.alias;
                ret[file_number] = filename;
                result.debug.files[filename] = 0;
            } else {
                section = Other;
            }
        }
    }
    
    return ret;
}

void Compiler::load_listing(std::string const& path, ProjectFile::Source const& source, Filenames const& filenames, CompilationResult& result)
{
    std::ifstream f(path + "/listing.txt");
    if (f.fail())
        throw std::runtime_error("File listing.txt does not exist or could not be opened.\n");
    
    enum Section { Source, Other, Symbols };
    
    std::string line;
    size_t file_number, file_line;
    Section section = Source;
    while (std::getline(f, line)) {
        if (line.empty())
            continue;
        if (line == "Sections:" || line == "Sources" )
            section = Other;
        else if (line == "Symbols:")
            section = Symbols;
        else if (section == Source && line[0] == 'F')    // regular sourceline line
            read_regular_line(filenames, line, result, file_number, file_line);
        else if (section == Source && line[0] == ' ')  // address
            file_line = read_address(source, filenames, line, result, file_number, file_line);
        else if (section == Symbols) {
            static std::regex regex1(R"(^([^\s][A-z0-9_]+)\s*EXPR\s*\([0-9]+=((0x)?[0-9A-Fa-f]+)\).*ABS.*)");
            static std::regex regex2(R"(^([^\s][A-z0-9_]+)\s+LAB\s*\(((0x)?[0-9A-Fa-f]+)\).*)");
            std::smatch m;
            if (std::regex_match(line, m, regex1) || std::regex_match(line, m, regex2)) {
                std::string symbol_name = m[1];
                unsigned long addr = strtoul(std::string(m[2]).c_str(), nullptr, 16);
                result.debug.symbols[symbol_name] = addr + source.address;
            }
            
        }
    }
}

void Compiler::read_regular_line(Filenames const& filenames, std::string const& line,
                              CompilationResult& result, size_t& file_number, size_t& file_line)
{
    // read line
    std::string file_number_s = line.substr(1, 2);
    std::string file_line_s = line.substr(4, 4);
    std::string sourceline = line.substr(15);
    file_number = strtoul(file_number_s.c_str(), nullptr, 10);
    file_line = strtoul(file_line_s.c_str(), nullptr, 10);
    if (file_number == ULONG_MAX || file_line == ULONG_MAX)
        throw std::runtime_error("Invalid listing file format.");
    std::string filename;
    try {
        filename = filenames.at(file_number);
    } catch (std::out_of_range&) {
        throw std::runtime_error("Error in listing.txt: a sourceline line refers to a file number " + std::to_string(file_number) + ", which doesn't exist.");
    }
    
    // store line in `debug.source[filename]`
    result.debug.files.at(filename) = std::max(result.debug.files.at(filename), file_line);
    if (result.debug.source.find(filename) == result.debug.source.end())
        result.debug.source.insert({ filename, SourceAddresses {} });
    result.debug.source.at(filename).insert({ file_line, { sourceline } });
}

size_t Compiler::read_address(ProjectFile::Source const& source, Filenames const& filenames,
                           std::string const& line, CompilationResult& result, size_t file_number, size_t file_line)
{
    // read line
    std::string addr_s = line.substr(23, 4);
    unsigned long addr = strtoul(addr_s.c_str(), nullptr, 16);
    if (addr == ULONG_MAX)
        throw std::runtime_error("Invalid listing file format.");
    addr += source.address;
    
    // store in source and location
    std::string const& filename = filenames.at(file_number);
    result.debug.source.at(filename)[file_line].address = addr;
    result.debug.location.insert({ addr, { filename, file_line } });
    result.debug.reverse_location.insert({ { filename, file_line }, addr });
    
    // add bytes
    size_t pos = 30;
    std::vector<uint8_t>& bytes = result.debug.source.at(filename)[file_line].bytes;
    while (line.size() >= pos + 2) {
        if (!isxdigit(line[pos]))
            break;
        unsigned long byte = strtoul(line.substr(pos, 2).c_str(), nullptr, 16);
        if (byte == ULONG_MAX)
            break;
        bytes.push_back(byte);
        pos += 3;
    }
    return file_line;
}

void Compiler::cleanup(std::filesystem::path const& path)
{
    fs::remove(path / "listing.txt");
    fs::remove(path / "rom.bin");
}

