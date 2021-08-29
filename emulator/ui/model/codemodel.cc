#include "codemodel.hh"
#include "../../emulator/emulator.hh"

CodeModel::CodeModel(Emulator& emulator, Debug const& debug)
    : emulator_(emulator), debug_(debug)
{
    update();
}

void CodeModel::update(bool update_file_selected)
{
    lines_.clear();
    
    std::optional<SourceLine> sl = find_pc_location(update_file_selected);
    if (!sl)
        return;
    
    create_lines(sl->filename);
}

void CodeModel::add_breakpoint(size_t line)
{
    if (!file_selected_.has_value())
        return;
    auto it = debug_.reverse_location.find({ *file_selected_, line });
    if (it != debug_.reverse_location.end())
        emulator_.add_breakpoint(it->second);
    update(false);
}

void CodeModel::remove_breakpoint(size_t line)
{
    if (!file_selected_.has_value())
        return;
    auto it = debug_.reverse_location.find({ *file_selected_, line });
    if (it != debug_.reverse_location.end())
        emulator_.remove_breakpoint(it->second);
    update(false);
}

std::optional<SourceLine> CodeModel::find_pc_location(bool update_file_selected)
{
    SourceLine sl;
    if (update_file_selected) {
        try {
            sl = debug_.location.at(emulator_.pc());
        } catch (std::out_of_range&) {
            file_selected_ = {};
            return {};
        }
        file_selected_ = sl.filename;
    } else {  // this is called when the user selects a file
        if (!file_selected_)
            return {};
        sl = SourceLine { *file_selected_, 1 };
    }
    
    return sl;
}

void CodeModel::create_lines(std::string const& filename)
{
    size_t number_of_lines = debug_.files.at(filename);
    SourceAddresses const& source_addresses = debug_.source.at(filename);
    
    for (size_t i = 1; i <= number_of_lines; ++i) {
        SourceAddress const& sa = source_addresses.at(i);
        lines_.emplace_back(sa.source, sa.address, sa.bytes, find_comment_start(sa.source));
    }
}

void CodeModel::set_file(std::string const& filename)
{
    file_selected_ = filename;
    update(false);
}

std::optional<uint16_t> CodeModel::go_to_symbol(std::string const& symbol)
{
    auto it = debug_.symbols.find(symbol);
    if (it == debug_.symbols.end())
        return {};
    
    auto it_loc = debug_.location.find(it->second);
    if (it_loc == debug_.location.end())
        return {};
    
    SourceLine const& sl = it_loc->second;
    set_file(sl.filename);
    return sl.line;
}

std::optional<size_t> CodeModel::find_comment_start(std::string const& line) const
{
    // TODO - is naive
    int f = line.find(';');
    if (f == std::string::npos)
        return {};
    else
        return f;
}
