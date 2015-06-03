#ifndef __XFTP_Z__
#define __XFTP_Z__
#include<libxftp/xmlstream.h++>
#include <list>
namespace xftp { namespace stdfilter {

	/*!
	 * Default compression filter implemented as single function. compresses
	 * using zip algorithm, 
	 * 
	 * \note As a simple function, this is a filter without state
	 */
	size_t ZCompress(boost::scoped_array<XMLByte>& b,size_t size);
	
	/*!
	 * Inverted operation ZCompress
	 */
	size_t UnZCompress(boost::scoped_array<XMLByte>& b, size_t size);

	
	/*!
	 * Class designed to allow usage of many filters for single XMLStream.
	 */
	class FilterChain
	{
		//! internal filter list
		std::list<XMLStream::FilterFunc> filters;
	public:
		//! function call operator, matching XMLStream::FilterFun specs.
		size_t operator ()(boost::scoped_array<XMLByte>& b, size_t size);
		
		//! add operation. Operations are executed in the same order, they
		//! were added. 
		void AddFilter(XMLStream::FilterFunc f);
	};//!~ FilterChain
}}



#endif 
