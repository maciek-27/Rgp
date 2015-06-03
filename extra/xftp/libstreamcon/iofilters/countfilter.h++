#ifndef _COUNTFILTER_H_
#define _COUNTFILTER_H_

#include <libstreamcon/stream.h++>

namespace streamcon {

	//! Simple data counting filter.
	class CountFilter : public Stream::IOFilter{
		//! Counter data
		struct Counters {
			//! read counter
			size_t readCounter;
			//! write counter
			size_t writeCounter;
		} counters;
		void WriteFilter(char *buffer, size_t size)
			throw(DataBuffer::Interrupted, Stream::StreamError);
		int ReadFilter(char *buffer, size_t size)
			throw(DataBuffer::Interrupted, Stream::StreamError);
	public:
		CountFilter();
		/*!
		  Reset counters.
		 */
		void Reset();
		/*!
		  \return counter data
		  Get counter data.
		 */
		const Counters& GetCounters() const;
	};	
}
#endif // _COUNTFILTER_H_
		
