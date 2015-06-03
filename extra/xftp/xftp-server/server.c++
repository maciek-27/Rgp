#include "libxftp/xftpserver.h++"
#include "libxftp/allmessages.h++"
#include "libxftp/zipfilters.h++"
#include "libxftp/servercontrol.h++"
#include "libxftp/client.h++"


#include <boost/function.hpp>
#include <boost/functional.hpp>

using namespace xftp::stdfilter;
using namespace xftp;
using namespace streamcon;
using std::tr1::shared_ptr;
using namespace server;

namespace xftpserver
{

class SuperControl : public ServerControl
{
public:

	SuperControl(ClientBase& client,
				Stream& stream) : ServerControl(client, stream)
	{
		;
	};

	void OnOpen(client::MsgOpen& m)
	{
		using namespace std;
		cout << "Opening File!";
		m.DefaultProcessImpl(*this);
	}
protected:

	void Run()
	{
		// demo server processes compressed data, so we have to start
		// compression/decompression for all incomming and outgoing XML
		// streams
		xmlStream.SetGetFilter(UnZCompress);
		xmlStream.SetPostFilter(ZCompress);

		client::MsgOpen o;
		o.SetProcessImpl(std::mem_fun(&SuperControl::OnOpen));
		while (1)
		{
			std::tr1::shared_ptr<Message> ptr = xmlStream.GetMessage();
			if (loggedIn)
				ptr->Process(*this);
			else if (!loggedIn &&
					(toNative(ptr->GetName()) == "hello" ||
					toNative(ptr->GetName()) == "auth"))
				ptr->Process(*this);

		}

		//			std::cout << "Thread running" << std::endl;
	};
};

class SuperData : public DataThread
{
public:

	SuperData(ClientBase& client,
			Stream& stream) : DataThread(client, stream)
	{
	};
protected:

	void Run()
	{
		;
	}
};

class SuperClient : public Client<SuperControl, SuperData>
{
public:

	SuperClient(Connection& _connection)
	: Client<SuperControl, SuperData>(_connection)
	{
		;
	}
};

// class supplies methods called on events
class SuperDuperServer : public xftpServer<SuperControl, SuperClient>
{
protected:
	// triggered wen new connection is granted
	void OnNewClient(SuperClient &client)
	{
		xftpServer<SuperControl, SuperClient>::OnNewClient(client);

		std::cout << "New client has connected (" <<
				client.GetConnection().GetAddress()
				<< ":" << client.GetConnection().GetPort()
				<< ")" << std::endl;
	}
	// triggered when connection is lost
	void OnLeaveClient(SuperClient &client)
	{
		xftpServer<SuperControl, SuperClient>::OnLeaveClient(client);
		std::cout << "Client has disconnected (" <<
				client.GetConnection().GetAddress() << ":" << client.GetConnection().GetPort()
				<< ")" << std::endl;
	}

	// triggered when connection is accepted
	void OnNewConnection(Connection& connection) throw()
		{
			xftpServer<SuperControl,
				SuperClient>::OnNewConnection(connection);
			
		}
	// triggered when connection is lost
	void OnCloseConnection(Connection& connection) throw()
		{
			xftpServer<SuperControl,
				SuperClient>::OnCloseConnection(connection);		  
		}
};
}

int main(int argc, char **argv)
{
	//only action of server is to listen.
	xftpserver::SuperDuperServer server;
	server.Listen(argc >= 2?argv[1]:"");
	return 0;
}
