cmake_minimum_required(VERSION 3.20)
project(Thoth VERSION 0.1 LANGUAGES CXX)

# C++20 setzen
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Alle .cpp Dateien automatisch finden
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(Thoth ${SOURCES})

# Include-Pfad setzen damit #include "Misc.h" funktioniert
target_include_directories(Thoth PRIVATE src)