#ifndef __UTF_8_H__
#define __UTF_8_H__
namespace Scr
{
	/*!
	  \param pstr pointer to NULL-terminated c-style string.
	  \return RAW UNICODE value of utf8 encoded first character of
	  string supplied.

	  if length of u8 code is greater than 1 byte, pstr is moved
	  by this length-1 forward.
	  
	  \exception Scr::Screen::InvalidFirstByte is
	  thrown when **pstr (or pstr[0][0]) does not match 1-byte,
	  2-byte, 3-byte nor 4-byte UTF-8 encoding pattern for first
	  byte.

	  \exception Scr::Screen::OverlongUTF8Encoding is
	  thrown when numeric value of result would fit in smaller
	  number of bytes with correct UTF-8.
	  
	  \exception Scr::Screen::InvalidTrailingByte is
	  thrown if second or maybe third or fourth byte does not
	  match template (exactly (c[x]&0xC0)!=0x80)

	  \note if compiled without -DDO_VALIDATE_UTF_8_OUTPUT, none
	  of theese exceptions is thrown, and even none of theese
	  error conditions are checked (code assumes, that they never happen)
	  \sa RFC 3629
	  \latexonly \index{RFC, reference to!3629}\endlatexonly
	*/
	wchar_t DecodeUTF8(const char ** pstr)
		throw(Screen::InvalidUTF8); // returns RAW UNICODE
	// value of utf8 encoded character
	// if length of u8 code is greater than 1 byte, pstr is moved
	// by this length-1 forward.

	/*!
	  \param c character to encode
	  \param o reference to output stream
	  
	  Print c directly to o in UTF8 encoded form
	*/
	void EncodeUTF8(std::ostream &o,Uint c)throw(); // encode UNICODE character
                                          	// passed and write it to output.				

	//!\param s UTF-8 string
	//!
	//! Compute number of bytes in UTF-8 encoding of the FIRST
	//! character of  UTF-8 string.
	//!\note function assumes, that string is correct. No validation
	//!or range checking is performed
	Uint CharLengthUTF8(const char * s)
		throw(Screen::InvalidUTF8);

	//!\param s UTF-8 string
	//!
	//! Compute length of null-terminated utf-8 string, that is number
	//! of UNICODE characters, not number of bytes in UTF-8 encoded version.
	//!\note function assumes, that string is correct. No validation
	//!or range checking is performed
	Uint StringLengthUTF8(const char * s)
		throw(Screen::InvalidUTF8);
}

#endif // __UTF_8_H__
