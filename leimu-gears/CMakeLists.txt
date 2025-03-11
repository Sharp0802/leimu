
file(GLOB_RECURSE SOURCES lib/*)
file(GLOB_RECURSE HEADERS include/*)

add_executable(leimu-gears WIN32 ${SOURCES} ${HEADERS})
target_include_directories(leimu-gears PUBLIC include)
target_link_libraries(leimu-gears PUBLIC leimu)
