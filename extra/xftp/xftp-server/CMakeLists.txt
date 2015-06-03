project (server)

set (CMAKE_CXX_FLAGS "-ansi -Wall -pedantic -pedantic-errors -std=c++98")

add_executable (server server.c++)
target_link_libraries (server streamcon xftp)
