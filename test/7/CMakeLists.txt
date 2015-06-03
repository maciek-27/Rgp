project (test7)

set (CMAKE_C_FLAGS "-ansi -Wall -pedantic -pedantic-errors -std=c99")

include_directories (include)
include_directories (../../include)
link_directories (../../lib)
set(EXECUTABLE_OUTPUT_PATH bin)

#add_executable (test test.c)
#target_link_libraries (test rexio rexiotk)
add_executable (test src/main.c++ src/map.c++ src/statusbar.c++
  src/mapdata.c++ src/playerassets.c++ src/keymap.c++
  src/pathfinder.c++ src/entity.c++ src/entity_types.c++
  src/addentitydialog.c++ src/hintbox.c++ src/canproduce.c++)
target_link_libraries (test rexio rexiotk boost_system)
