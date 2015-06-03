#include "connection.h++"
#include "stream.h++"

using namespace streamcon;

Connection::Connection() throw()
{
	;
}



Connection::~Connection() throw()
{
	for(int i = 0;i < maxStreams;i++) {
		delete streams[i];
		streams[i] = NULL;
	}
}

void Connection::Loop() throw(ConnectionSevered)
{
	while(1) {
		LoopOnce();
	}
}

const std::string& Connection::GetAddress() const throw()
{
	return address;
}

int Connection::GetPort() const throw()
{
	return port;
}

void Connection::SetMaxStreams(int _maxStreams) throw()
{
	maxStreams = _maxStreams;
	streams.resize(maxStreams);
}

int Connection::GetMaxStreams() throw()
{
	return maxStreams;
}

Stream& Connection::AllocStream() throw(NoStreamsAvailable)
{
	RecursiveScopedLock lock(allocMutex);

	for(int i = 0;i < maxStreams;i++)
		if(!streams[i]) {
			return AllocStream(i);
			break;
		}
	THROW(NoStreamsAvailable);
}

Stream& Connection::AllocStream(int id) throw(StreamIdOccupied,
												  StreamIdExceeded)
{
	RecursiveScopedLock lock(allocMutex);

	if(streams[id])
		THROW(StreamIdOccupied);
	if(id >= maxStreams)
		THROW(StreamIdExceeded);
	streams[id] = new Stream(*this, id);
	return *(streams[id]);
}

void Connection::CloseStream(Stream& stream) throw(BadStreamProvided)
{
	RecursiveScopedLock lock(allocMutex);
	if(dynamic_cast<Connection *>(&stream.Owner()) != this)
		THROW(BadStreamProvided);
	streams[stream.GetId()] = NULL;
	delete &stream;
}

Stream& Connection::GetStreamById(int id) throw(SuchStreamNotAllocated,
													StreamIdExceeded)
{
	if(!streams[id]) {
		THROW(SuchStreamNotAllocated);
	}
	if(id >= maxStreams)
		THROW(StreamIdExceeded);
	return *(streams[id]);
}

Stream& Connection::operator[](int id) throw(SuchStreamNotAllocated,
												 StreamIdExceeded)
{
	return GetStreamById(id);
}
