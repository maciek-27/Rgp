#include<iostream>
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
#include<set>
#include<cstdlib>

pthread_mutex_t startSequenceMutex;
pthread_mutex_t allprogsStackMutex;

using namespace std;
class ProgEntry
{
private:
    Demo* d; 
public:
    ProgEntry(Demo * _d):d(_d){;}
    ~ProgEntry()
	{
	    d->Exit(4);
	    cout << "-";
	}
    friend bool operator<(const ProgEntry& a, const ProgEntry& b);
    
};

// for set<ProgEntry> underlaying tree. 
bool operator<(const ProgEntry& a, const ProgEntry& b) {
    // comparing addresses of a and b would be wrong, as they may
    // differ while referencing the same Demo class object.
    return a.d < b.d;
}

set<ProgEntry> allprogs;
Server s;

void starter(std::istream & in, std::ostream & out)
{
    Demo prog(in,out);
    pthread_mutex_lock(&allprogsStackMutex);// prevent accidental stack
    allprogs.insert(ProgEntry(&prog));// data structure destruction
    pthread_mutex_unlock(&allprogsStackMutex);
    cout << "Trying to initialize connection" << endl;
    try
    {
	int i = prog.Start(); // start
	cout << "Connection finished with code " << i <<endl;// result
							     // on success
    }
    catch (exception)// exception caught. try to recover by ignoring it.
    {
	cout << "Connection finished with exception, but app is fine." <<endl;
    }
    pthread_mutex_lock(&allprogsStackMutex);
    cout << "Requesting erase of 1 app out of "<<allprogs.size()<<endl;;

    // if ProgEntry exists (not deleted by localInterface func)
    if (allprogs.find(ProgEntry(&prog)) != allprogs.end())
	allprogs.erase(ProgEntry(&prog));//erase it.
    cout << endl;
    pthread_mutex_unlock(&allprogsStackMutex);
    return;
}

Manager manager;
void * localInterface(void * arg)
{
    manager.Start(); // as logn as manager is running, everything is
		     // fine. Start shutdown procedure when it stops. 
    
    s.Stop();
    cout << "Server stopped correctly. ";
    pthread_mutex_lock(&allprogsStackMutex);
    cout << "Requesting " << allprogs.size()<<" client apps to end."<<endl;
    allprogs.clear();//stop all instances of program  (ProgEntry
		     //destructor stops associated app)
    cout << endl;
    pthread_mutex_unlock(&allprogsStackMutex);
    return 0;
}

//used in netconn and other
void err(const char * s)
{	
    manager.Exit(0);
    sleep(3);// make sure, it'll be displayed afted last message
    cout << "\nFatal error occured:" <<s << endl;
    exit(1);
}

typedef void (*pfv)();
int main (int argc,char ** argv)
{
    int port;
    pthread_t ctl_local;
    pthread_mutex_init(&startSequenceMutex,NULL);
    pthread_mutex_lock(&startSequenceMutex); // unlocked after
					     // clearing screen by
					     // Manager object
    pthread_create(&ctl_local, NULL, localInterface, NULL);
    pthread_mutex_lock(&startSequenceMutex);
    pthread_mutex_unlock(&startSequenceMutex);
    pthread_mutex_destroy(&startSequenceMutex);

    
    pthread_mutex_init(&allprogsStackMutex,NULL);
    if (argc == 1)
	port = 5000;
    else
    {
	stringstream ss;
	ss<<argv[1];
	
	if (ss.peek()>='0' && ss.peek()<='9')
	    ss>>port;
	else
	    err("portnumber as argument");
    }

    cout << "opening port " << port << endl;
    
    signal(SIGPIPE, SIG_IGN);	//disable signal (app has other ways
				//of detecting connection errors)
    s.Start(port,starter);
    pthread_join(ctl_local,NULL);
    cout << "Game over" << endl;
    pthread_mutex_destroy(&allprogsStackMutex);
    return 0;
}
/*end of main function of program*/

