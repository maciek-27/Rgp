cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
 
project (xftp)

set(CMAKE_MODULE_PATH CMakeModules)
find_package(Boost COMPONENTS thread)
find_package(Xerces)
find_package(OpenSSL)

include_directories (./)
include_directories (./libxftp)

add_custom_target (doc make -C doc/ COMMENT Building documentation)
add_custom_target (cleandoc make -C doc/ clean COMMENT Cleaning documentation)

add_subdirectory (libstreamcon)
add_subdirectory (libxftp)
add_subdirectory (tests)
add_subdirectory (xftp-client)
add_subdirectory (xftp-server)
