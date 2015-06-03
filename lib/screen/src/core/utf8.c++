#include <iostream>
#include"screen.h++"
#include"throw.h++"
#include"utf8.h++"

using namespace Scr;
using namespace std;

#ifdef DO_VALIDATE_UTF_8_OUTPUT
# define VALIDATE_TRAILING(x)   \
	if ( (c[x]&0xC0)!=0x80)  \
	    THROW(Screen::InvalidTrailingByte)
#else
# define VALIDATE_TRAILING(x)
#endif //DO_VALIDATE_UTF_8_OUTPUT

wchar_t Scr::DecodeUTF8(const char ** pstr)
	throw(Screen::InvalidUTF8)
{
    Uint result;
    unsigned char c[4];
    c[0]=**pstr;
    //result=c[0];
 
    if ( (c[0] & 0xF8) == 0xF0) // 4 byte char
    {
		(*pstr)++;
		c[1]=**pstr;
		VALIDATE_TRAILING(1); // only some values for trailing bytes
		// are valid
		(*pstr)++;
		c[2]=**pstr;
		VALIDATE_TRAILING(2);
		(*pstr)++;
		c[3]=**pstr;
		VALIDATE_TRAILING(3);
		c[0]&=0x0F;
		c[1]&=0x7F;
		c[2]&=0x7F;
		c[3]&=0x7F;
		result = c[3]+(((Uint)c[2])<<6)+(((Uint)c[1])<<12)
			+(((Uint)c[0])<<18);

#ifdef DO_VALIDATE_UTF_8_OUTPUT	
		if (result < (1<<16))// overlong UTF8 encoding FORBIDDEN
			// according to RFC 3629
		{
			(*pstr)-=3;
			THROW(Screen::OverlongUTF8Encoding);
		}
#endif	
    }
    else if ((c[0] & 0xF0) == 0xE0) // 3 byte char
    {
		(*pstr)++;
		c[1]=**pstr;
		VALIDATE_TRAILING(1);
		(*pstr)++;
		c[2]=**pstr;
		VALIDATE_TRAILING(2);
		c[0]&=0x1F;
		c[1]&=0x7F;
		c[2]&=0x7F;
		result = c[2]+(((Uint)c[1])<<6)+(((Uint)c[0])<<12);

#ifdef DO_VALIDATE_UTF_8_OUTPUT	
		if (result < (1<<11))// overlong UTF8 encoding FORBIDDEN
			// according to RFC 3629
		{
			(*pstr)-=2;
			THROW(Screen::OverlongUTF8Encoding);
		}
#endif	
    }
    else if ((c[0] & 0xE0) == 0xC0) // 2 byte char
    {
		(*pstr)++;
		c[1]=**pstr;
		VALIDATE_TRAILING(1);
	
		c[0]&=0x3F;
		c[1]&=0x7F;

		result = c[1]+(((Uint)c[0])<<6);

#ifdef DO_VALIDATE_UTF_8_OUTPUT	
		if (result < (1<<7))// overlong UTF8 encoding FORBIDDEN
			// according to RFC 3629
		{
			(*pstr)--;
			THROW(Screen::OverlongUTF8Encoding);
		}
#endif	
    }
    else
#ifdef DO_VALIDATE_UTF_8_OUTPUT	
		if ((c[0] & 0x80) == 0)
#endif	
			// 1 byte char
		{
			result=c[0];
		}
#ifdef DO_VALIDATE_UTF_8_OUTPUT
		else
			THROW(Screen::InvalidFirstByte);
#endif	

    return result;
}

//std namespace specifier for ostream is said explicitly. It would
//compilewithout it as "using namespace" is above, but doxygen
//dislikes such inconsistencies
void Scr::EncodeUTF8(std::ostream & o, Uint c)throw()
{
    if (c<(1<<7))// 7 bit Unicode encoded as plain ascii
    {
		o << static_cast<char>(c);
		return;
    }
    if (c<(1<<11))// 11 bit Unicode encoded in 2 UTF-8 bytes
    {
		o << static_cast<unsigned char>((c>>6)|0xC0)
		  << static_cast<unsigned char>((c&0x3F)|0x80);
		return;
    }
    if (c<(1<<16))// 16 bit Unicode encoded in 3 UTF-8 bytes
    {
		o << static_cast<unsigned char>(((c>>12))|0xE0)
		  << static_cast<unsigned char>(((c>>6)&0x3F)|0x80)
		  << static_cast<unsigned char>((c&0x3F)|0x80);
		return;
    }
    
    if (c<(1<<21))// 21 bit Unicode encoded in 4 UTF-8 bytes
    {
		o << static_cast<unsigned char>(((c>>18))|0xF0)
		  << static_cast<unsigned char>(((c>>12)&0x3F)|0x80)
		  << static_cast<unsigned char>(((c>>6)&0x3F)|0x80)
		  << static_cast<unsigned char>((c&0x3F)|0x80);
		return;
    }
}

Uint Scr::CharLengthUTF8(const char * s)
	throw(Screen::InvalidUTF8)
{
	if ( (*s) & 0x80 )
	{ // more than 1 byte
		if ( ( (*s) & 0xF8) == 0xF0) // 4-byte
			return 4;
		if ( ( (*s) & 0xF0) == 0xE0)
			return 3;
		if ( ( (*s) & 0xE0) == 0xC0)
			return 2;
	}
	else
		return 1;
	THROW(Screen::InvalidUTF8);
}

Uint Scr::StringLengthUTF8(const char * s)
	throw(Screen::InvalidUTF8)
{
	Uint result = 0;
	while (*s)
	{
		s += CharLengthUTF8(s);
		result++;
	}
	return result;
}
