cmake_minimum_required(VERSION 3.17)
project(emulator)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-DIMGUI_IMPL_OPENGL_LOADER_GLAD2)

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
    ui/window.hh ui/gui/gui.cc ui/gui/gui.hh ui/window/cpuwindow.cc ui/window/cpuwindow.hh ui/window/terminalwindow.cc ui/window/terminalwindow.hh
    ui/modal/sendkeypressmodal.cc ui/modal/sendkeypressmodal.hh ui/window/codewindow.cc ui/window/codewindow.hh ui/model/codemodel.cc
    ui/model/codemodel.hh ui/gui/properties.cc ui/gui/properties.hh
    ui/window/symbolselectwindow.cc ui/window/symbolselectwindow.hh ui/window/storagewindow.cc ui/window/storagewindow.hh)

set(SOURCES
    contrib/z80/Z80.c
    compiler/compiler.hh compiler/compiler.cc compiler/projectfile.cc compiler/projectfile.hh compiler/compilationresult.hh
    compiler/debug.hh compiler/debug.cc emulator/emulator.hh emulator/emulator.cc emulator/terminal.cc emulator/terminal.hh
    emulator/imagefile.cc emulator/imagefile.hh)

add_executable(fortuna-2
               main.cc ${SOURCES} ${GUI_SRC})
target_link_libraries(fortuna-2
                      yaml-cpp stdc++fs glfw)

add_executable(fortuna-2_createimage cmd/createimage.cc ${SOURCES})
target_link_libraries(fortuna-2_createimage yaml-cpp stdc++fs)

# TESTS

add_executable(fortuna-2_tests
               EXCLUDE_FROM_ALL
               ${SOURCES} test/testmain.cc test/projectfile_test.cc test/compiler_test.cc test/terminal_test.cc)
target_link_libraries(fortuna-2_tests
                      yaml-cpp stdc++fs)
add_custom_command(
        TARGET fortuna-2_tests
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/test/sample
                ${CMAKE_CURRENT_BINARY_DIR}/sample)
