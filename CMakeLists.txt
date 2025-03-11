cmake_minimum_required(VERSION 3.31)
project(leimu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(GLFW)
include(GLM)
include(ImGui)
include(STB)

add_subdirectory(leimu)
add_subdirectory(leimu-gears)
