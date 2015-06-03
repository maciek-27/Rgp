project (test3)

set (CMAKE_CXX_FLAGS "-ansi -Wall -pedantic -pedantic-errors -std=c++11 -Wno-long-long")
set (CMAKE_LD_FLAGS "-Wl,-Bstatic")
include_directories (~/sw/rexio/include)
link_directories (~/sw/rexio/lib)
set(EXECUTABLE_OUTPUT_PATH bin)

add_executable (test test.c++)
target_link_libraries (test rexio rt rexiotk)