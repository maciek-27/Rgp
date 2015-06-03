#include "libxftp/xftpserver.h++"
#include "libxftp/allmessages.h++"
#include "libxftp/zipfilters.h++"
using namespace xftp::stdfilter;

using namespace xftp;
using namespace streamcon;
using std::tr1::shared_ptr;
using namespace server;

class SuperControl : public ControlThread {
public:
	SuperControl(ClientBase& client, 
				 Stream& stream) : ControlThread(client, stream)
		{
			;
		};
protected:
	void Run()
		{
			xmlStream.SetGetFilter( UnZCompress);
			xmlStream.SetPostFilter( ZCompress);
			std::cout << "Recieving ..";
			std::tr1::shared_ptr<Message> ptr = xmlStream.GetMessage();
			std::cout << "Recieved " << toNative(ptr->GetName())
					  << std::endl;

			MsgList *lst = new MsgList;
			shared_ptr<Message> l(lst);
			lst->Fill("/", 2);
			std::cout << "Sending list ..";
			xmlStream.PostMessage(l);
			std::cout << "Sent" << std::endl;
			
			std::cout << "Thread running" << std::endl;
		};
};

class SuperData : public DataThread
{
public:
	SuperData(ClientBase& client, 
			  Stream& stream) : DataThread(client, stream)
		{
			;
		};
protected:
	void Run()
		{

			;
		}
};

class SuperDuperServer : public xftpServer<SuperControl,
										   Client<SuperControl, SuperData> >
{
protected:
	void OnNewConnection(Connection& connection) throw()
		{
			xftpServer<SuperControl,
				Client<SuperControl, SuperData> >::OnNewConnection(connection);
			
			std::cout << "New client has connected" << std::endl;
		}
	void OnCloseConnection(Connection& connection) throw()
		{
			xftpServer<SuperControl,
				Client<SuperControl, SuperData> >::OnCloseConnection(connection);
			
			std::cout << "Client has disconnected!\n" << std::endl;
		}
};

int main(int argc, char **argv)
{
	SuperDuperServer server;
	server.Listen(argc >= 2?argv[1]:"");
	
	return 0;
}
