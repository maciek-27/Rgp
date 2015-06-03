#include <libstreamcon/sctpserver.h++>
#include <libstreamcon/databuffer.h++>
#include <libstreamcon/stream.h++>
#include <libstreamcon/iofilters/tlsfilter.h++>
#include <sys/time.h>

using namespace streamcon;


#define KEYFILE "server.pem"
#define PASSWORD "password"
#define DHFILE "dh1024.pem"
#define CA_LIST "root.pem"

#define CHUNK_SIZE 4096

bool bUseTls;

class DataThread : public StreamThread
{
public:
	int counter;

	DataThread(Stream& _stream) : StreamThread(_stream) {;}

	void Run() {
		char buffer[4096];
		counter = 0;

		if(bUseTls) {
			TLSFilter* tls = new TLSFilter;
			stream.PushFilter(tls);
			tls->InitializeCTX(KEYFILE, DHFILE, PASSWORD, CA_LIST);
			tls->AcceptTLS();
			printf("TLS Accepted!\n");
		}

		while(1) {
			stream.SendData(buffer, 4096);
			counter += 4096;
			if(!(counter % (4096*4096))) {
				printf("%i Counter %iMB\n", stream.GetId(),
					   counter/1024/1024);
			}
		}
	}
};

class TestServer : public SCTPServer
{
private:
	std::map<Connection *,  DataThread**> conn2threads;
	int nthreads;
public:
	TestServer(int _nthreads) : nthreads(_nthreads)
		{
		}
	~TestServer() throw()
		{
			printf("End of server\n");
			for(std::map<Connection*, DataThread**>::iterator i = conn2threads.begin();
				i != conn2threads.end();i++) {
				for(int j = 0;j<nthreads;j++)
					delete (i->second)[j];
				free(*(i->second));
			}
		}
	void OnNewConnection(Connection& connection) throw()
		{
			std::cout << "OnNewConnection" << std::endl;
			DataThread **threads = (DataThread**)malloc(sizeof(DataThread*)*nthreads);
			conn2threads[&connection] = threads;

			threads[0] = new DataThread(bUseTls?
										connection.AllocStream(1):
										connection.AllocStream(1));
			for(int i = 1;i<nthreads;i++)
				threads[i] = new DataThread(bUseTls?
											connection.AllocStream(i+1):
											connection.AllocStream(i+1));
			for(int i = 0;i<nthreads;i++)
				threads[i]->Start();

		}
	void OnCloseConnection(Connection& connection) throw()
		{
			std::cout << "Disconnected??" << std::endl;

			DataThread **threads = conn2threads[&connection];
			conn2threads.erase(&connection);
			for(int i = 0;i<nthreads;i++) {
				std::cout << i << " recieved " << threads[i]->counter/1024
						  << "KBs" << std::endl;

				delete threads[i];
				printf("Post delete thread\n");
			}
			free(threads);

			std::cout << "Client is disconnected!\n" << std::endl;
		}
};

TestServer *serv;

int main(int argc, char **argv)
{
	int nstreams = 1;
	if(argc>1) {
		if(argv[1][0] == 's') {
			bUseTls = true;
			argv[1]++; // omit this char
		}
		if(argv[1][0] != 0) {
			nstreams = atoi(argv[1]);
		}
	}
	printf("Starting client with %i cuncurrent %sstreams.\n", nstreams, bUseTls?"TLS ":"");
	TestServer serv(nstreams);
	serv.Listen("");
	
	return 0;
}
