#ifndef _XFTPSERVER_H_
#define _XFTPSERVER_H_

#include "xftpcommons.h++"
#include "libstreamcon/multiserver.h++"
#include <libxftp/client.h++>

namespace xftp {
	template <class _Control, class _Client>
	class xftpServer : public streamcon::MultiServer
	{
	protected:
		std::map<streamcon::Connection*, _Client*> clients;

		~xftpServer() throw() {
			for(typename std::map<streamcon::Connection*, _Client*>::iterator i = clients.begin();
				i != clients.end();i++) {
				delete i->second;
				clients.erase(i);	
			}
		}

		_Client& GetClient(Connection& connection)
			{
				return *clients[connection];
			}

		virtual void OnNewClient(_Client& client)
			{
				;
			}		
		virtual void OnLeaveClient(_Client& client)
			{
				;
			}
		virtual void OnNewConnection(streamcon::Connection& connection) throw()
			{
				clients[&connection] = new _Client(connection);
				OnNewClient(*clients[&connection]);
			}
		virtual void OnCloseConnection(streamcon::Connection& connection) throw()
			{
				OnLeaveClient(*clients[&connection]);
				delete clients[&connection];
				clients.erase(&connection);
			}
		
	};
}

#endif // _XFTP_SERVER_H_
