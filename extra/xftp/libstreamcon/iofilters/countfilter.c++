#include "countfilter.h++"

using namespace streamcon;

CountFilter::CountFilter()
{
	Reset();
}

void CountFilter::WriteFilter(char *buffer, size_t size)
	throw(DataBuffer::Interrupted, Stream::StreamError)
{
	IOFilter::WriteFilter(buffer, size);
	counters.writeCounter += size;
}

int CountFilter::ReadFilter(char *buffer, size_t size)
	throw(DataBuffer::Interrupted, Stream::StreamError)
{
	int n = IOFilter::ReadFilter(buffer, size);
	counters.readCounter += n;
	return n;
}

void CountFilter::Reset()
{
	counters.readCounter = 0;
	counters.writeCounter = 0;
}

const CountFilter::Counters& CountFilter::GetCounters() const
{
	return counters;
}

