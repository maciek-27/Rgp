project (test4)
include (FindThreads)

set (CMAKE_CXX_FLAGS "-ansi -Wall -pedantic -pedantic-errors -std=c++98 -Wno-long-long")

include_directories (include)
include_directories (../../include)
link_directories (../../lib)
set(EXECUTABLE_OUTPUT_PATH bin)

add_executable (test src/main.c++ src/demo.c++ src/manager.c++ src/netconn.c++)
target_link_libraries (test rexio rexiotk ${CMAKE_THREAD_LIBS_INIT})