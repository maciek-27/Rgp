#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <map>
#include "libstreamcon/connection.h++"
#include <libxftp/servercontrol.h++>
#include <libxftp/xftpserver.h++>

namespace xftp {
	class ClientBase 
	{
	public:
		virtual ~ClientBase() {;}
		virtual ControlThread& CreateControl()
			throw(Connection::NoStreamsAvailable) = 0;
		virtual DataThread& CreateData(int streamid) 
			throw(Connection::NoStreamsAvailable,
				streamcon::Connection::StreamIdOccupied,
				  streamcon::Connection::StreamIdExceeded) = 0;
		virtual ControlThread& GetMainControl() = 0;
		virtual DataThread& GetExistingData(int streamid) = 0;
		virtual streamcon::Stream& operator[](int id) throw(
		streamcon::Connection::SuchStreamNotAllocated,
												 streamcon::Connection::StreamIdExceeded) = 0;
	};

	template <class _Control, class _Data>
	class Client : public ClientBase
	{
	private:
		streamcon::Connection &connection;
		typedef std::map<int, _Control*> ControlMap;
		ControlMap controlStreams;
		typedef std::map<int, _Data*> DataMap;
		DataMap dataStreams;
	public:
		Client(Connection& _connection) :
			connection(_connection)
			{
				controlStreams[0] = new _Control(*this,
												 connection[0]);
				controlStreams[0]->Start();
			}
		~Client()
			{
				for(typename ControlMap::iterator i = controlStreams.begin();
					i != controlStreams.end();++i) {
					delete i->second;
				}
				for(typename DataMap::iterator i = dataStreams.begin()
						;i != dataStreams.end();++i) {
					delete i->second;
				}

			}
		ControlThread& CreateControl()
			throw(Connection::NoStreamsAvailable)

			{
				Stream& stream = connection.AllocStream();
				controlStreams[stream.GetId()] =
					new _Control(*this, stream);
				return *controlStreams[stream.GetId()];
			}
		
		DataThread& CreateData(int streamid) 
			throw(Connection::NoStreamsAvailable,
				streamcon::Connection::StreamIdOccupied,
				streamcon::Connection::StreamIdExceeded)
			{
				Stream& stream = connection.AllocStream(streamid);
				dataStreams[streamid] =
					new _Data(*this, stream);
				return *dataStreams[stream.GetId()];
			}
		DataThread& GetExistingData(int streamid)
			{
				return *dataStreams[streamid];
			}
		
		ControlThread& GetMainControl() 
			{
				return *controlStreams[0];
			}
		Connection& GetConnection()
			{
				return connection;
			}
		streamcon::Stream& operator[](int id) throw(streamcon::Connection::SuchStreamNotAllocated,
												 streamcon::Connection::StreamIdExceeded)
		{
			return connection[id];
		}
	};
}
		
#endif // _CLIENT_H_
