#ifndef __XFTP__STREAM_H__
#define __XFTP__STREAM_H__
//#include "throw.h++"
#include <cstring>
#include "databuffer.h++"
#include "threads.h++"

namespace streamcon {

	class Connection;
	class IOFilter;

	/*!
	  Class representing one separate logical data stream.
	*/
	class Stream {
	public:
		__DE(StreamError, Exception);

		/*!
		  Input/Output filter for data streams. Can be used to implement
		  security / compression / statistics.
		*/
		class IOFilter {
			IOFilter *prev, *next;
			Stream *owner;
		protected:
			//! Simple constructor.
			IOFilter();
			/*!
			  \param buffer data
			  \param size buffer size
			  
			  Fetch incoming data. Overloaded functions should pass
			  the modified data into the next filter in the filter chain
			  using the base class method WriteFilter.
			 */
			virtual void WriteFilter(char *buffer, size_t size)
				throw(DataBuffer::Interrupted, StreamError) = 0;
			/*!
			  \param buffer data
			  \param size buffer size
			  \return amount of data read
			  
			  Overloaded functions should pass the call to the 
			  next filter chains using the base class method ReadFilter
			  and optionally modify it.
			 */
			virtual int ReadFilter(char *buffer, size_t size)
				throw(DataBuffer::Interrupted, StreamError) = 0;
			virtual ~IOFilter();
			friend class Stream;
		};


	private:
		//! underlying connection
		Connection &owner;
		//! stream id
		int id;

		//! Default filter used interfacing with Stream's connection
		class DefaultFilter : public IOFilter{
			void WriteFilter(char *buffer, size_t size)
				throw(DataBuffer::Interrupted, Stream::StreamError);
			int ReadFilter(char *buffer, size_t size)
				throw(DataBuffer::Interrupted, Stream::StreamError);
		} defaultFilter;

		//! filter stack
		std::list<Stream::IOFilter*> filters;		
	protected:
		/*!
		  \param _owner
		  \param _id id to assign
		*/
		Stream(Connection& _owner, int _id) throw();
	public:

		/*!
		  \return stream id
		*/
		int GetId() throw();
		/*!
		  Interrupt any blocking operations in underlying data buffers.
		*/
		virtual void Interrupt() throw();

		/*!
		  \param buffer
		  \param size
		  
		  \exception DataBuffer::Interrupted
		  \exception StreamError
		*/
		virtual void SendData(char *buffer, size_t size)
			throw(DataBuffer::Interrupted, StreamError);

		/*!
		  \param buffer
		  \param size

		  \exception DataBuffer:Interrupted
		  \exception StreamError
		*/
		virtual int GetData(char *buffer, size_t size)
			throw(DataBuffer::Interrupted, StreamError);

		/*!
		  \return reference to the underlying connection.
		*/
		Connection& Owner() throw();

		/*!
		  \param filter new filter
		  \return the param argument
		  
		  Add a new filter to the filter stack.
		*/
		IOFilter* PushFilter(IOFilter* filter) throw();
		/*!
		  \return filter from the top of the stack
		  
		  Remove a filter from the filter stack.
		*/
		IOFilter* PopFilter() throw();

		virtual ~Stream(){;};

		friend class Connection;
		friend class DefaultFilter;
	};
	
	/*!
	  Special thread class that is designed to govern stream
	  blocking operations. Its purpose is to interrupt any blocking
	  operations if the underlying stream stops being valid
	  or invokes the interruption itself upon the destructor call.
	  It's the only way to ensure thread proper destruction since
	  normally the blocking operations would not allow the thread
	  function to quit.
	*/
	class StreamThread : public Thread {
	private:
		void Execute(void *arg) {
			try {
				Run();
			} catch(DataBuffer::Interrupted) {
				return;
			}		   
		}
	protected:
		//! stream attached to a thread
		Stream& stream;	   
		/*!
		  Main thread function. Override it in derivating thread.
		*/
		virtual void Run() = 0;
	public:
		/*!
		  \param _stream stream which shall be handled by this thread
		*/
		StreamThread(Stream& _stream) : stream(_stream) {;};
		virtual ~StreamThread() {
			stream.Interrupt();
		}
	};
	
}
#endif
