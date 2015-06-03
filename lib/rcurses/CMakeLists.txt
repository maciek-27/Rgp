project (rcurses)

set (CMAKE_CXX_FLAGS "-ansi -Wall -pedantic -pedantic-errors -fPIC -std=c++98 -Wno-long-long")

include_directories (include)
include_directories (../../include)
include (src/CMakeLists.txt)

add_library (rcurses SHARED ${rcurses_sources})
