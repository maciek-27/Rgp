#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <poll.h>

#include "sctpconnection.h++"
#include "stream.h++"
#include "databuffer.h++"

using namespace streamcon;

SCTPConnection::SCTPConnection() throw()
{

}

SCTPConnection::SCTPConnection(int _sockfd, int _maxStreams) throw()
	:sockfd(_sockfd)
{
	struct sctp_event_subscribe events;
	SetMaxStreams(_maxStreams);
	struct sockaddr_storage peer_info;

	// retrieve sockaddr from socket
	socklen_t len = sizeof(sockaddr_storage);
	getpeername(_sockfd, (struct sockaddr *)&peer_info, &len);

	// resolve the address into hostname and numeric port
	char host[NI_MAXHOST], service[NI_MAXSERV];
	int s = getnameinfo((struct sockaddr *)&peer_info,
						len, host, NI_MAXHOST,
					service, NI_MAXSERV, NI_NUMERICSERV);

	// resolve successful
	if (s == 0) {
		address = host;
		port = atoi(service);
	}
	else
		fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

	bzero(&events, sizeof(events));

	// data events are crucial for handling multiple streams
	events.sctp_data_io_event = 1;
	setsockopt(_sockfd, IPPROTO_SCTP, SCTP_EVENTS,
			   &events, sizeof(events));

	setsockopt(_sockfd, IPPROTO_SCTP, SCTP_EVENTS,
			   &events, sizeof(events));
	int nodelay = 1;
	setsockopt(_sockfd, SOL_SCTP, SCTP_NODELAY, &nodelay, sizeof(nodelay));



	AllocStream(0);
}

class _auto_freeaddrinfo{
public:
	struct addrinfo **ptr;
	_auto_freeaddrinfo(struct addrinfo **_ptr) : ptr(_ptr) {;}
	~_auto_freeaddrinfo() {	if(*ptr) freeaddrinfo(*ptr);}
};

void SCTPConnection::Start(const std::string& address, int port,
						   int _maxStreams) throw(ConnectionFailure)
{
	struct sctp_initmsg initmsg;
	struct sctp_status status;
	struct sctp_event_subscribe events;

	struct addrinfo *addrresult = NULL;

	{
		_auto_freeaddrinfo a(&addrresult);
		
		{
			struct addrinfo hints;
			char portstr[6]; // max length of a 64bit integer
			snprintf(portstr, 6, "%i", port);
			
			memset(&hints, 0, sizeof(hints));
			hints.ai_socktype = SOCK_STREAM;
			
			// Below causes getaddrinfo's error ...
//		hints.ai_protocol = IPPROTO_SCTP;
			
			int error;
			if((error = getaddrinfo(address.c_str(), portstr, &hints,
									&addrresult )) < 0)
				THROWP(AddressResolutionFailure, gai_strerror(error));
			if(!addrresult)
				THROW(AddressResolutionFailure);
			
		}
		sockfd = socket(addrresult->ai_family,
						addrresult->ai_socktype,
						IPPROTO_SCTP);
		
		if(sockfd < 0) {
			if(errno == EPROTONOSUPPORT)
				THROW(SCTPNotSupported);
			else
				THROW(SocketSetupFailure);
		}

		bzero(&events, sizeof(events));
		events.sctp_data_io_event = 1;
		events.sctp_association_event = 1;
		if(setsockopt(sockfd, IPPROTO_SCTP, SCTP_EVENTS,
					  &events, sizeof(events)) < 0) {
			THROW(SocketSetupFailure);
		}
		
		memset(&initmsg, 0, sizeof(initmsg));
		initmsg.sinit_max_instreams = _maxStreams;
		initmsg.sinit_num_ostreams = _maxStreams;
		if(setsockopt(sockfd, IPPROTO_SCTP, SCTP_INITMSG,
					  &initmsg, sizeof(initmsg)) < 0) {
			THROW(SocketSetupFailure);
		}
		int nodelay = 1;
		setsockopt(sockfd, SOL_SCTP, SCTP_NODELAY, &nodelay, sizeof(nodelay));

		if(connect(sockfd, addrresult->ai_addr, addrresult->ai_addrlen) < 0)
		{
			switch(errno)
			{
			case ECONNREFUSED:
				THROW(ConnectionRefused);
			case EAFNOSUPPORT:
				THROW(BadAddress);
			case ETIMEDOUT:
				THROW(ConnectionTimeout);
			default:
				THROW(ConnectionFailure);
			}	
		}
	}
	bzero(&status, sizeof(status));
	socklen_t len = sizeof(status);
	status.sstat_assoc_id = 1;

	if (getsockopt(sockfd, IPPROTO_SCTP, SCTP_STATUS, &status, &len) < 0) {
		THROW(ConnectionFailure);
	}
	int maxStreams = std::min(status.sstat_instrms, status.sstat_outstrms);
	SetMaxStreams(maxStreams);

	AllocStream();

	this->address = address;
	this->port = port;
}


