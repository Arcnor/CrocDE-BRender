cmake_minimum_required(VERSION 3.12)
project(BRenderSample)

# Prefer glvnd
cmake_policy(SET CMP0072 NEW)

find_package(Perl REQUIRED)

option(BRENDER_BUILD_DRIVERS  "Build Drivers"  ON)

add_subdirectory(brender)

option(BRENDER_BUILD_TOOLS    "Build Tools"    ON)
option(BRENDER_BUILD_EXAMPLES "Build Examples" ON)

if(BRENDER_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(BRENDER_BUILD_EXAMPLES AND BRENDER_BUILD_DRIVERS)
    add_subdirectory(examples)
endif()
