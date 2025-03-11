
set(IMGUI_ROOT "${CMAKE_SOURCE_DIR}/dep/imgui")

file(GLOB SOURCES "${IMGUI_ROOT}/*.cpp")
file(GLOB HEADERS "${IMGUI_ROOT}/*.h")

add_library(imgui STATIC
        "${SOURCES}"
        "${HEADERS}"
        "${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_ROOT}/backends/imgui_impl_glfw.h"
        "${IMGUI_ROOT}/backends/imgui_impl_vulkan.cpp"
        "${IMGUI_ROOT}/backends/imgui_impl_vulkan.h"
        "${IMGUI_ROOT}/misc/cpp/imgui_stdlib.cpp"
        "${IMGUI_ROOT}/misc/cpp/imgui_stdlib.h"
)
target_include_directories(imgui PUBLIC "${IMGUI_ROOT}")