// function to set blocking or non blocking mode on a socket
// state - 1 for nonblocking, 0 for blocking
static void setnonblocking(int sock, int state)
{
	int opts;

	if( (opts = fcntl(sock,F_GETFL)) < 0) {
		perror("fcntl");
		return;
	}

	if(state)
		opts = (opts | O_NONBLOCK);
	else
		opts &= ~O_NONBLOCK;

	if (fcntl(sock, F_SETFL, opts) < 0) {
		perror("fcntl(F");
		exit(EXIT_FAILURE);
	}
}

void SCTPConnection::LoopOnce() throw(ConnectionSevered)
{
	char buffer[sctpMaxMessage];
	struct sctp_sndrcvinfo sinfo;

	bzero(&sinfo, sizeof(sinfo));

	
	setnonblocking(sockfd, 1);
	

	int flags = 0;
	int len = 0;
	struct pollfd pollspec;
	pollspec.fd = sockfd;
	pollspec.events = POLLIN; // request if data is in
	pollspec.revents = 0;

	poll(&pollspec, 1, 10);

	if((pollspec.revents & POLLIN) == 0) // no data to read
		len = -1;
	else {
		len = sctp_recvmsg(sockfd, buffer, sctpMaxMessage, NULL, NULL,
						   &sinfo, &flags);
	}
	if(len == -1)
		return;
	
	setnonblocking(sockfd, 0);

	if(len == 0) {
		THROW(ConnectionSevered);
	}
	
	if(flags & MSG_NOTIFICATION) {
		return;
	}

	
	try {
		GetStreamById(sinfo.sinfo_stream); // this should throw

		try {
			inbuffers[sinfo.sinfo_stream]->Write(buffer, len);
		} catch(DataBuffer::Interrupted) {
			;
		}
	} catch(StreamIdExceeded) {
		;
	} catch(SuchStreamNotAllocated) {
		;
	}
}

SCTPConnection::~SCTPConnection() throw()
{
	close(sockfd);
	for(int i = 0;i < GetMaxStreams();i++) {
		delete inbuffers[i];
		inbuffers[i] = NULL;
	}
}

void SCTPConnection::SendData(int streamId, char* buffer, size_t size)
	throw(SuchStreamNotAllocated, StreamIdExceeded,
		  DataBuffer::Interrupted)
{
	GetStreamById(streamId); // this should throw exceptions on error
	
	size_t sent = 0;
	do {

	    int status =
			sctp_sendmsg(
				sockfd,
				(void*)((char *)buffer + sent),
				std::min(size - (size_t)sent, sctpMaxMessage),
				NULL, 0, 3141592, 				
				0, static_cast<uint16_t>(streamId), 0, 123);
		if(status != -1) {
			sent += status;
		}
/*		else {
			perror("sctp_sendmsg");
			}*/
	} while(sent < size); // sent data until all is passed down
	// 3141592 is a value that can be passed along with the message
	// 123 is a number identifying failed messages
	//    both of them are not of any use at the moment
}

void SCTPConnection::Interrupt(int streamId)
	throw(SuchStreamNotAllocated, StreamIdExceeded)
{
	GetStreamById(streamId); // this should throw exceptions on error
	inbuffers[streamId]->Interrupt();
}

int SCTPConnection::GetData(int streamId, char* buffer, size_t size)
	throw(SuchStreamNotAllocated, StreamIdExceeded,
		  DataBuffer::Interrupted)
{
	GetStreamById(streamId); // this should throw exceptions on error
	return inbuffers[streamId]->Read(buffer, size);

}

Stream& SCTPConnection::AllocStream() throw(NoStreamsAvailable)
{
	return Connection::AllocStream();
}

Stream& SCTPConnection::AllocStream(int id) throw(StreamIdOccupied,
												  StreamIdExceeded)
{
	inbuffers[id] = new DataBuffer(sctpBufferSize);
	return Connection::AllocStream(id);
}

void SCTPConnection::SetMaxStreams(int _maxStreams) throw()
{
	inbuffers.resize(_maxStreams);
	Connection::SetMaxStreams(_maxStreams);
}
