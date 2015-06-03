#include "libstreamcon/sctpserver.h++"
#include "libstreamcon/databuffer.h++"
#include "libstreamcon/stream.h++"
#include <sys/time.h>

using namespace streamcon;

struct message {
	char msg_data[1024];
};

class ControlThread : public StreamThread
{
public:
	ControlThread(Stream& _stream) : StreamThread(_stream) {;}
protected:
	virtual void Run() {
		int counter = 0;
		while(1) {
			struct message msg;
			stream.GetData((char *)&msg, sizeof(message));
			printf("Recieved message: %s\n", msg.msg_data);
			snprintf(msg.msg_data,
					 sizeof(msg.msg_data), "Foo nr. %i", counter++);
			stream.SendData((char *)&msg, sizeof(message));
			printf("Sent message: %s\n", msg.msg_data);
		}
	}
};

class TestServer : public SCTPServer
{
public:
	std::map<Connection*, ControlThread*> threads;

	void OnNewConnection(Connection& connection) throw()
		{
			std::cout << "New client has connected" << std::endl;
			
			threads[&connection] = new ControlThread(connection[0]);
			threads[&connection]->Start();
		}
	void OnCloseConnection(Connection& connection) throw()
		{
			delete threads[&connection];

			std::cout << "Client has disconnected!\n" << std::endl;
		}
};


int main(int argc, char **argv)
{
	TestServer serv;
	serv.Listen(argc>=2?argv[1]:"");

	return 0;
}
