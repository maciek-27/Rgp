project (libxftp)

set (CMAKE_CXX_FLAGS "-ansi -Wall -fPIC -std=c++98")

link_libraries (-lxerces-c -lboost_filesystem)
link_libraries (-lz)
link_libraries (streamcon)
include_directories(./)
add_library (xftp SHARED
commondomfeatures.c++
message.c++
xmlstream.c++
xftpcommons.c++
zipfilters.c++
xftpserver.c++
xftpclient.c++
clientcontrol.c++
servercontrol.c++
getpass.c
fsutil.c++
client/hello.c++
client/auth.c++
client/cstreamrequest.c++
client/dstreamrequest.c++
client/chdir.c++
client/list.c++
client/seek.c++
client/close.c++
client/write.c++
client/read.c++
client/open.c++
client/data_transfer.c++
client/filename_operations.c++
client/createdirectory.c++
client/remove.c++
server/hello.c++
server/authrefuse.c++
server/serverinfo.c++
server/list.c++
server/acknowledgement.c++
server/error.c++
)
