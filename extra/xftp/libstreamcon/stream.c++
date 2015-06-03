#include <libstreamcon/stream.h++>
#include <libstreamcon/iofilters/countfilter.h++>
#include <libstreamcon/connection.h++>

using namespace streamcon;

Stream::Stream(Connection& _owner, int _id) throw()
	: owner(_owner), id(_id)
{
	defaultFilter.owner = this;
}

int Stream::GetId() throw()
{
	return id;
}

void Stream::SendData(char *buffer, size_t size)
	throw(DataBuffer::Interrupted, StreamError)
{
	if(filters.empty()) // no filters, don't bother with the chain
		owner.SendData(id, buffer, size);
	else
		return filters.back()->WriteFilter(buffer, size);
}

int Stream::GetData(char *buffer, size_t size)
	throw(DataBuffer::Interrupted, StreamError)
{
	if(filters.empty()) // no filters, don't bother with the chain
		return owner.GetData(id, buffer, size);
	else
		return filters.front()->ReadFilter(buffer, size);
}

void Stream::Interrupt() throw()
{
	owner.Interrupt(id);
}

Connection& Stream::Owner() throw()
{
	return owner;
}

Stream::IOFilter::IOFilter() : prev(NULL), next(NULL), owner(NULL)
{
	;
}

Stream::IOFilter::~IOFilter()
{
	;
}

void Stream::IOFilter::WriteFilter(char *buffer, size_t size)
	throw(DataBuffer::Interrupted, Stream::StreamError)
{
	prev->WriteFilter(buffer, size);
}

int Stream::IOFilter::ReadFilter(char *buffer, size_t size)
	throw(DataBuffer::Interrupted, Stream::StreamError)
{
	return next->ReadFilter(buffer, size);
}

void Stream::DefaultFilter::WriteFilter(char *buffer, size_t size)
	throw(DataBuffer::Interrupted, Stream::StreamError)
{
	owner->owner.SendData(owner->id, buffer, size);
}

int Stream::DefaultFilter::ReadFilter(char *buffer, size_t size)
	throw(DataBuffer::Interrupted, Stream::StreamError)
{
	return owner->owner.GetData(owner->id, buffer, size);
}

Stream::IOFilter* Stream::PushFilter(Stream::IOFilter* filter) throw()
{
	if(filters.empty())
		filter->prev = &defaultFilter;
	else  {
		filter->prev = (filters.back());
		filters.back()->next = filter;
	}
	filter->next = &defaultFilter;
	filter->owner = this;
	filters.push_back(filter);
	return filter;
}

Stream::IOFilter* Stream::PopFilter() throw()
{
	IOFilter* tmp =  *(--filters.end());
	tmp->prev = NULL;
	tmp->next = NULL;
	tmp->owner = NULL;
	filters.pop_back();
	filters.back()->next = &defaultFilter;
	return tmp;
}
