project (client)

set (CMAKE_CXX_FLAGS "-ansi -Wall -std=c++98")

add_executable (client 
client.c++ 
superdata.c++ 
supercontrol_put.c++
supercontrol_get.c++
supercontrol_list.c++
supercontrol_llist.c++
supercontrol_chdir.c++
supercontrol_lchdir.c++
supercontrol_run.c++
supercontrol_help.c++
supercontrol_describecommand.c++
supercontrol_assuredt.c++
supercontrol_printnode.c++
supercontrol_remove.c++
supercontrol_lremove.c++
supercontrol_mkdir.c++
supercontrol_lmkdir.c++
supercontrol_exit.c++
supercontrol_generateprompt.c++
printxml.c++
completions.c++
)
target_link_libraries (client streamcon xftp readline)
