cmake_minimum_required(VERSION 3.17)
project(emulator)

set(CMAKE_CXX_STANDARD 20)

set(OBJECTS compiler/projectfile.cc compiler/projectfile.hh)

add_subdirectory(contrib/yaml-cpp)
include_directories(contrib/yaml-cpp/include)

add_executable(emulator main.cc ${OBJECTS})
target_link_libraries(emulator yaml-cpp)

add_executable(emulator-tests EXCLUDE_FROM_ALL ${OBJECTS} test/testmain.cc test/projectfile_test.cc)
target_link_libraries(emulator-tests yaml-cpp)
