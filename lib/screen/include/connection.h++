#ifndef __CONNECTION_H__
#define __CONNECTION_H__
#include "keyboard.h++"

#include"fileno_hack.h++"

namespace Scr
{
	
	//!Intermediate between Scr::__ScreenConnection and std::istream
	class BufferedInput
	{
	private:
		static const Uint maxCharBufferSize = 32;
		Uint currentCharBufferSize;
		Uint currentCharBufferIndex;
		/*!
		  read some bytes from input, then transform em to keyboard
		  events (no direct access to istream outside of
		  ProcessConnection, where readsome() performed - to ensure )
		*/
		char charBuffer[maxCharBufferSize];
		/*!
		  input stream
		*/
		std::istream & stream;
		/*! 
		 * Blocking buffering function
		 */
		void ForceBuffer()throw();
	public:
		__DE(BufferEmpty,Exception);
		/*!
		 * \return true if input device is ready to transmit data
		 */
		bool KbHit()throw();
		/*!
		  \param _stream stream to be contained
		 */
		explicit BufferedInput(std::istream & _stream)throw()
			:
			 currentCharBufferSize(0),
			 currentCharBufferIndex(0),
			 stream(_stream)
			 {;}

		/*!
		  Save some characters in internal buffer (it is not invoked
		  automatically when Get() is called and buffer is empty.
		 */
		void Buffer()throw()
			{
				currentCharBufferIndex=0;
				currentCharBufferSize=
					stream.readsome(
						static_cast<char*>(&charBuffer[0]),
						static_cast<std::streamsize>(maxCharBufferSize));
				if (currentCharBufferSize == 0)// no text read, while it
												// has to be read.
					ForceBuffer();
			}
		
		/*!
		 * \return true if any character is availble in buffer
		 */
		bool HasBufferedText()throw()
			{
				if (currentCharBufferIndex <  currentCharBufferSize )
					return true;
				if (currentCharBufferIndex == currentCharBufferSize )
					return false;
				THROW(FatalException);// kill app (assume that it is impossible)
			}

		/*!
		 * \return first availble character w/o moving pointer
		 */
		unsigned char Peek()throw(BufferEmpty)
			{
				EASSERT(currentCharBufferIndex < currentCharBufferSize,
						BufferEmpty);
				if (currentCharBufferIndex == currentCharBufferSize)
					Buffer();
//				std::RexIOLog(LogLevelModerate) <<  "Peek: " <<
//					static_cast<int>(charBuffer[currentCharBufferIndex]) <<'\n';
				return charBuffer[currentCharBufferIndex];
			};
		
		/*!
		 * get character
		 */
		unsigned char Get()throw(BufferEmpty)
			{
				EASSERT(currentCharBufferIndex < currentCharBufferSize,
						BufferEmpty);
				if (currentCharBufferIndex == currentCharBufferSize)
					Buffer();
				std::RexIOLog(LogLevelModerate) <<  "Get: " <<
					static_cast<int>(charBuffer[currentCharBufferIndex]) <<'\n';
				return charBuffer[currentCharBufferIndex++];
			};
			
		/*!
		 * \return character to buffer
		 */
		void UnGet()throw()
			{
				EASSERT(currentCharBufferIndex > 0,
						BufferEmpty);
				currentCharBufferIndex--;
			};
			
		/*!
		 * \return UNIX* file descriptor for associated stream.
		 *
		 * * UNIX is registered trademark of AT&T and OpenGroup.
		 */	
		int FD()throw()
			{
				return fileno_hack(stream);
			}
		
		/*!
		 * \return associated C++ stream.
		 */
		std::istream & Stream()throw()
			{
				return stream;
			}
	};
	
    //! \brief internal class which is base for all connection-specific
    //! implementations of screen (multiple-inheritance case)
    /*!
      It represents internal interface between Scr::Connection and
      Scr::Screen classes. 
	*/
    class __ScreenConnection
    {
    protected:
		/*!
		  ProcessConnection will return this value upon successful finish
		*/
		int exitcode;
		/*!
		  is application running? does it have to stop?
		  (ExitConnection() is called by Connection::Exit(int), sets
		  exit code and breaks main loop performed in ProcessConnection)
		*/
		Connection & connection;

		/*!
		  break main loop if set to false
		 */
		bool active;

		BufferedInput input;
		/*!
		  get key esc-code from std input stream. decode it into form
		  from keyboard.h++
		*/
		virtual Key DecodeKeyPressed();

	public:
		/*!

		  \param _input input stream (used to capture some of events,
		  in particular keyboard events).	  
		  \param _connection newely estabilished connection to serve
		*/
		__ScreenConnection(Connection & _connection,std::istream & _input)throw();
		/*!
	  
		  \return value of exitcode, as it was in the moment of
		  connection termination if successful.
	  	  
		  Initialize, conduct and end connection in way apropriate to
		  connection type, operating system etc. Inform
		  Scr::Connection object supplied about all captured events

		  \note as function (for design reasons) lacks exception-set
		  specification, it may throw any exceptions, but it is
		  recommended, that only exceptions typical to
		  Scr::Connection::Start() will be thrown.
	  
		*/
		virtual int  ProcessConnection() =0;

		/*!
		  \param _code exit code return from ProcessConnection after
		  successfully finished connection
	  
		  Force stopping connection as soon as possible
		  \note as function (for design reasons) lacks exception-set
		  specification, it may throw any exceptions, but it is
		  recommended, that only exceptions typical to
		  Scr::Connection::Exit() will be thrown.
		*/
		virtual void ExitConnection(int _code);
		virtual ~__ScreenConnection()throw();
    };
}
#endif
