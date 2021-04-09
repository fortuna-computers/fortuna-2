#include "catch.hpp"

#include "../compiler/projectfile.hh"

TEST_CASE("Full project file", "[ProjectFile]") {
    std::string contents = R"(
        source: app.z80
        type: app
        debug:
          rom: boot.z80
          os: os.z80
          app_address: 0x2000
          image:
            name: debug.img
            format: fat32
            os_filename: KERNEL.BIN
            app_filename: APP.EXE )";
    ProjectFile project_file = ProjectFile::import(contents);
    REQUIRE(project_file.source == "app.z80");
    REQUIRE(project_file.source_type == ProjectFile::SourceType::App);
    REQUIRE(project_file.debug->rom == "boot.z80");
    REQUIRE(project_file.debug->os == "os.z80");
    REQUIRE(project_file.debug->app_address == 0x2000);
    REQUIRE(project_file.debug->image->name == "debug.img");
    REQUIRE(project_file.debug->image->format == ProjectFile::Image::Format::Fat32);
    REQUIRE(project_file.debug->image->app_filename == "APP.EXE");
}