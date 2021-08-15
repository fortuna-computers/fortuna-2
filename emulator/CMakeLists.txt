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

set(GUI_SRC
    ${IMGUI_SRC}
    ui/window/loadprojectwindow.cc ui/window/loadprojectwindow.hh manager.cc manager.hh ui/window/demowindow.cc ui/window/demowindow.hh
    ui/modal/messagebox.cc ui/modal/messagebox.hh ui/window/ramwindow.cc ui/window/ramwindow.hh ui/window/menuwindow.cc ui/window/menuwindow.hh
    ui/window.hh ui/window/mainwindow.cc ui/window/mainwindow.hh
    ui/window/cpuwindow.cc ui/window/cpuwindow.hh ui/window/terminalwindow.cc ui/window/terminalwindow.hh ui/modal/sendkeypressmodal.cc ui/modal/sendkeypressmodal.hh ui/window/codewindow.cc ui/window/codewindow.hh ui/model/codemodel.cc ui/model/codemodel.hh ui/window/fileselectwindow.cc ui/window/fileselectwindow.hh)

set(SOURCES
    contrib/z80/Z80.c
    compiler/compiler.hh compiler/compiler.cc compiler/projectfile.cc compiler/projectfile.hh compiler/compilationresult.hh
    compiler/debug.hh compiler/debug.cc emulator/emulator.hh emulator/emulator.cc emulator/terminal.cc emulator/terminal.hh emulator/image.hh emulator/image.cc)

add_executable(emulator
               main.cc ${SOURCES} ${GUI_SRC})
target_link_libraries(emulator
                      yaml-cpp stdc++fs glfw)

# TESTS

add_executable(emulator-tests
               EXCLUDE_FROM_ALL
               ${SOURCES} test/testmain.cc test/projectfile_test.cc test/compiler_test.cc test/terminal_test.cc)
target_link_libraries(emulator-tests
                      yaml-cpp stdc++fs)
add_custom_command(
        TARGET emulator-tests
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/test/sample
                ${CMAKE_CURRENT_BINARY_DIR}/sample)
