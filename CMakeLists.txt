cmake_minimum_required(VERSION 3.13)
project(BRenderSample)

# Prefer glvnd
cmake_policy(SET CMP0072 NEW)

find_package(Perl REQUIRED)

add_compile_definitions(BGFX_CONFIG_RENDERER_OPENGL=41)
add_compile_definitions(BGFX_CONFIG_RENDERER_DIRECT3D9=1)
add_compile_definitions(BGFX_CONFIG_RENDERER_DIRECT3D11=1)
add_compile_definitions(BGFX_CONFIG_RENDERER_DIRECT3D12=1)
add_compile_definitions(BGFX_CONFIG_RENDERER_METAL=1)
add_compile_definitions(BGFX_CONFIG_RENDERER_VULKAN=1)
add_subdirectory(libsrc/bgfx.cmake EXCLUDE_FROM_ALL)

if (WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

option(BRENDER_DISABLE_FINDSDL "Disable searching for SDL2" OFF)

if(NOT BRENDER_DISABLE_FINDSDL)
    # glrend needs this
    find_package(SDL2 REQUIRED)
endif()

add_subdirectory(brender)

option(BRENDER_BUILD_TOOLS    "Build Tools"    ON)
option(BRENDER_BUILD_EXAMPLES "Build Examples" ON)

if(BRENDER_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(BRENDER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
