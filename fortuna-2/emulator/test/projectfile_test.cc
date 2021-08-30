#include "catch.hpp"

#include "../compiler/projectfile.hh"

TEST_CASE("Full project file", "[ProjectFile]") {
    std::string contents = R"(
        sources:
          - source: boot.z80
            address: 0x0
          - source: boot.z80
            address: 0x200
            alias: boot.z80.bak
          - source: os.z80
            address: 0x400
            add_to_image: KERNEL.BIN
          - source: app.z80
            address: 0x2000
            add_to_image: APP.EXE

        rom: boot.z80

        image:
          format: fat16
    )";
    
    ProjectFile project_file = ProjectFile::import(contents);
    REQUIRE(project_file.sources.at(0).source == "boot.z80");
    REQUIRE(project_file.sources.at(1).address == 0x200);
    REQUIRE(project_file.sources.at(1).alias.value() == "boot.z80.bak");
    REQUIRE(project_file.sources.at(2).add_to_image.value() == "KERNEL.BIN");
    REQUIRE(project_file.rom == "boot.z80");
    REQUIRE(project_file.image.format == ProjectFile::Image::Format::Fat16);
}
