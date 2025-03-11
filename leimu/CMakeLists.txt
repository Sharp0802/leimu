
file(GLOB_RECURSE SOURCES lib/*)
file(GLOB_RECURSE HEADERS include/*)

add_library(leimu SHARED ${SOURCES} ${HEADERS})
target_include_directories(leimu PUBLIC include)
target_link_libraries(leimu PUBLIC
        glfw
        glm::glm
        imgui
        vulkan
)
