#include "catch.hpp"

#include "../compiler/projectfile.hh"

TEST_CASE("full project file") {
    std::string contents = R"(
        source: app.z80
        type: app
        debug:
          rom: boot.z80
          os: os.z80
          image:
            name: debug.img
            format: fat32
            os_filename: KERNEL.BIN
            app_filename: APP.EXE )";
    ProjectFile project_file = ProjectFile::import(contents);
    CHECK(project_file.source == "app.z80");
    CHECK(project_file.source_type == ProjectFile::SourceType::App);
    CHECK(project_file.debug->rom == "boot.z80");
    CHECK(project_file.debug->os == "os.z80");
    CHECK(project_file.debug->image->name == "debug.img");
    CHECK(project_file.debug->image->format == ProjectFile::Image::Format::Fat32);
    CHECK(project_file.debug->image->app_filename == "APP.EXE");
}