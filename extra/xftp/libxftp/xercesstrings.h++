#ifndef XERCES_STRINGS_HPP_INCLUDED
#define XERCES_STRINGS_HPP_INCLUDED

#include <string>
#include <boost/scoped_array.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#define XercesSsie fromNative // little revenge on Xerces

typedef std::basic_string<XMLCh> XercesString;

// Converts from a narrow-character string to a wide-character string.
inline XercesString fromNative(const char* str)
{
	boost::scoped_array<XMLCh> ptr(xercesc::XMLString::transcode(str));
    return XercesString(ptr.get( ));
}

// Converts from a narrow-character string to a wide-charactr string.
inline XercesString fromNative(const std::string& str)
{
    return fromNative(str.c_str( ));
}

// Converts from a wide-character string to a narrow-character string.
inline std::string toNative(const XMLCh* xmlChars)
{
	unsigned int srcCount = xercesc::XMLString::stringLen(xmlChars);
	boost::scoped_array<XMLByte> xmlBytes(new XMLByte[srcCount * 2]);
// pessimistic buffer size but ensured immediate conversion
	
	xercesc::XMLTransService::Codes failReason;
	xercesc::XMLTranscoder* utf8Transcoder =
		xercesc::XMLPlatformUtils::
		fgTransService->makeNewTranscoderFor("UTF-8", failReason, 1024);
	
	unsigned int eaten;
	unsigned int transcoded =
		utf8Transcoder->transcodeTo(xmlChars, srcCount, xmlBytes.get(), srcCount*2, eaten,
									xercesc::XMLTranscoder::UnRep_RepChar);
	xmlBytes.get() [transcoded] = '\0'; // terminate the string since Xerces doesn't do it
	
	return std::string(reinterpret_cast<char *>(xmlBytes.get()));
}

// Converts from a wide-character string to a narrow-character string.
inline std::string toNative(const XercesString& str)
{
    return toNative(str.c_str( ));
}

#endif // #ifndef XERCES_STRINGS_HPP_INCLUDED
