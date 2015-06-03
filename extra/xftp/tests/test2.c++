#include <libstreamcon/sctpconnection.h++>
#include <libstreamcon/databuffer.h++>
#include <libstreamcon/stream.h++>
#include <sys/time.h>
#include <libstreamcon/sctpclient.h++>
#include <libstreamcon/iofilters/tlsfilter.h++>

using namespace streamcon;

Mutex mutex;

#define KEYFILE "client.pem"
#define PASSWORD "password"
#define CA_LIST "root.pem"

#define CHUNK_SIZE 4096

unsigned long gettv()
{
	struct timeval tvstart;
	gettimeofday(&tvstart,0);
	return tvstart.tv_sec;
}

bool bUseTls;

class DataThread : public StreamThread
{
public:
	DataThread(Stream& _stream) : StreamThread(_stream) {;}
	unsigned long size;

	void Run() {
		char buffer[CHUNK_SIZE];

		if(bUseTls) {
			TLSFilter* tls = new TLSFilter;
			stream.PushFilter(tls);
			tls->InitializeCTX(KEYFILE, PASSWORD, CA_LIST);
			tls->StartTLS();
			printf("TLS Started!\n");
		}

		printf("Thread started\n");

		unsigned long t_start = gettv();
		unsigned long t_last = t_start;
		unsigned long old_size = 0;
		float speed = 1.0;
		while(1) {
			size += stream.GetData(buffer, CHUNK_SIZE);

			if(size - old_size > (unsigned long)speed && gettv() > t_last) {
				speed=(size)/(gettv()-t_start);;
				old_size = size;
				t_last = gettv();

				printf("%i Counter %luMB - %.2fMB/s\n", stream.GetId(),
					   size/1024/1024, speed/1024/1024);
			}
		}
	}
};

class TestClient : public SCTPClient
{
public:
	DataThread** threads;
	int nthreads;
	TestClient(int _nthreads) : nthreads(_nthreads){
		threads = (DataThread**)malloc(sizeof(DataThread*)*nthreads);
	}
	~TestClient() {
		for(int i = 0;i< nthreads;i++)
			delete threads[i];
		free(threads);
	}
	void OnConnect(Connection& conn) throw()
		{
			std::cout << "Client is connected!\n" << std::endl;
			threads[0] = new DataThread(bUseTls?
											conn.AllocStream(1):
											conn.AllocStream(1));
			for(int i = 1;i< nthreads;i++) {
				threads[i] = new DataThread(bUseTls?
											conn.AllocStream(i+1):
											conn.AllocStream(i+1));
			}
			for(int i = 0;i<nthreads;i++) {
				threads[i]->Start();
			}

		}
	void OnDisconnect() throw()
		{
			std::cout << "Disconnected??" << std::endl;
			for(int i = 0;i<5;i++) {
				std::cout << i << " recieved " << threads[i]->size/1024
						  << "KBs" << std::endl;

				delete threads[i];
			}

			std::cout << "Client is disconnected!\n" << std::endl;
		}
};

int main(int argc, char** argv)
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
	TestClient client(nstreams);

	while(1) {
		try {
			client.Connect(argc>2?argv[2]:"127.0.0.1", defaultPort, defaultStreams);
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
