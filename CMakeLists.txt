cmake_minimum_required(VERSION 3.12.0)
set(VTEFFECT_PROJECT_NAME "VT Effect" CACHE STRING "Project name for IDEs")
project("${VTEFFECT_PROJECT_NAME}" C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(misc_target)
include(set_cxx_standard)
include(target_folder)
include(vs_startup_project)

set_cxx_standard(17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(stdout_control)
add_subdirectory(scat)
add_subdirectory(vtasm)
add_subdirectory(vtdump)
add_subdirectory(vteffect)
vs_startup_project(vteffect)

misc_target(FILES
    ".editorconfig"
    ".clang-format"
    ".github/workflows/cmake.yml"
    "cmake/misc_target.cmake"
    "cmake/set_cxx_standard.cmake"
    "cmake/target_folder.cmake"
    "cmake/vs_startup_project.cmake"
    "CMakePresets.json"
    "readme.md"
    "LICENSE")
misc_group_sources("CMake" FILES "CMakePresets.json")
misc_group_sources("CMake Scripts" FILES
    "cmake/misc_target.cmake"
    "cmake/set_cxx_standard.cmake"
    "cmake/target_folder.cmake"
    "cmake/vs_startup_project.cmake")
misc_group_sources("Documentation" FILES "LICENSE" "readme.md")
misc_group_sources("Editing" FILES ".editorconfig" ".clang-format")
misc_group_sources("GitHub" FILES ".github/workflows/cmake.yml")
