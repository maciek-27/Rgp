#ifndef _SUPERDUPERCLIENT_H_
#define _SUPERDUPERCLIENT_H_

#include <libxftp/xftpclient.h++>
#include <libxftp/clientcontrol.h++>
#include <libxftp/message.h++>
#include <libxftp/servercontrol.h++>
#include <boost/filesystem.hpp>
#include "client.h++"
#include "supercontrol.h++"
#include "superdata.h++"

namespace xftpclient {
	class SuperDuperClient : public xftp::xftpClient<SuperControl, 
							 xftp::Client<SuperControl,SuperData> > {
	protected:
		void OnConnect(xftp::Connection &conn) throw()
			{
				xftp::xftpClient<SuperControl,
					xftp::Client<SuperControl,SuperData> >::OnConnect(conn);
				std::cout << "Client is connected!\n" << std::endl;
			}
		void OnDisconnect() throw()
			{
				xftp::xftpClient<SuperControl,
					xftp::Client<SuperControl,SuperData> >::OnDisconnect();
				std::cout << "Client is disconnected!\n" << std::endl;
			}
	};

}

#endif
