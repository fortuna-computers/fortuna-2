#ifndef EMULATOR_CODEMODEL_HH
#define EMULATOR_CODEMODEL_HH

#include "../../compiler/debug.hh"
#include "../../emulator/emulator.hh"

enum class Order { Source, Alphabetical };

struct CodeViewLine {
    CodeViewLine(std::string code, std::optional<uint16_t> const& address, std::vector<uint8_t> bytes, std::optional<size_t> comment_start)
            : code(std::move(code)), address(address), bytes(std::move(bytes)), comment_start(comment_start) {}
    
    std::string             code;
    std::optional<uint16_t> address;
    std::vector<uint8_t>    bytes;
    std::optional<size_t>   comment_start;
};


class CodeModel {
public:
    explicit CodeModel(Emulator& emulator, Debug const& debug);
    
    std::vector<CodeViewLine> const&  lines() const { return lines_; }
    std::optional<std::string> const& file_selected() const { return file_selected_; }
    
    void update(bool update_file_selected = true);
    
    void add_breakpoint(size_t line);
    void remove_breakpoint(size_t line);
    
    void set_file(std::string const& filename);
    std::optional<uint16_t> go_to_symbol(std::string const& symbol);
    
    Debug const& debug() const { return debug_; }

private:
    Emulator&                  emulator_;
    Debug                      debug_;
    std::vector<CodeViewLine>  lines_;
    std::optional<std::string> file_selected_;
    
    std::optional<SourceLine> find_pc_location(bool update_file_selected);
    void                      create_lines(std::string const& filename);
    std::optional<size_t>     find_comment_start(std::string const& line) const;
};

#endif
