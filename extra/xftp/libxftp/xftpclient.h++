#ifndef __XFTP__XFTPCLIENT__
#define __XFTP__XFTPCLIENT__

#include "libstreamcon/multiclient.h++"
#include <libxftp/xftpcommons.h++>
#include <libxftp/xmlstream.h++>
#include <libxftp/client.h++>

namespace xftp {
	template <class _Control, class _Client>
	class xftpClient : public streamcon::MultiClient
	{
	protected:
		_Client *client;
		void OnConnect(Connection &conn) throw()
			{
				client = new _Client(conn);
			}
		void OnDisconnect() throw()
			{
				delete client;
			}
	};
}

#endif
