#ifndef _XFTPCOMMONS_H_
#define _XFTPCOMMONS_H_

#include "libstreamcon/stream.h++"
#include "xmlstream.h++"



namespace xftp {
//	namespace {
	//}
	extern streamcon::Mutex opendir_Mutex;
	inline std::string toString(int num)
	{
		char buf[10];
		snprintf(buf, 10, "%i", num);
		return std::string(buf);
	}
	inline std::string toString(unsigned int num)
	{
		char buf[10];
		snprintf(buf, 10, "%u", num);
		return std::string(buf);
	}
	inline std::string stringError(int _errno)
	{
		//char buf[1024];
		//strerror_r(_errno, buf, 1024);
		return std::string(strerror(_errno));
	}

	class ClientBase;

	class ManagedStream : public streamcon::StreamThread
	{
	private:
		ClientBase& client;
	protected:
		ManagedStream(ClientBase& _client,
					  streamcon::Stream& _stream) :
			StreamThread(_stream), client(_client)
													
			{ ; }
	public:
		ClientBase& GetClient() {
			return client;
		};
	};
	

	class ClientBase;
	
	class ControlThread : public ManagedStream
	{
	public:
		XMLStream xmlStream;
	protected:
		ControlThread(ClientBase& client, streamcon::Stream& _stream) :
			ManagedStream(client, _stream),   
			xmlStream(&_stream)
			{ ; }
		
		virtual void Run() = 0;
	};
	
	class DataThread : public ManagedStream
	{			 
	protected:  
		DataThread(ClientBase& client, streamcon::Stream& _stream):
			ManagedStream(client, _stream)
			{ ; }
		virtual void Run() = 0;
	};
}

#endif // _XFTPCOMMONS_H_
