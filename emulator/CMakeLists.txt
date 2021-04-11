cmake_minimum_required(VERSION 3.17)
project(emulator)

set(CMAKE_CXX_STANDARD 20)

# COMPILER FLAGS

add_subdirectory(contrib/yaml-cpp)
add_subdirectory(contrib/glfw)
include_directories(contrib/yaml-cpp/include contrib/glad/include contrib/glfw/include contrib/imgui contrib/imgui/backends
                    contrib/z80 contrib/imgui-filebrowser)

# EXECUTABLE

set(IMGUI_SRC
    contrib/imgui/imgui.cpp contrib/imgui/imgui_demo.cpp contrib/imgui/imgui_draw.cpp contrib/imgui/imgui_tables.cpp
    contrib/imgui/imgui_widgets.cpp contrib/imgui/backends/imgui_impl_glfw.cpp contrib/imgui/backends/imgui_impl_opengl3.cpp
    contrib/glad/src/gl.c)

set(SOURCES
    ${IMGUI_SRC}
    contrib/z80/Z80.c
    compiler/compiler.hh compiler/compiler.cc compiler/projectfile.cc compiler/projectfile.hh compiler/compilationresult.hh
    compiler/debug.hh compiler/debug.cc ui/window.hh ui/mainwindow.cc ui/mainwindow.hh emulator/emulator.cc
    ui/loadprojectwindow.cc ui/loadprojectwindow.hh manager.cc manager.hh ui/demowindow.cc ui/demowindow.hh
    ui/messagebox.cc ui/messagebox.hh ui/ramwindow.cc ui/ramwindow.hh emulator/emulator.hh ui/menuwindow.cc ui/menuwindow.hh)

add_executable(emulator
               main.cc ${SOURCES})
target_link_libraries(emulator
                      yaml-cpp stdc++fs glfw)

# TESTS

add_executable(emulator-tests
               EXCLUDE_FROM_ALL
               ${SOURCES} test/testmain.cc test/projectfile_test.cc test/compiler_test.cc)
target_link_libraries(emulator-tests
                      yaml-cpp stdc++fs)
add_custom_command(
        TARGET emulator-tests
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/test/sample
                ${CMAKE_CURRENT_BINARY_DIR}/sample)
