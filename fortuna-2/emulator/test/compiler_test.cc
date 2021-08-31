#include "catch.hpp"

#include "../compiler/compiler.hh"

#include <filesystem>
#include <fstream>
#include <iostream>

static CompilationResult compile_rom(std::string const& content, std::string const& content2 = {})
{
    auto temp = std::filesystem::temp_directory_path() / "fortuna-2";
    std::filesystem::remove_all(temp);
    std::filesystem::create_directories(temp);
    
    std::ofstream ofs(temp / "test.yaml");
    ofs << R"(
        sources:
          - source: sample1.z80
            address: 0x0
        rom: sample1.z80
    )";
    ofs.close();
    
    std::ofstream ofs2(temp / "sample1.z80");
    ofs2 << content;
    ofs2.close();
    
    if (!content2.empty()) {
        std::ofstream ofs3(temp / "sample2.z80");
        ofs3 << content2;
        ofs3.close();
    }
    
    Compiler compiler;
    auto result = compiler.compile_from_project_file(temp / "test.yaml");
    std::filesystem::remove_all(temp);
    if (result.error.has_value())
        std::cerr << result.error.value() << "\n";
    return result;
}


TEST_CASE("Simple NOP (sanity check)", "[Compiler]") {
    CompilationResult r = compile_rom(" nop");
    REQUIRE(std::vector<uint8_t>{0} == r.binaries.at("sample1.z80").data);
    REQUIRE((Files{{"sample1.z80", 1}}) == r.debug.files);
    REQUIRE((SourceAddress{" nop", 0, {0}}) == r.debug.source.at("sample1.z80").at(1));
    REQUIRE((SourceLine{"sample1.z80", 1}) == r.debug.location.at(0));
}

TEST_CASE("File with labels", "[Compiler]") {
    CompilationResult r = compile_rom( R"(
    nop
main:
    nop
    jp main
    )");
    REQUIRE((std::vector<uint8_t>{0x0, 0x0, 0xc3, 0x1, 0x0}) == r.binaries.at("sample1.z80").data);
    REQUIRE((Files{{"sample1.z80", 6}}) == r.debug.files);
    REQUIRE((SourceAddress{"main:", {}, {}}) == r.debug.source.at("sample1.z80").at(3));
    REQUIRE((SourceAddress{"    jp main", 2, {0xc3, 0x1, 0x0}}) == r.debug.source.at("sample1.z80").at(5));
    REQUIRE((SourceLine{"sample1.z80", 5}) == r.debug.location.at(2));
    REQUIRE((Symbols{{"main", 1}}) == r.debug.symbols);
}

TEST_CASE("Compilation error", "[Compiler]") {
    CompilationResult r = compile_rom(" invalid");
    REQUIRE(true == r.error.has_value());
    REQUIRE(true == !r.error.value().empty());
}

TEST_CASE("Import files", "[Compiler]") {
    CompilationResult r = compile_rom(R"(
    jp dest
    include sample2.z80
    )", R"(
dest:
    ret
    )");
    REQUIRE((std::vector<uint8_t>{0xc3, 0x3, 0x0, 0xc9}) == r.binaries.at("sample1.z80").data);
    REQUIRE((Files{{"sample1.z80", 4}, {"sample2.z80", 4}}) == r.debug.files);
    REQUIRE((SourceAddress{"dest:", {}, {}}) == r.debug.source.at("sample2.z80").at(2));
    REQUIRE((SourceAddress{"    jp dest", 0, {0xc3, 0x3, 0x0}}) == r.debug.source.at("sample1.z80").at(2));
    REQUIRE((SourceLine{"sample2.z80", 3}) == r.debug.location.at(3));
    REQUIRE((Symbols{{"dest", 3}}) == r.debug.symbols);
}

TEST_CASE("Test full", "[Compiler]") {
    CompilationResult r = Compiler().compile_from_project_file("sample/sample.yaml");
    REQUIRE(std::vector<uint8_t>{0x3e, 0xf0, 0xc3, 0x00, 0x02} == r.binaries.at("boot.z80").data);
    REQUIRE(std::vector<uint8_t>{0xcf, 0x00} == r.binaries.at("os.z80").data);
    REQUIRE(std::vector<uint8_t>{0x05, 0x00} == r.binaries.at("app.z80").data);
    REQUIRE(0x0 == r.binaries.at("boot.z80").address);
    REQUIRE(0x400 == r.binaries.at("os.z80").address);
    REQUIRE(0x2000 == r.binaries.at("app.z80").address);
    REQUIRE((Files{{"boot.z80", 2}, {"boot.z80.bak", 2}, {"os.z80", 2}, {"app.z80", 3}}) == r.debug.files);
    REQUIRE((SourceAddress{"    dec b   ; 05", 0x2000, {0x5}}) == r.debug.source.at("app.z80").at(1));
    REQUIRE((SourceAddress{"    jp  200h     ; C3 00 02", 0x2, {0xc3, 0x0, 0x2}}) == r.debug.source.at("boot.z80").at(2));
    REQUIRE((Symbols{{"hello", 0x2001}}) == r.debug.symbols);
}
