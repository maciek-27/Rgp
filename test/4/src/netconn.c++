#include<iostream>
#include<pthread.h>
#include <fcntl.h>
#include "main.h++"
#include "netconn.h++"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<ext/stdio_filebuf.h>
#include <unistd.h> /* sleep*/
#include<stack>
#include<cstring>
using namespace std;

#include<ext/stdio_filebuf.h>

/*Class for internal use representing reprezenting initialization
 and termination of connection*/
class __Connection 
{
private:
	int fd;
	pthread_t th;
	
	FILE * oF;
	__gnu_cxx::stdio_filebuf<char> * obuf;
	ostream * ostr;

	FILE * iF;
	__gnu_cxx::stdio_filebuf<char> * ibuf;
	istream * istr;
public:
	__Connection(int _fd);
	~__Connection();	friend void * ServeConnnection(void * conn);
}; 
/* connection */

/* callback function serving connection*/
ConnectionFunc f;

/*for joining "dead" threads*/
stack<pthread_t> CleanerStack;
pthread_mutex_t CleanerStackMutex;

void * ServeConnnection(void * _conn)
{
	__Connection * conn = (__Connection *) _conn;
	cout << "in thread for conn fd: "<< conn->fd << endl;

	f(*(conn->istr),*(conn->ostr));
	delete conn;
	return 0;
}

__Connection::__Connection(int _fd)
{
	fd=_fd;
	
	cout << "Accepted connection; fd = " << fd << endl;
	iF = fdopen(fd,"r");
	oF = fdopen(fd,"w");
	ibuf = new __gnu_cxx::stdio_filebuf<char>(iF,std::ios_base::in,1);
	obuf = new __gnu_cxx::stdio_filebuf<char>(oF,std::ios_base::out,1);
	istr = new std::istream(ibuf);
	ostr = new std::ostream(obuf);

	pthread_create(&th, NULL, ServeConnnection, this);
}

__Connection::~__Connection()
{
	delete istr;
	delete ibuf;
	fclose(iF);

	delete ostr;
	delete obuf;
	fclose(oF);
	close(fd);
	pthread_mutex_lock(&CleanerStackMutex);
	CleanerStack.push(th);
	pthread_mutex_unlock(&CleanerStackMutex);
	
}	

void * CleanerFunc(void * activity_mark)
{

    while (* static_cast<bool*>(activity_mark))
	{
		sleep(2);
		pthread_mutex_lock(&CleanerStackMutex);
		while (!CleanerStack.empty())
		{
			pthread_t t = CleanerStack.top();
			pthread_join(t,NULL);
			CleanerStack.pop();
			cout << "Joined thread" << endl;
		}
		pthread_mutex_unlock(&CleanerStackMutex);
	}
	return 0;
}
	

Server::Server() {;}
void Server::Start(int portnum,ConnectionFunc _f){
    
    f=_f;
    active=true;
	struct sockaddr_in srv;
	socklen_t socklen;
	int iSockFD;
	if ((iSockFD=socket(PF_INET,SOCK_STREAM,0))<0)
		err("socket");

	int opt = 1, len = 4;
	setsockopt(iSockFD, SOL_SOCKET, SO_REUSEADDR, &opt, len);

	memset(&srv,0,sizeof(srv));
	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = htonl(INADDR_ANY);
	srv.sin_port = htons(portnum);
   
	socklen=sizeof(srv);

	if (bind(iSockFD, (struct sockaddr *) & srv, socklen) < 0)
		err("bind");

	struct sockaddr_in cli;
	int fd;
	listen(iSockFD,5);

	if(pthread_mutex_init(&CleanerStackMutex,NULL))
	    err("mutex_initialize");
	pthread_t cleaner_thread;

	if (fcntl(iSockFD, F_SETFL, O_NDELAY) < 0) 
	    err("Can't make nonblocking socked");

	if (pthread_create (&cleaner_thread,NULL,CleanerFunc,&active))
	    err("pthread_create (&cleaner_thread,NULL,CleanerFunc,NULL)");
	
	while (active)
	{
	    fd = accept(iSockFD, (struct sockaddr *) & cli, & socklen);
	    if (fd>0)
		new __Connection(fd);
	    else
		usleep(1000);
	}
	if (pthread_join(cleaner_thread,NULL))
		err("pthread_join(&cleaner_thread,NULL)");
	pthread_mutex_destroy(&CleanerStackMutex);
	close(iSockFD);

}

void Server::Stop()
{
    active=false;
}
