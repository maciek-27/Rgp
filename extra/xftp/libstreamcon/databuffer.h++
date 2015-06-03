#ifndef _DATABUFFER_H_
#define _DATABUFFER_H_

#include "commons.h++"
#include "throw.h++"
#include "threads.h++"

namespace streamcon {
	
	/*!
	  Thread-safe blocking data buffer.
	*/
	class DataBuffer {
	private:
		//! read-write mutex
		Mutex mutex;
		//! variable for blocking condition
		Condition cond;

		//! actual place where the data is hold
		char *bufferData;
		size_t size;
		//! amount of data inside
		size_t fill;

		//! index of buffer beginning in the \a buffer_data 
		int bufStart;

		/*!
		  set to true after destructor call or consent interruption.
		  Signals the blocking operations to end
		*/
		bool interrupt;
	public:
		__DE(BufferUnderrun, Exception);
		__DE(Interrupted, Exception);

		/*!
		  \param _size

		  Initialize the data buffer with a static size.
		*/
		DataBuffer(size_t _size);

		~DataBuffer();

		void Interrupt();

		/*!
		  \return size of the data buffer
		*/
		int Size();

		/*!
		  \return amount of data available in the buffer
		*/
		int Peek();

		/*!
		  \param buf destination
		  \param count amount of data to read

		  Fetch data from the buffer.
		  
		  \return amount of data read. This can be smaller if the available
		  amount of data was less than \a count
		*/
		int Read(char *buf, size_t count) throw(Interrupted);

		/*!
		  \param buf source
		  \param count amount of data to read
		  
		  Feed buffer with data.

		  \return amount of data read. This can be smaller if the available
		  amount of data was less than \a count
		*/
		int Write(char *buf, size_t count) throw(Interrupted,BufferUnderrun);
	};

}

#endif // _DATABUFFER_H_
