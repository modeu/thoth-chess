cmake_minimum_required(VERSION 3.20)
project(Thoth VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build-Type auf Debug setzen falls nicht angegeben
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(Thoth ${SOURCES})
target_include_directories(Thoth PRIVATE src)

target_compile_options(Thoth PRIVATE
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)