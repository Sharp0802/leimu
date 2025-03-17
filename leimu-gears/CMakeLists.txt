
file(GLOB_RECURSE SOURCES lib/*)
file(GLOB_RECURSE HEADERS include/*)

file(GLOB SHADERS RELATIVE "${CMAKE_CURRENT_LIST_DIR}" data/* )

add_executable(leimu-gears WIN32 ${SOURCES} ${HEADERS} ${SHADERS})
target_include_directories(leimu-gears PUBLIC include)
target_link_libraries(leimu-gears PUBLIC leimu)

foreach(SHADER IN LISTS SHADERS)
    get_filename_component(SHADER_DIR "${SHADER}" DIRECTORY)
    add_custom_command(
            TARGET leimu-gears PRE_BUILD
            COMMAND mkdir -p "${SHADER_DIR}"
            COMMAND glslc "${CMAKE_CURRENT_LIST_DIR}/${SHADER}"
            -o "${CMAKE_CURRENT_BINARY_DIR}/${SHADER}.spv"
    )
endforeach()
