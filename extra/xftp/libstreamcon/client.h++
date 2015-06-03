#ifndef _SCLIENT_H_
#define _SCLIENT_H_

#include "connection.h++"

namespace streamcon {

	/*!
	  Template for creating client application classes.
	  Connection type is a parameter.
	*/
	template <class T>
	class ClientApp
	{
	private:
		//! connection owned by the client
		T* connection;

		__DE(Disconnected, Exception);
		int doexit;

	protected:
		/*!
		  Connection event.
		*/
		virtual void OnConnect(Connection &conn) throw() = 0;
		/*!
		  Disconnection event.
		*/
		virtual void OnDisconnect() throw() = 0;

		/*!
		  Exit event. Triggered after successful ending of the main loop.
		*/
		virtual void OnExit() throw() {;};

		/*!
		  \return reference to \a Connection

		  \exception Disconnected is thrown if the client is not connected
		  and there's no \a Connection object to return a reference to.
		*/
		virtual T& GetConnection() throw(Disconnected) 
			{
				if(connection)
					return *connection;
				else
					THROW(Disconnected);
			}
	public:
		~ClientApp() {
			if(connection)
				delete connection;
		}
		
		/*!
		  Order main loop to break.
		*/
		void Exit() throw()
			{
				doexit = 1;
				OnExit();
			}

		/*!
		  \param address hostname, ipv4 or ipv6 address.
		  \param port target port
		  \param maxStreams maximum number of streams allowed to use

		  Connect and block until disconnected or severed.

		  \exception Connection::ConnectionFailure is thrown had the
		  actual connection process failed.
		*/
		void Connect(const std::string& address, int port,
							 int maxStreams)
			throw(Connection::ConnectionFailure)
			{
				doexit = 0;
				connection = new T();
				connection->Start(address, port, maxStreams);
				OnConnect(this->GetConnection());
				try {
					while(!doexit) {
						connection->LoopOnce();
					}
				} catch(Connection::ConnectionSevered) {
					Disconnect();
					THROW(Connection::ConnectionSevered);
				}
				OnExit();
			}
		/*!
		  Disconnect the client and delete the connection.
		*/
		void Disconnect() throw()
			{				
				OnDisconnect();
				delete connection;
				connection = NULL;
			}
	};
}

#endif // _SCLIENT_H_
