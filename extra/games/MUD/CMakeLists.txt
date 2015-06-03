project (test4)
include (FindThreads)

set (CMAKE_CXX_FLAGS "-ansi -Wall -pedantic -pedantic-errors -std=c++98 -Wno-long-long")

include_directories (include)
include_directories (../../../include)
link_directories (../../../lib/screen ../../../lib/toolkit ../../../lib/net ../../../lib)
set(EXECUTABLE_OUTPUT_PATH bin)

add_executable (test src/main.c++ src/gamewindow.c++
src/dungeonview.c++ src/character.c++ src/u8conv.c++
src/textbox.c++ src/dungeon.c++ src/evil_creatures.c++)
link_libraries (-lboost_thread  -lpthread -ldl) 

target_link_libraries (test rexio rexiotk ${CMAKE_THREAD_LIBS_INIT} rexionet boost_system boost_thread)
