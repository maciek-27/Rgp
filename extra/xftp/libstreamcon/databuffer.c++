#include "databuffer.h++"

using namespace streamcon;


DataBuffer::DataBuffer(size_t _size) : size(_size), fill(0),
									   bufStart(0), interrupt(false)
{
	bufferData = (char *)malloc(_size);
}

DataBuffer::~DataBuffer()
{
	Interrupt(); /* interrupt any possible blocking operations since after
					destructor there won't be any object to block in :)  */
	free(bufferData);
}

void DataBuffer::Interrupt()
{
	interrupt = true;
	
	cond.notify_all();
	{
		ScopedLock lock(mutex);
	}
}

int DataBuffer::Size()
{
	return size;
}

int DataBuffer::Peek()
{
	return fill;
}

int DataBuffer::Read(char *buf, size_t count) throw(Interrupted)
{
	ScopedLock lock(mutex);

//	std::cout << "Databuffer reaaad " << Peek() << std::endl;

	while(fill == 0) {
		cond.wait(lock); /* notify the writer of a possible change of
							condition */
		if(interrupt) {
			THROW(Interrupted);
		}
	}

	if(count > fill) // if there's not enough data, read less
		count = fill;
	
	if(size - bufStart >= count) { // possible to copy in one chunk
		memcpy(buf, bufferData + bufStart, count);
		bufStart += count;
	}
	else {
		size_t count1 = (size - bufStart);
		// copy first chunk
		memcpy(buf, bufferData + bufStart, count1);
		// copy second
		memcpy(buf + count1, bufferData, count - count1);
		bufStart = (bufStart + count) % size;
	}
	fill -= count;

	cond.notify_one();
	
	return count;
}

int DataBuffer::Write(char *buf, size_t count) throw(BufferUnderrun,
													 Interrupted)
{
	ScopedLock lock(mutex);

	int buf_end = (bufStart + fill) % size;;
//	if(fill + count > size) // temporarily don't allow for buffer overflow
//		THROW(BufferUnderrun); 
	while(fill + count > size) { // block until there's place in the buffer
//		printf("Buffer blocked\n");
		cond.wait(lock);
		if(interrupt) {
			THROW(Interrupted);
		}
	}
//	printf("Buffer unblocked\n");

	if(size - buf_end >= count) { // possible to copy in one chunk
		memcpy(bufferData + buf_end, buf, count);
	}
	else {
		size_t count1 = (size - buf_end);
		// copy first chunk
		memcpy(bufferData + buf_end, buf, count1);
		// copy second
		memcpy(bufferData, buf + count1, count - count1);
	}
	fill += count;

	if(fill) // notify the reader that there might have been a change
		cond.notify_one(); // in blocking condition

	return count;
}
