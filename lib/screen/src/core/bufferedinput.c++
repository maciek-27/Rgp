#include <iostream>
#include <string>
#include"bufferedinput.h++"
#include <sys/select.h>
#include <sys/ioctl.h>
#include <unistd.h>
using namespace Scr;
using namespace std;
#include <cassert>
bool BufferedInput::KbHit()const throw()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    FD_ZERO(&fds);
    FD_SET(FD(), &fds); //STDIN_FILENO is 0
    return select(FD()+1, &fds, NULL, NULL, &tv);
}

void BufferedInput::ForceBuffer()const throw()
{
	while(!KbHit());
	do
	{
		if (currentCharBufferSize==maxCharBufferSize)
		{// yet something to read, but nowwhere to store it
			filledToCapacity=true;
			return;
		}
		charBuffer[currentCharBufferSize++]=stream.get();
	}
	while (KbHit());
	RexIOLog(LogLevelModerate) << "ForceBuffer resulted in reading "
		 << currentCharBufferSize -1  << "characters:"
		 << DebugInfo()
		 << endl;
	
}

void BufferedInput::DoBuffer()const throw()
{
	filledToCapacity=false;
	//to allow at least one UnGet any time
	charBuffer[0]=charBuffer[currentCharBufferSize-1];
	currentCharBufferIndex=1;
	if (q!=NULL)
	{
		if (q->empty())
		{
			q=NULL;
		}
		else
		{
			filledToCapacity=true;
			charBuffer[1]=q->front();
			q->pop();
			currentCharBufferSize=2;
			return;
		}
	}
	currentCharBufferSize=
		stream.readsome(
			static_cast<char*>(&charBuffer[1]),
			static_cast<std::streamsize>(maxCharBufferSize-1));
	currentCharBufferSize++;
	if (currentCharBufferSize==maxCharBufferSize)
		filledToCapacity=true;
	if (currentCharBufferSize == 1)// no text read, while it
		// has to be read.
		ForceBuffer();
}

string BufferedInput::String()throw()
{
	return string(charBuffer,currentCharBufferSize);	
}

string BufferedInput::DebugInfo()throw()
{
	return 
	(static_cast<const BufferedInput &>(*this)).DebugInfo();
}
const string BufferedInput::DebugInfo()const throw()
{
	
	std::stringstream dss;
	for (Uint i = 1; i< currentCharBufferSize; i++)
		dss << i<<':'
			<< ( (charBuffer[i]>31 && charBuffer[i]<0x7f) ?
				 charBuffer[i] : '?' ) << '('
			<< static_cast<int>(charBuffer[i]) << ")  " ;
	return dss.str();
}
