#ifndef _SCTPSERVER_H_
#define _SCTPSERVER_H_

#include "server.h++"
#include "sctpconnection.h++"
#include <map>

namespace streamcon {

	/*!
	  Server based on SCTPConnection class.
	*/
	class SCTPServer : public Server
	{
	private:
		int sockfd;
		/*!
		  Per connection watcher.
		*/
		class ConnWatcher : public Thread
		{
		private:
			SCTPServer& owner;
			SCTPConnection& conn;
		protected:
			ConnWatcher(SCTPServer* _owner, SCTPConnection* _conn) :
				owner(*_owner), conn(*_conn) {;};
			~ConnWatcher();
			void Execute(void *arg);
			void Clean() {
				owner.DelConnection(conn);
			};
			friend class SCTPServer;
		};
		std::map<Connection *, ConnWatcher *> watchers;
	protected:
		SCTPServer() throw();
		virtual ~SCTPServer() throw();
		
		virtual void DelConnection(Connection& connection)
			throw(NoSuchConnection);

		virtual void OnNewConnection(Connection& connection) throw() = 0;
		
		virtual void OnCloseConnection(Connection& connection) throw() = 0;
	public:
		virtual void CloseConnection(Connection& connection)
			throw(NoSuchConnection);

		__DE(SCTPListenException, ListenException);
		virtual void Listen(const std::string& interface = "",
							int port = defaultPort,
							int _maxStreams = defaultStreams)
			throw(ListenException);
	};


}

#endif // _SCTPSERVER_H_
