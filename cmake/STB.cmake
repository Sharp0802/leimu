
set(STB_PATH "${CMAKE_SOURCE_DIR}/dep/stb")

file(GLOB HEADERS "${STB_PATH}/*.h")

add_library(STB INTERFACE "${HEADERS}")
target_include_directories(STB INTERFACE "${STB_PATH}")
