if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)
project (TP2007)

include_directories (${TP2007_SOURCE_DIR}/include)
set (SCREEN_SOURCE_DIR ${TP2007_SOURCE_DIR}/lib/screen)
set (TOOLKIT_SOURCE_DIR ${TP2007_SOURCE_DIR}/lib/toolkit)


include (FindLATEX)
add_custom_target (install make -f installer.mak install COMMENT Installing)
add_custom_target (uninstall make -f installer.mak uninstall COMMENT Un-Installing)
add_custom_target (doc make -C doc/ COMMENT Building documentation)
add_custom_target (cleandoc make -C doc/ clean COMMENT Cleaning documentation)

add_subdirectory (lib)
add_subdirectory (test)
add_subdirectory (extra/games/MUD)
