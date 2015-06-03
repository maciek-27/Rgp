#ifndef ___XFTPCLIENT__REAL_APP__PRINTXML_
#define ___XFTPCLIENT__REAL_APP__PRINTXML_
#include <boost/scoped_array.hpp>
#include <xercesc/util/XMLChar.hpp>
namespace xftpclient
{
	/*!
	 * XMLStream filter, that does not modify text, but prints it to stdout
	 *
	 */
	class PrintXML
	{
	private :
		int c;
		static bool doPrintXML;
	public :
		/*!
		 * Turns off printing (filter will just do nothing)
		 */
		static void Disable() {doPrintXML=false;}
		/*!
		 * Turns on printing
		 */
		static void Enable() {doPrintXML=true;}
		
		/*!
		 * default constructor
		 */
		explicit PrintXML (int color) : c (color)
		{}
		
		/*!
		 * Real filter function
		 */
		size_t operator ()(boost::scoped_array<XMLByte>& b, size_t s);
	};
}
#endif

