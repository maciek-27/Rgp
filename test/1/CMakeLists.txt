project (test1)

set (CMAKE_CXX_FLAGS "-ansi -Wall -pedantic -pedantic-errors -std=c++98 -Wno-long-long")

include_directories (../../include)
link_directories (../../lib)
set(EXECUTABLE_OUTPUT_PATH bin)

add_executable (test src/main.c++)
target_link_libraries (test rexio)