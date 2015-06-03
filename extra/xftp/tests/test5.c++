#include <libxftp/xftpclient.h++>
#include <libxftp/allmessages.h++>
#include <libxftp/zipfilters.h++>
using namespace xftp;
using namespace xftp::stdfilter;
using namespace streamcon;
using std::tr1::shared_ptr;

class SuperControl : public ControlThread {
public:
	SuperControl(ClientBase& client, 
				 Stream& _stream) : ControlThread(client, _stream) {;}
protected:

	class PrintXML
	{
	private :
		int c;
	public :
		PrintXML(int color) : c(color) {}
		size_t operator ()(boost::scoped_array<XMLByte>& b,size_t s)
			{
				if (s<400)
					std::cout << "\x1b["<< c <<"m" << b.get() << "\x1b[0m" << std::endl;
				else
				{
					XMLByte t = b[200];
					b[200]=0;
					std::cout << "\x1b["<< c <<"m" << b.get()
							  << "\n(...)\n" 
							  << "\x1b[0m" << std::endl;
					b[200]=t;
				}
				return s;
			}
	};



	void Run()
		{
			FilterChain InputFilters;
			InputFilters.AddFilter(UnZCompress);
			InputFilters.AddFilter(PrintXML(32));
			xmlStream.SetGetFilter( InputFilters);

			FilterChain OutputFilters;
			OutputFilters.AddFilter(PrintXML(33));
			OutputFilters.AddFilter(ZCompress);
			xmlStream.SetPostFilter( OutputFilters);


			std::cout << "Sending hello ..";
			xmlStream.PostMessage(
				shared_ptr<Message>(new server::MsgHello));
			
			std::cout << "DONE" << std::endl;
						
			std::tr1::shared_ptr<Message> ptr = xmlStream.GetMessage();
			std::cout << "Recieved " << toNative(ptr->GetName()) << std::endl;
			
			std::cout << "Thread running!" << std::endl;
		}
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

class SuperDuperClient : public xftpClient<SuperControl,
										   Client<SuperControl,SuperData> > {
protected:
	void OnConnect(Connection &conn) throw()
		{
			xftpClient<SuperControl,
				Client<SuperControl, SuperData> >::OnConnect(conn);
			std::cout << "Client is connected!\n" << std::endl;
		}
	void OnDisconnect() throw()
		{
			xftpClient<SuperControl,
				Client<SuperControl, SuperData> >::OnDisconnect();
			std::cout << "Client is disconnected!\n" << std::endl;
		}
};

int main(void)
{
	SuperDuperClient client;

	while(1) {
		try {
			client.Connect("127.0.0.1", defaultPort, defaultStreams);
		} catch(Connection::ConnectionSevered) {
			std::cout << "Disconnected, retrying in 5 seconds." << std::endl;
			sleep(5);
		} catch(Connection::ConnectionRefused) {
			std::cout << "Refused, retrying in 5 seconds." << std::endl;
			sleep(5);
		}		
	}
}
