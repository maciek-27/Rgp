project (screen)


set (CMAKE_CXX_FLAGS "-ansi -Wall -pedantic -pedantic-errors -fPIC -std=c++98 -Wno-long-long -rdynamic")
add_definitions(-DDO_VALIDATE_UTF_8_OUTPUT)

include_directories (include)
include_directories (../../include/rexio)
include (src/terminfo/CMakeLists.txt)
include (src/core/CMakeLists.txt)
include (src/real/CMakeLists.txt)
include (src/subscreen/CMakeLists.txt)
link_libraries (-lboost_thread  -lpthread -ldl) 

add_library (rexio SHARED ${terminfo_sources} ${core_sources} ${real_sources} ${subscreen_sources} )
