cmake_minimum_required(VERSION 3.17)
project(emulator)

set(CMAKE_CXX_STANDARD 20)

# COMPILER FLAGS

add_subdirectory(contrib/yaml-cpp)
include_directories(contrib/yaml-cpp/include)

# EXECUTABLE

set(OBJECTS
    compiler/compiler.hh compiler/compiler.cc compiler/projectfile.cc compiler/projectfile.hh compiler/compilationresult.hh
    compiler/debug.hh compiler/debug.cc)

add_executable(emulator
               main.cc ${OBJECTS})
target_link_libraries(emulator
                      yaml-cpp stdc++fs)

# TESTS

add_executable(emulator-tests
               EXCLUDE_FROM_ALL
               ${OBJECTS} test/testmain.cc test/projectfile_test.cc test/compiler_test.cc)
target_link_libraries(emulator-tests
                      yaml-cpp stdc++fs)
add_custom_command(
        TARGET emulator-tests
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/test/sample
                ${CMAKE_CURRENT_BINARY_DIR}/sample)
