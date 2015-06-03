project (libstreamcon)

set (CMAKE_CXX_FLAGS "-ansi -Wall -pedantic -pedantic-errors -fPIC -std=c++98")

IF(DEFINED Boost_LIBRARIES)
    link_libraries (${Boost_LIBRARIES} -lpthread -lsctp)
ELSE(DEFINED Boost_LIBRARIES)
    link_libraries (-lboost_thread -lpthread -lsctp)
ENDIF(DEFINED Boost_LIBRARIES)

IF(DEFINED OPENSSL_FOUND)
    link_libraries (${OPENSSL_LIBRARIES})
ENDIF(DEFINED OPENSSL_FOUND)

add_library (streamcon STATIC 
commons.c++
databuffer.c++
threads.c++
stream.c++
connection.c++
sctpconnection.c++
server.c++
sctpserver.c++
iofilters/tlsfilter.c++
iofilters/countfilter.c++
)

