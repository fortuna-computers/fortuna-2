#ifndef EMULATOR_CODEMODEL_HH
#define EMULATOR_CODEMODEL_HH

#include "../../compiler/debug.hh"

enum class Order { Source, Alphabetical };

struct CodeViewLine {
    CodeViewLine(std::string code, std::optional<uint16_t> const& address, bool is_breakpoint, std::vector<uint8_t> bytes)
            : code(std::move(code)), address(address), is_breakpoint(is_breakpoint), bytes(std::move(bytes)) {}
    
    std::string             code;
    std::optional<uint16_t> address;
    bool                    is_breakpoint;
    std::vector<uint8_t>    bytes;
};


class CodeModel {
public:
    explicit CodeModel(Debug const& debug);
    
    std::vector<CodeViewLine> const&  lines() const { return lines_; }
    std::optional<std::string> const& file_selected() const { return file_selected_; }
    
    void update(bool update_file_selected = true);
    
    void add_breakpoint(size_t line);
    void remove_breakpoint(size_t line);
    
    void set_file(std::string const& filename);
    
    Debug const& debug() const { return debug_; }

private:
    Debug                      debug_;
    std::vector<CodeViewLine>  lines_;
    std::optional<std::string> file_selected_;
    
    std::optional<SourceLine> find_pc_location(bool update_file_selected);
    void                      create_lines(std::string const& filename);
};

#endif
