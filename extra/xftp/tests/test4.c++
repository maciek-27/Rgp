#include "libstreamcon/sctpconnection.h++"
#include "libstreamcon/databuffer.h++"
#include "libstreamcon/stream.h++"
#include <sys/time.h>
#include "libstreamcon/sctpclient.h++"

using namespace streamcon;

struct message {
	char msg_data[1024];
};

class ControlThread : public Thread
{
public:
	Stream& stream;

	ControlThread(Stream& _stream) : stream(_stream) {;}

	void Execute(void *arg) {
		while(1) {
			struct message msg;
			fgets(msg.msg_data, sizeof(message), stdin);
			msg.msg_data[strlen(msg.msg_data) - 1] = 0;
			stream.SendData((char *)&msg, sizeof(message));
			printf("Sent message: %s\n", msg.msg_data);
			stream.GetData((char *)&msg, sizeof(message));
			printf("Recieved message: %s\n", msg.msg_data);
		}
	}
};

class TestClient : public SCTPClient
{
protected:
	ControlThread *thr;
	void OnConnect(Connection &conn) throw()
		{
			std::cout << "Client is connected!\n" << std::endl;
			thr = new ControlThread(conn[0]);
			thr->Start();
		}
	void OnDisconnect() throw()
		{
			delete thr;
			std::cout << "Client is disconnected!\n" << std::endl;
		}
};

int main()
{
	TestClient client;

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

	return 0;
}
