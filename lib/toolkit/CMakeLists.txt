project (toolkit)

set (CMAKE_CXX_FLAGS "-ansi -Wall -pedantic -pedantic-errors -fPIC -std=c++98 -Wno-long-long")

include_directories (include)
include_directories (../../include/)
include_directories (../../include/rexio)
include_directories (../../include/rexio/tk)
include (src/CMakeLists.txt)

add_library (rexiotk SHARED ${toolkit_sources})
