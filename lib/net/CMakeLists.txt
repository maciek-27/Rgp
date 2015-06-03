project (rexionet)


set (CMAKE_CXX_FLAGS "-ansi -Wall -pedantic -pedantic-errors -fPIC -std=c++98 -Wno-long-long -rdynamic")
add_definitions(-DDO_VALIDATE_UTF_8_OUTPUT)

#link_libraries (-lboost_thread-mt  -lpthread -ldl) 
add_library (rexionet SHARED netconn.c++)

