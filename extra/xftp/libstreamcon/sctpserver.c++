#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <vector>

#include "sctpserver.h++"
#include "sctpconnection.h++"

using namespace streamcon;

class _auto_freeaddrinfo{
public:
	struct addrinfo **ptr;
	_auto_freeaddrinfo(struct addrinfo **_ptr) : ptr(_ptr) {;}
	~_auto_freeaddrinfo() {	if(*ptr) freeaddrinfo(*ptr);}
};

SCTPServer::SCTPServer() throw()
{
	;
}

SCTPServer::~SCTPServer() throw()
{
	;
}

void SCTPServer::Listen(const std::string& interface, int port,
						int _maxStreams)
	throw(ListenException)
{
	struct addrinfo *addrresult = NULL, *rp;
	struct sockaddr_in client_addr;
	
/* the below code may be used in the future for allowing to selectively
   bind to many interfaces, not it has proved impossible to do in universal
   way */
//	std::vector<std::string> interfaces;
//	for(int i = 0;i < interface.length();) { /* generate list of requested
//												addresses to bind to */
//		int from = i;
//		i = interface.find(",", i);
//		if(i == -1) { // not found, single address or the last on the list
//			interfaces.push_back( interface.substr(from) );
//			break;
//		}
//		i = i+1;
//		interfaces.push_back( interface.substr(from, (i-1)-from) );
//	}
	{
		_auto_freeaddrinfo a(&addrresult);
		{
			struct addrinfo hints;
			char portstr[6];
			snprintf(portstr, 6, "%i", port); // convert port to C string format
			
			memset(&hints, 0, sizeof(hints));
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags |= AI_NUMERICSERV;
			hints.ai_flags |= AI_ADDRCONFIG;
			hints.ai_family = AF_UNSPEC; // allow IPv4 and IPv6
			
			if(interface.length() == 0) {
				hints.ai_flags |= AI_PASSIVE; // accept on any interface
			}
			
			int error;
			if((error = getaddrinfo(interface.length()?
									interface.c_str():NULL, portstr, &hints,
									&addrresult)) < 0) {
				THROW(SCTPListenException);
			}
			if(!addrresult)
				THROW(SCTPListenException);
		}
		sockfd = socket(addrresult->ai_family,
						addrresult->ai_socktype,
						IPPROTO_SCTP);
		if(sockfd < 0) {
			THROW(SCTPListenException);
		}
		
		int bound = 0;
		for (rp = addrresult; rp != NULL; rp = rp->ai_next) {
			if(sctp_bindx(sockfd, rp->ai_addr, 1,
						  SCTP_BINDX_ADD_ADDR) == 0)
				bound++;
		}
		if(!bound) // couldn't bind to any address
			THROW(SCTPListenException);
	
//	if(bind(sockfd, addrresult->ai_addr, addrresult->ai_addrlen) < 0) {
//		THROW(SCTPListenException);
//	}
	}	

	struct sctp_initmsg initmsg;
	bzero(&initmsg, sizeof(initmsg));
	initmsg.sinit_max_instreams = _maxStreams;
	initmsg.sinit_num_ostreams = _maxStreams;
	if(setsockopt(sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, 
				  sizeof(initmsg))) {
		THROW(SCTPListenException);
	}
	

	struct sctp_event_subscribe events;
	bzero(&events, sizeof(events));
	events.sctp_data_io_event = 1;
	if(setsockopt(sockfd, IPPROTO_SCTP, SCTP_EVENTS, &events,
				  sizeof(events))) {
		THROW(SCTPListenException);
	}
		
	listen(sockfd, 128);

	while(1) {
		socklen_t socklen = sizeof(client_addr);;
		int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr,
						   &socklen);
		
		if(errno == EINTR) {
			;
		}

		if(client_sockfd < 0) {
			THROW(SCTPListenException);
		}
		SCTPConnection* clientConn =
			new SCTPConnection(client_sockfd, _maxStreams);

		AddConnection(*clientConn);
		watchers[clientConn] = new ConnWatcher(this, clientConn);
		OnNewConnection(*clientConn); // handle the event first, before recieving data
		watchers[clientConn]->Start();
	}
}

void SCTPServer::ConnWatcher::Execute(void *arg)
{	
	try {
		while(!Interrupt()) {
			conn.LoopOnce();
		}
	}
	catch(Connection::ConnectionSevered) {

	};
}

SCTPServer::ConnWatcher::~ConnWatcher()
{
	owner.OnCloseConnection(conn);
}

void SCTPServer::DelConnection(Connection& connection)
	throw(NoSuchConnection)
{
	delete watchers[&connection];
	watchers.erase(&connection);
	Server::DelConnection(connection);
}

void SCTPServer::CloseConnection(Connection& connection)
	throw(NoSuchConnection)
{
	DelConnection(connection);	
}
