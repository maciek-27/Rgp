#include <iostream>
#include <exception>
#include <signal.h>
#include "netconn.h++"
#include "demo.h++"
#include "main.h++"
#include "manager.h++"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <set>
#include <cstdlib>
#include <cstring>

pthread_mutex_t startSequenceMutex;
pthread_mutex_t allprogsStackMutex;

using namespace std;

// for set<ProgEntry> underlaying tree. 
bool operator<(const ProgEntry& a, const ProgEntry& b) {
    // comparing addresses of a and b would be wrong, as they may
    // differ while referencing the same Demo class object.
    return a.d < b.d;
}

set<ProgEntry> allprogs;
Server s;

static std::pair<int, char **> args;

void starter(std::istream & in, std::ostream & out)
{
    Demo prog(in,out);
    pthread_mutex_lock(&allprogsStackMutex);// prevent accidental stack
    allprogs.insert(ProgEntry(&prog));// data structure destruction
    pthread_mutex_unlock(&allprogsStackMutex);
    cerr << "Trying to initialize connection" << endl;
    try
    {
		int i = prog.Start(args.first, args.second); // start
		cerr << "Connection finished with code " << i <<endl;// result
		// on success
    }
    catch (exception)// exception caught. try to recover by ignoring it.
    {
		cerr << "Connection finished with exception, but app is fine."
			 <<endl;
    }
    pthread_mutex_lock(&allprogsStackMutex);
    cerr << "Requesting erase of 1 app out of "<<allprogs.size()<<endl;;
	
    // if ProgEntry exists (not deleted by localInterface func)
    if (allprogs.find(ProgEntry(&prog)) != allprogs.end())
		allprogs.erase(ProgEntry(&prog));//erase it.
    cerr << endl;
    pthread_mutex_unlock(&allprogsStackMutex);
    return;
}

Manager manager;

void * localInterface(void * arg)
{
	std::pair<int, char **>& args = *
		reinterpret_cast<std::pair<int, char **> *>(arg);

    manager.Start(args.first, args.second);
// as login as manager is running, everything is
	// fine. Start shutdown procedure when it stops. 
    

    s.Stop();
    cerr << "Server stopped correctly. ";
    pthread_mutex_lock(&allprogsStackMutex);
    cerr << "Requesting " << allprogs.size()<<" client apps to end."<<endl;
    allprogs.clear();//stop all instances of program  (ProgEntry
		     //destructor stops associated app)
    cerr << endl;
    pthread_mutex_unlock(&allprogsStackMutex);
    return 0;
}

//used in netconn and other
void err(const char * s)
{	
    manager.Exit(0);
    sleep(3);// make sure, it'll be displayed afted last message
    cerr << "\nFatal error occured:" <<s << endl;
    exit(1);
}

typedef void (*pfv)();
int port = 5000;
int main (int argc,char ** argv)
{
	args.first = argc;
	args.second = argv;
	
	if(argc > 1)
    {
		stringstream ss;
		ss<<argv[1];
		
		for(int i = 0;i<argc;i++) {
			if(strncmp(argv[i], "-port=", 6) == 0) {
				std::string str(argv[i] + 6);
				std::stringstream ss(str);
				ss >> port;
			}
		}
    }

    pthread_t ctl_local;
    pthread_mutex_init(&startSequenceMutex,NULL);
    pthread_mutex_lock(&startSequenceMutex); // unlocked after
					     // clearing screen by
					     // Manager object
    pthread_create(&ctl_local, NULL, localInterface, new std::pair<int, char**>(argc, argv));
    pthread_mutex_lock(&startSequenceMutex);
    pthread_mutex_unlock(&startSequenceMutex);
    pthread_mutex_destroy(&startSequenceMutex);

    
    pthread_mutex_init(&allprogsStackMutex,NULL);
	
    cerr << "opening port " << port << endl;
    
    signal(SIGPIPE, SIG_IGN);	//disable signal (app has other ways
	//of detecting connection errors)
    s.Start(port,starter);
    pthread_join(ctl_local,NULL);
    cerr << "Game over" << endl;
    pthread_mutex_destroy(&allprogsStackMutex);
    return 0;
}
/*end of main function of program*/

