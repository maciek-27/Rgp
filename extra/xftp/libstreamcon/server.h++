#ifndef _SERVER_H_
#define _SERVER_H_

#include <set>
#include "connection.h++"

namespace streamcon {
	class Server
	{
	private:
		std::set<Connection *> connections;
		
	protected:
		__DE(ConnectionAlreadyAdded, Exception);
		__DE(NoSuchConnection, Exception);

		__DE(ListenException, Exception);
		__DE(BadInterfaceProvided, ListenException);
		__DE(PermissionDenied, ListenException);
		
		/*!
		  \param connection
		  
		  Adds connection to the pool of connections.
		*/
		virtual void AddConnection(Connection& connection)
			throw(ConnectionAlreadyAdded);
		
		/*!
		  \param connection
		  
		  Removes connection from the pool of connections
		*/
		virtual void DelConnection(Connection& connection)
			throw(NoSuchConnection);
		
		Server() throw();
		virtual ~Server() throw();
		
		/*!
		  Listens for new connections and calls \a OnNewConnection
		  events.

		  \exception ListenException is thrown on any problems.
		*/
		virtual void Listen(const std::string& interface = "",
							int port = defaultPort,
							int _maxStreams = defaultStreams)
			throw(ListenException) = 0;
		
		/*!
		  \param connection
		  Incoming connection event. This event can be used to attach
		  incoming connection to any custom client.
		*/
		virtual void OnNewConnection(Connection& connection) throw() = 0;
		
		/*!
		  \param connection
		  Connection close event. This event can be used to detach
		  connections from custom client.
		*/
		virtual void OnCloseConnection(Connection& connection) throw() = 0;
		
	public:
		/*!
		  Request ending a connection.
		*/
		virtual void CloseConnection(Connection& connection)
			throw(NoSuchConnection) = 0;
	};
}

#endif // _SERVER_H_
