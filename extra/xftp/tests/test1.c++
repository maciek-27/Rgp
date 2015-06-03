#include "libstreamcon/sctpconnection.h++"
#include "libstreamcon/databuffer.h++"

using namespace streamcon;

DataBuffer buf(1000000);
Mutex mutex;

#define CHUNK_SIZE 1024

unsigned int writecounter, readcounter;

class WriteThread : public Thread
{
	char data[CHUNK_SIZE];
	
	void Execute(void* arg) {
		while(1) {
			buf.Write(data, CHUNK_SIZE);
			writecounter += 1024;
		}		
	}
};

class ReadThread : public Thread
{
	char data[CHUNK_SIZE];

	void Execute(void* arg) {
		while(1) {
			int n = buf.Read(data, CHUNK_SIZE);
			readcounter += n;
		}
	}
};

int main()
{
	ReadThread r;
	WriteThread w;
	r.Start();
	w.Start();
	sleep(4);
	printf("Speed r: %iMB/s w: %iMB/s\n",
		   readcounter/4/1024/1024, writecounter/4/1024/1024);

	return 0;
}
