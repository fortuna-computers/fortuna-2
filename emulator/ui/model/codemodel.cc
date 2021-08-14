#include "codemodel.hh"
#include "../../emulator/emulator.hh"

CodeModel::CodeModel(Debug const& debug)
    : debug_(debug)
{
    update();
}

void CodeModel::update()
{
    lines_.clear();
    
    std::optional<SourceLine> sl = find_pc_location();
    if (!sl)
        return;
    
    /*
    // retrieve breakpoints - TODO
    std::vector<uint16_t> bkps = z80aw::query_breakpoints();
     */
    
    create_lines(sl->filename);
}

void CodeModel::add_breakpoint(size_t line)
{

}

void CodeModel::remove_breakpoint(size_t line)
{

}

std::optional<SourceLine> CodeModel::find_pc_location()
{
    Emulator& emulator = Emulator::get();
    
    SourceLine sl;
    try {
        sl = debug_.location.at(emulator.pc());
    } catch (std::out_of_range&) {
        file_selected_ = {};
        return {};
    }
    file_selected_ = sl.filename;
    
    return sl;
}

void CodeModel::create_lines(std::string const& filename)
{
    size_t number_of_lines = debug_.files.at(filename);
    SourceAddresses const& source_addresses = debug_.source.at(filename);
    
    for (size_t i = 1; i <= number_of_lines; ++i) {
        SourceAddress const& sa = source_addresses.at(i);
        lines_.emplace_back(sa.source, sa.address, false, sa.bytes);
        // TODO - check if is breakopint
    }
}
