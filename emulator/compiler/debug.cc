#include "debug.hh"

bool
SourceAddress::operator==(SourceAddress const& rhs) const
{
    return std::tie(source, address, bytes) == std::tie(rhs.source, rhs.address, rhs.bytes);
}

std::ostream& operator<<(std::ostream& os, SourceAddress const& address_)
{
    os << std::hex << std::uppercase << std::setfill('0') << std::setw(2);
    os << "source: \"" << address_.source << "\" address: " << address_.address.value_or(-1) << " bytes: ";
    for (uint8_t b: address_.bytes)
        os << (int) b << ' ';
    return os;
}

bool SourceLine::operator==(SourceLine const& rhs) const {
    return std::tie(filename, line) == std::tie(rhs.filename, rhs.line);
}

std::ostream& operator<<(std::ostream& os, SourceLine const& line_)
{
    os << "filename: " << line_.filename << " line_: " << line_.line;
    return os;
}

std::ostream& operator<<(std::ostream& os, Files const& files)
{
    for (auto const& file: files)
        os << file.first << ": " << file.second << ", ";
    return os;
}

std::ostream& operator<<(std::ostream& os, Symbols const& symbols)
{
    for (auto const& symbol: symbols)
        os << symbol.first << ": " << symbol.second << ", ";
    return os;
}
