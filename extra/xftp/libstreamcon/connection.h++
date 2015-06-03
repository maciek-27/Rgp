#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "commons.h++"
#include "throw.h++"
#include <vector>
#include "threads.h++"
#include "databuffer.h++"

namespace streamcon {

	class Stream;

	//! default port for connection
	const int defaultPort = 2015;

	//! default value for negotiating number of streams
	const int defaultStreams = 1024;
	
	/*!
	  Class provides an interface for building stream-oriented
	  connections.

	  Connection class can represent both client-side and server-side
	  connections. 

	  \note See \a Server for building server-side connections.
	  \note See \a Client for building client-side connections.
	*/
	class Connection {
	private:
		std::vector<Stream *> streams;

		/*! maximum number of simultaneous streams 
		  \note this value should be negotiated with the server part
		  so that the maximal amount of streams 
		*/
		int maxStreams;

		RecursiveMutex allocMutex;

		int doexit;

	public:
		__DE(NoStreamsAvailable, Exception);
		__DE(BadStreamProvided, Exception);
		__DE(SuchStreamNotAllocated, Exception);
		__DE(StreamIdOccupied, Exception);
		__DE(StreamIdExceeded, Exception);

		/*!
		  \return address of the remote host represented by this connection
		*/
		const std::string& GetAddress() const throw();
		/*!
		  \return port of the remote host represented by this connection
		*/	   
		int GetPort() const throw();

	protected:
		//! address for remote host
		std::string address;
		//! target port
		int port;

		/*!
		  \param streamId
		  \param buffer
		  \param size

		  Send data to a given stream.

		  /exception DataBuffer::Interrupted is thrown upon
		  the interruption of underlying data buffer's blocking
		  operations.
		  \exception SuchStreamNotAllocated is thrown if the given
		  streamId was not pre assigned to any Stream.
		  \exception StreamIdExceeded is thrown if streamId is beyond
		  negotiated limit.
		*/
		virtual void SendData(int streamId, char* buffer, size_t size)
			throw(SuchStreamNotAllocated,
				   StreamIdExceeded, DataBuffer::Interrupted) = 0;
		
		/*!
		  \param streamId
		  \param buffer
		  \param size

		  Retrieve data from a given stream.

		  /exception DataBuffer::Interrupted is thrown upon
		  the interruption of underlying data buffer's blocking
		  operations.
		  \exception SuchStreamNotAllocated is thrown if the given
		  streamId was not pre assigned to any Stream.
		  \exception StreamIdExceeded is thrown if streamId is beyond
		  negotiated limit.
		*/
		virtual int GetData(int streamId, char* buffer, size_t size)
			throw(SuchStreamNotAllocated,
				  StreamIdExceeded, DataBuffer::Interrupted) = 0;

		/*!
		  \param streamId

		  Interrupt blocking operations inside a given stream.

		  \exception SuchStreamNotAllocated is thrown if the given
		  streamId was not pre assigned to any Stream.
		  \exception StreamIdExceeded is thrown if streamId is beyond
		  negotiated limit.
		*/
		virtual void Interrupt(int streamId)
			throw(SuchStreamNotAllocated,
				  StreamIdExceeded) = 0;

		/*!
		  \param _maxStreams
		
		  Set maximum number of streams. Maximum number of streams
		  is usually set during the connection initialization.
		*/
		virtual void SetMaxStreams(int _maxStreams) throw();

	public:
		__DE(ConnectionFailure, Exception);

		__DE(ConnectionSevered, ConnectionFailure);
		__DE(ConnectionTimeout, ConnectionFailure);
		__DE(ConnectionRefused, ConnectionFailure);
		__DE(AddressResolutionFailure, ConnectionFailure);
		__DE(BadAddress, ConnectionFailure);

		Connection() throw();
		virtual ~Connection() throw();

		/*!
		  \param address any Internet address i.e domain name, ipv4 address
		  or ipv6 address
		  \param port target port to connect to
		  \param maxStreams maximum number of streams to negotiate

		  \note the negotiated number of streams will be the minimum of
		  both client's and server's number of streams

		  This function should initiate a connection to the remote
		  host and do any necessary configuration trasmissions.
		  
		  \exception ConnectionFailure is thrown in case of any problems,
		  see specialized versions of this exception for further details.
		*/		
		virtual void Start(const std::string& address, int port,
						   int maxStreams) throw(ConnectionFailure) = 0;
		
		/*!
		  Function blocks the current thread until the connection is
		  finished and thus should be started in a separate thread.

		  \exception ConnectionSevered is thrown whenever the remote
		  part has broken the connection.
		*/
		virtual void Loop() throw(ConnectionSevered);

		/*!
		  Makes a one time loop which can be used to gently extend
		  \a Loop

		  \exception ConnectionSevered is thrown whenever the remote
		  part has broken the connection.
		*/
		virtual void LoopOnce() throw(ConnectionSevered) = 0;
		
		/*!
		  \return maximum number of negotiated streams
		*/
		virtual int GetMaxStreams() throw();

		/*!
		  Allocates resources for handling new logical data stream. First available
		  stream slot is returned and the stream id can be read from \a Stream itself.

		  \return reference to new \a Stream
		*/
		virtual Stream& AllocStream() throw(NoStreamsAvailable);

		/*!
		  \param id 

		  Allocates resources for handling new logical data stream. Tries to use
		  a given id.

		  \return reference to new \a Stream

		  \exception StreamIdOccupied is thrown if there's already an active stream designated
		  by given \a id
		  \exception StreamIdExceeded is thrown if the given \a id is out of bounds in regards
		  to negotiated value which can be obtained by \a GetMaxStreams
		*/
		virtual Stream& AllocStream(int id) throw(StreamIdOccupied,
												  StreamIdExceeded);

		/*!
		  \param stream Stream to disengage
		  Frees resources allocated for handling a particular stream.
		*/
		virtual void CloseStream(Stream& stream)
			throw(BadStreamProvided);

		/*!
		  \param id Stream id number
		  \return reference to associated stream
		  \exception SuchStreamNotAllocated is thrown had the given id does not designate into
		  an allocated Stream.
		*/
		virtual Stream& GetStreamById(int id) throw(SuchStreamNotAllocated,
													StreamIdExceeded);
		
		/*!
		  \param id Stream id number
		  \return reference to associated stream
		  \exception SuchStreamNotAllocated is thrown had the given id does not designate into
		  an allocated Stream.
		*/
		virtual Stream& operator[](int id) throw(SuchStreamNotAllocated,
												 StreamIdExceeded);

		friend class Stream;
	};

}

#endif // _CONNECTION_H_
