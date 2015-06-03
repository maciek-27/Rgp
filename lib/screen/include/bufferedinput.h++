#ifndef __BUFFERED_INPUT_H__
#define __BUFFERED_INPUT_H__
#include "keyboard.h++"

#include"fileno_hack.h++"
#include"throw.h++"
#include"commons.h++"
#include<queue>
#include"screen.h++"

namespace Scr
{
	
	//!Intermediate between Scr::__ScreenConnection and std::istream
	class BufferedInput
	{
	private:
		static const Uint maxCharBufferSize = 32;
		/*!
		  if after last read buffer was filled while still something
		  on input was availble
		 */
		mutable bool filledToCapacity;
		/*!
		  number of characters staying in buffer after last read.
		 */
		mutable Uint currentCharBufferSize;

		/*!
		  idx of current character
		 */
		mutable Uint currentCharBufferIndex;
		/*!
		  read some bytes from input, then transform em to keyboard
		  events (no direct access to istream outside of
		  ProcessConnection, where readsome() performed - to ensure )
		*/
		mutable char charBuffer[maxCharBufferSize];
		/*!
		  input stream
		*/
		std::istream & stream;

 		mutable std::queue<char> * q;
		
		/*!
		  std::istream::readsome returned 0, while something needs to
		  be read.
		 */
		void ForceBuffer()const throw();

		void DoBuffer() const throw();
	public:
		__DE(BufferEmpty,Exception);

		bool KbHit()const throw();
		/*!
		  \param _stream stream to be contained
		 */
		explicit BufferedInput(std::istream & _stream)throw()
			:filledToCapacity(false),
			 currentCharBufferSize(1),
			 currentCharBufferIndex(1),
			 stream(_stream),
			 q(NULL)
			 {;}

		/*!
		  Save some characters in internal buffer (it is not invoked
		  automatically when Get() is called and buffer is empty.
		 */
		void Buffer()throw() {DoBuffer();}

		/*!
		  Inquiry if object has some buffered text, or at least can
		  make this text availble instantly
		 */
		bool HasBufferedText()const throw()
			{				
				if (currentCharBufferIndex <  currentCharBufferSize )
					return true;
				if (currentCharBufferIndex == currentCharBufferSize )
				{
					if (filledToCapacity)
					{
						DoBuffer();
						return true;
					}
					else
						return false;
				}
				THROW(FatalException);// kill app (assume that it is impossible)
			}

		/*!
		  Peek if it won't block app
		 */
		unsigned char TryPeek()const throw(BufferEmpty)
			{
				EASSERT(HasBufferedText(),
						BufferEmpty);
				RexIOLog(LogLevelModerate) <<  "TryPeek: " <<
					static_cast<int>(charBuffer[currentCharBufferIndex]) <<'\n';
				return charBuffer[currentCharBufferIndex];
			};

		/*!
		  Get if it won't block app (throw exception otherwise)
		 */
		unsigned char TryGet()throw(BufferEmpty)
			{
				EASSERT(HasBufferedText(),
						BufferEmpty);
				RexIOLog(LogLevelModerate) <<  "TryGet: " <<
					static_cast<int>(charBuffer[currentCharBufferIndex]) <<'\n';
				return charBuffer[currentCharBufferIndex++];
			};

		/*!
		  Current character. The same will be availble after call to
		  this func.
		 */
		unsigned char Peek() const throw()
			{
				EASSERT(currentCharBufferIndex <= currentCharBufferSize,
						FatalException);
				if (currentCharBufferIndex == currentCharBufferSize)
					DoBuffer();
				RexIOLog(LogLevelModerate) <<  "Peek: " <<
					static_cast<int>(charBuffer[currentCharBufferIndex]) <<'\n';
				return charBuffer[currentCharBufferIndex];
			};

		/*!
		  Get character from stream
		 */
		unsigned char Get()throw()
			{
				EASSERT(currentCharBufferIndex <= currentCharBufferSize,
						FatalException);
				if (currentCharBufferIndex == currentCharBufferSize)
					Buffer();
				RexIOLog(LogLevelModerate) <<  "Get: " <<
					static_cast<int>(charBuffer[currentCharBufferIndex]) <<'\n';
				return charBuffer[currentCharBufferIndex++];
			};

		/*!
		  UnGet().

		  \note that this function may fail if called just after
		  buffering, or called too frequently: only one successful
		  UnGet per one Get is guaranteed.

		  \exception Scr::BufferedString::BufferEmpty is thrown when
		  too many UnGet's are processed oneafter another
		 */
		void UnGet()throw(BufferEmpty)
			{
				EASSERT(currentCharBufferIndex > 0,
						BufferEmpty);
				currentCharBufferIndex--;
			};

		/*!
		  Unix style file descriptor
		 */
		int FD()const throw()
			{
				return fileno_hack(stream);
			}

		/*!
		  direct access to underlying std::stream - const version
		 */
		const std::istream & Stream()const throw()
			{
				return stream;
			}
		/*!
		  direct access to underlying std::stream
		 */
		std::istream & Stream()throw()
			{
				return stream;
			}
		/*!
		  contents of internal buffer as string
		 */
		std::string String()throw();

		/*!
		  more than info returned by String(): function created
		  specifically for debugging/logging purposes
		*/
		std::string DebugInfo()throw();

		/*!
		  more than info returned by String(): function created
		  specifically for debugging/logging purposes
		*/
		const std::string DebugInfo()const throw();

		void SyncWithQueue(std::queue<char> & _q)throw()
			{
				q=&_q;
			}
	};

	
}
#endif
