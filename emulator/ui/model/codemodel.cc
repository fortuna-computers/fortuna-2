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
    update();
}

void CodeModel::remove_breakpoint(size_t line)
{
    if (!file_selected_.has_value())
        return;
    auto it = debug_.reverse_location.find({ *file_selected_, line });
    if (it != debug_.reverse_location.end())
        emulator_.remove_breakpoint(it->second);
    update();
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
        lines_.emplace_back(sa.source, sa.address, emulator_.is_breakpoint(*sa.address), sa.bytes);
    }
}

void CodeModel::set_file(std::string const& filename)
{
    file_selected_ = filename;
    update(false);
}
