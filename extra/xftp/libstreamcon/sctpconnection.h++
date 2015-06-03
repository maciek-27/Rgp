#ifndef _SCTPCONNECTION_H_
#define _SCTPCONNECTION_H_

#include "connection.h++"
#include "threads.h++"

namespace streamcon {
	
	class DataBuffer;

	const int sctpBufferSize = 65536;
	const size_t sctpMaxMessage = 16384;

	/*!
	  Connection based on SCTP protocol.
	*/
	class SCTPConnection : public Connection {
	private:
		int sockfd;
		std::vector<DataBuffer *> inbuffers;
		
	protected:
		virtual void SendData(int streamId, char* buffer, size_t size)
			throw(SuchStreamNotAllocated,
				  StreamIdExceeded,DataBuffer::Interrupted);
		virtual int GetData(int streamId, char* buffer, size_t size)
			throw(SuchStreamNotAllocated,
				  StreamIdExceeded,DataBuffer::Interrupted);
		virtual void Interrupt(int streamId)
			throw(SuchStreamNotAllocated,
				  StreamIdExceeded);

		virtual void SetMaxStreams(int _maxStreams) throw();

	public:		
		__DE(SocketSetupFailure, ConnectionFailure);
		__DE(SCTPNotSupported, SocketSetupFailure);
		
		SCTPConnection() throw();
		SCTPConnection(int _sockfd, int _maxStreams) throw();
		virtual ~SCTPConnection() throw();

		virtual void Start(const std::string& address, int port,
						   int maxStreams = defaultStreams)
			throw(ConnectionFailure);
		virtual void LoopOnce() throw(ConnectionSevered);

		virtual Stream& AllocStream() throw(NoStreamsAvailable);
		virtual Stream& AllocStream(int id) throw(StreamIdOccupied,
												  StreamIdExceeded);
		friend class SCTPServer;
	};
}

#endif // _SCTPCONNECTION_H_
