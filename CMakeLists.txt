cmake_minimum_required(VERSION 3.20)

# Crosscompiling
include(cmake/crosscompile.cmake)

project(BigBaseV2 CXX ASM_MASM)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

# Fetch modules
message("\nFetching modules")
include(cmake/minhook.cmake)
include(cmake/async-logger.cmake)
include(cmake/json.cmake)
include(cmake/fmt.cmake)

message("\nFetching custom modules")
include(cmake/imgui.cmake)
include(cmake/gtav-classes.cmake)


# BigBaseV2
message(STATUS "BigBaseV2")
file(GLOB_RECURSE SRC_MAIN
    "${SRC_DIR}/**.hpp"
    "${SRC_DIR}/**.h"
    "${SRC_DIR}/**.cpp"
    "${SRC_DIR}/**.cc"
    "${SRC_DIR}/**.cxx"
    "${SRC_DIR}/**.asm"
)
add_library(BigBaseV2 MODULE "${SRC_MAIN}")


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET BigBaseV2 PROPERTY CXX_STANDARD 20)

source_group(TREE ${SRC_DIR} PREFIX "src" FILES ${SRC_MAIN} )

target_include_directories(BigBaseV2 PRIVATE 
    "${SRC_DIR}"
    "${json_SOURCE_DIR}/single_include"
    "${gtav_classes_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}"
    "${minhook_SOURCE_DIR}/src/hde"
)

target_precompile_headers(BigBaseV2 PRIVATE "${SRC_DIR}/common.hpp")
target_link_libraries(BigBaseV2 PRIVATE minhook AsyncLogger dbghelp imgui fmt d3dcompiler dwmapi winmm psapi)

add_compile_definitions(BigBaseV2 
    "_CRT_SECURE_NO_WARNINGS"
    "NOMINMAX"
    "WIN32_LEAN_AND_MEAN"
    "_WIN32_WINNT=0x0600"
)