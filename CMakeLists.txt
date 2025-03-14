cmake_minimum_required(VERSION 3.31)
project(leimu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(GLFW)
include(GLM)
include(ImGui)
include(STB)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(LEIMU_DEBUG=1)
endif ()

add_subdirectory(leimu)
add_subdirectory(leimu-gears)
