cmake_minimum_required(VERSION 3.23)

set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")

project(presetter)

set(C74_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals")

add_subdirectory(source/presetter)
