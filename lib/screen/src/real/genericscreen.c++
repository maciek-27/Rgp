#include <iostream>
#include<wchar.h>
#include"screen.h++"
#include"screenbuffer.h++"
#include"genericscreen.h++"
#include"throw.h++"

using namespace Scr;
using namespace std;

template<typename _char_type>
Uint GenericScreen::PrecomputeTextCharsWidth(_char_type * text, vector<char>&
	widths, Uint maxwidth)
	throw(RangeError, IllegalCharacter)
{
	Uint sum=0;
	do
	{
		register Uint w = width(*text);
		widths.push_back(w);
		sum+=w;
		text++;
	}
	while ( ( *text != 0 ) and (sum<maxwidth) );
		//break loop if sum is equal or greater than maxwidth. it it is equal, 
		//and *text!=0, than surely in next pass of loop it will be greater
	
	if (*text!=0)//loop broken, but not whole string calculated
		THROW(RangeError);
	return sum;
}

namespace Scr
{
//!local template specialization: adds UTF8 Decoding
	template<>
	Uint GenericScreen::PrecomputeTextCharsWidth (const char * text, 
			vector<char>& widths, Uint maxwidth)
	throw(RangeError, IllegalCharacter)
	{
		Uint sum=0;
		do
		{
			register Uint w = width (DecodeUTF8 (&text));
			widths.push_back (w);
			sum+=w;
			text++;
		}
		while ( ( *text != 0 ) and (sum<maxwidth) );
		//break loop if sum is equal or greater than maxwidth. it it is equal,
		//and *text!=0, than surely in next pass of loop it will be greater
		
		if (*text!=0)//loop broken, but not whole string calculated
			THROW (RangeError);
		return sum;
	}

}

Scr::Key Scr::GenericScreen::DecodeKeyPressed()
		throw(Connection::UnsupportedKey,Screen::InvalidUTF8)
{
	Uint c = input.Get();

	if (c==Key::LF)
		return Key::Enter;

	if (c==127)
		return Key::Backspace;
	if (c==8)
		return Key::Backspace;
	
	if (c>=' ')
	{
		input.UnGet();
		return DecodeBasicKeyPressed();
	}
	
	if (input.HasBufferedText())
		THROW(Scr::Connection::UnsupportedKey);
	else if (c==0x1b)
		return Key::Escape;
	else
		THROW(Scr::Connection::UnsupportedKey);
}

//simple macros for enabling and disabling utf-8 validation
#ifdef DO_VALIDATE_UTF_8_OUTPUT
# define VALIDATE_TRAILING(x)              \
	if ( (c[x]&0xC0)!=0x80)                \
	    THROW(Screen::InvalidTrailingByte)
#else
# define VALIDATE_TRAILING(x)
#endif //DO_VALIDATE_UTF_8_OUTPUT

Scr::Key Scr::GenericScreen::DecodeBasicKeyPressed()throw(Screen::InvalidUTF8)
{
	    
    Uint result;
    unsigned char c[4];
    c[0]=input.Get();
 
    if ( (c[0] & 0xF8) == 0xF0) // 4 byte char
    {
	
		c[1]=input.Get();
		VALIDATE_TRAILING(1); // only some values for trailing bytes
		// are valid
	
		c[2]=input.Get();
		VALIDATE_TRAILING(2);
	
		c[3]=input.Get();
		VALIDATE_TRAILING(3);
		c[0]&=0x0F;
		c[1]&=0x7F;
		c[2]&=0x7F;
		c[3]&=0x7F;
		result = c[3]+(((Uint)c[2])<<6)+(((Uint)c[1])<<12)
			+(((Uint)c[0])<<18);

		if (result < (1<<16))// overlong UTF8 encoding FORBIDDEN
			// according to RFC 3629
		{
			THROW(Screen::OverlongUTF8Encoding);
		}
    }
    else if ((c[0] & 0xF0) == 0xE0) // 3 byte char
    {
	
		c[1]=input.Get();
		VALIDATE_TRAILING(1);
	
		c[2]=input.Get();
		VALIDATE_TRAILING(2);
		c[0]&=0x1F;
		c[1]&=0x7F;
		c[2]&=0x7F;
		result = c[2]+(((Uint)c[1])<<6)+(((Uint)c[0])<<12);

		if (result < (1<<11))// overlong UTF8 encoding FORBIDDEN
			// according to RFC 3629
		{
			THROW(Screen::OverlongUTF8Encoding);
		}
    }
    else if ((c[0] & 0xE0) == 0xC0) // 2 byte char
    {
	
		c[1]=input.Get();
		VALIDATE_TRAILING(1);
	
		c[0]&=0x3F;
		c[1]&=0x7F;

		result = c[1]+(((Uint)c[0])<<6);
		if (result < (1<<7))// overlong UTF8 encoding FORBIDDEN
			// according to RFC 3629
		{
			THROW(Screen::OverlongUTF8Encoding);
		}
    }
    else
		if ((c[0] & 0x80) == 0)
			// 1 byte char
		{
			result=c[0];
		}
		else
			THROW(Screen::InvalidFirstByte);

    return result;
}

/*!
  Print block of text from specific source: while condition is true
  add chr from source and perform additional action "finish"
 */
#define PRINT_TEXT(condition,source,finish)								\
	bool fgt (properties.GetFgColor() ==Fg::Transparent);				\
    bool bgt (properties.GetBgColor() ==Bg::Transparent);				\
																		\
	if (fgt && bgt )/*both: background and foreground are transparent*/	\
		while (condition)												\
		{																\
			controlBuffer[aPoint.row][aPoint.col].style.SetFgStyle(properties.GetFgStyle()); \
			controlBuffer[aPoint.row][aPoint.col].c=source;				\
			finish;														\
		}																\
    else if (fgt)/* foreground is transparent, but background isn't */	\
		while (condition)												\
		{																\
			controlBuffer[aPoint.row][aPoint.col].style.SetFgStyle(properties.GetFgStyle()); \
			controlBuffer[aPoint.row][aPoint.col].style.SetBgColor(properties.GetBgColor()); \
			controlBuffer[aPoint.row][aPoint.col].c=source;				\
			finish;														\
		}																\
    else if (bgt)														\
		while (condition)												\
		{																\
			controlBuffer[aPoint.row][aPoint.col].style.SetFgColor(properties.GetFgColor()); \
			controlBuffer[aPoint.row][aPoint.col].style.SetFgStyle(properties.GetFgStyle()); \
			controlBuffer[aPoint.row][aPoint.col].c=source;				\
			finish;														\
		}																\
	else /*niether background, nor foreground is transparent */			\
		while (condition)												\
 		{																\
			controlBuffer[aPoint.row][aPoint.col].style=properties;		\
			controlBuffer[aPoint.row][aPoint.col].c=source;				\
			finish;                                                     \
		}

// end of macro PRINT_TEXT

// FOLLOWING MACRO HAS TO BE REWRITTEN IN TERMS OF widths VECTOR for
// improved efficiency (use precomputed widths)

/*! To achieve UNICODE compliance CJK must be supported - this macro
 *  performs additional configuration after adding character, that
 *  may be CJK*/
#define ADDWCHAR_BASE(op_1,op_CJK,w)									\
	if (w==1)															\
	{																	\
		/*to balance column width w/ CJK*/								\
		if (aPoint.col != 0 &&											\
			width(controlBuffer[aPoint.row][aPoint.col-1].c)==2)		\
			controlBuffer[aPoint.row][aPoint.col-1].c=' ';				\
		if (aPoint.col+1 <controlBuffer.GetWidth() )					\
		{																\
			if (controlBuffer[aPoint.row][aPoint.col+1].c==0)			\
				controlBuffer[aPoint.row][aPoint.col+1].c=' ';			\
		}																\
		op_1;															\
	}																	\
	else if (w==2)														\
	{   /*Previous column CAN'T be CJK, as theese characters MUST be*/	\
		/* separated by NULL character to balance width*/				\
		/*  */															\
		if (aPoint.col != 0 &&											\
			width(controlBuffer[aPoint.row][aPoint.col-1].c)==2)		\
			controlBuffer[aPoint.row][aPoint.col-1].c=' ';				\
		/* Fill subsequent character w/ 0*/								\
		if (aPoint.col+1 <controlBuffer.GetWidth() )					\
		{																\
			if (aPoint.col+2 <controlBuffer.GetWidth() &&				\
				width(controlBuffer[aPoint.row][aPoint.col+1].c)==2)	\
			{															\
				/* subsequent is CJK, so next is 0. fill it w/ space */	\
				/* to engorce correct layout of text during refresh  */	\
				controlBuffer[aPoint.row][aPoint.col+2].c=' ';			\
				controlBuffer[aPoint.row][aPoint.col+2].style=			\
					controlBuffer[aPoint.row][aPoint.col].style;		\
			}															\
			controlBuffer[aPoint.row][aPoint.col+1].c=0;				\
			controlBuffer[aPoint.row][aPoint.col+1].style=				\
				controlBuffer[aPoint.row][aPoint.col].style;			\
		}																\
		op_CJK;															\
	}																	\
	else																\
		;

//end of macro ADDWCHAR_BASE

/*!
 * Add wide character. doo width lookup for character using C function
 */
#define ADDWCHAR_DEFAULT(op_1,op_CJK)									\
	int w = width(controlBuffer[aPoint.row][aPoint.col].c);				\
	ADDWCHAR_BASE(op_1,op_CJK,w)

/*!		
 * Add character. Check width in lookup table.
 */
#define ADDWCHAR_PRECOMPUTED_DEFAULT(op_1,op_CJK)\
	;ADDWCHAR_BASE(op_1,op_CJK,widths[i]);i++;

// macro for default wide char adding
#define ADDWCHAR ADDWCHAR_DEFAULT(aPoint.col++,aPoint.col+=2)

#define ADDWCHAR_PRECOMPUTED \
	ADDWCHAR_PRECOMPUTED_DEFAULT(aPoint.col++,aPoint.col+=2)

// Constructor initializes base objects
GenericScreen::GenericScreen(std::istream & _input,std::ostream & _output)throw()
    : ScreenBase(),
      controlBuffer(25,80),// will be changed before object used
						   // (now actual dimensions are unknown, and therefore
						   //  25x80 is as good as 12x13 or 120x430)
			
	  cursorPosition(0,0),
	  cursorFlags(cursorVisible),
	  input(_input),
      output(_output)
	{;}

void GenericScreen::Clear()throw()
{
    controlBuffer.Fill(ScreenCharacter(' ', properties));
}

void GenericScreen::SetBgColor(Bg::Color col)throw()
{
    properties.SetBgColor(col);
}

void GenericScreen::SetFgColor(Fg::Color col)throw()
{
    properties.SetFgColor(col);
}

void GenericScreen::SetFgStyle(Fg::Style s)throw()
{
    properties.SetFgStyle(s);
}
void GenericScreen::GotoYX(Uint y, Uint x)    
	    throw(GotoOutOfRange)
{
    if (y>=GetHeight() || x>=GetWidth())
		THROW(GotoOutOfRange);
    aPoint.row = y;
    aPoint.col = x;
}

void GenericScreen::AddText(const char * text)
    throw(PrintOutOfRange,
	  IllegalCharacter)
{
	vector<char> widths;
	widths.reserve(controlBuffer.GetWidth());
	try
	{
		AddText(text,
			// number of columns needed for specific text (if throws exception,
			//		AddText(const char *, Uint) is not executed)	
			PrecomputeTextCharsWidth(text,widths,controlBuffer.GetWidth()),
									 widths);
	}
	catch (RangeError & e)
	{
		throw(PrintOutOfHorizontalRange(string(e.what())+__WHERE_AM_I__));
	}
}

void GenericScreen::AddText(const std::string & text)
    throw(PrintOutOfRange,
		  IllegalCharacter)
{
	AddText(text.c_str());
}



void GenericScreen::AddText(const char * text, Uint cols,
		const vector<char> &widths)
        throw(PrintOutOfRange, IllegalCharacter)
{
	if (cols> controlBuffer.GetWidth()-aPoint.col)
		THROW(PrintOutOfHorizontalRange);
    if (aPoint.row>=GetHeight())
		THROW(PrintOutOfVerticalRange);
	size_t i=0;
	PRINT_TEXT(*text,DecodeUTF8(&text),text++;ADDWCHAR_PRECOMPUTED);
}

void GenericScreen::AddText(const std::wstring & text)   
	throw(PrintOutOfRange,
		  IllegalCharacter)
{
    AddText(text.c_str());
}

#define DECLARE_WIDTHS_AND_COLS(m)											\
	vector<char> widths;													\
	widths.reserve(controlBuffer.GetWidth());								\
	Uint cols;																\
	try																		\
	{																		\
		cols=PrecomputeTextCharsWidth(text,widths,m);						\
	}																		\
	catch (RangeError & e)													\
	{																		\
		throw(PrintOutOfHorizontalRange(string(e.what())					\
								+"\n " __WHERE_AM_I__));					\
	}																		\

void GenericScreen::AddText(const wchar_t * text)   
	throw(PrintOutOfRange, IllegalCharacter)
{	
	DECLARE_WIDTHS_AND_COLS(controlBuffer.GetWidth());
	if (cols> controlBuffer.GetWidth()-aPoint.col)
		THROW(PrintOutOfHorizontalRange);
    if (aPoint.row>=GetHeight())
		THROW(PrintOutOfVerticalRange);

	PRINT_TEXT(*text,*text,text++;ADDWCHAR);
}

//adding specified text, at most limitcols columns
Uint GenericScreen::AddTextCols(const wchar_t * text, Uint limitcols)
	throw(PrintOutOfRange, IllegalCharacter)
{
	vector<char> widths;													
	widths.reserve(controlBuffer.GetWidth());								
	Uint cols=0;																
	Uint i = 0;
	while ( ( text[i] != 0 ) and (cols<=limitcols) )
	{
		const register Uint w = width (text[i++]);
		widths.push_back (w);
		cols+=w;
	}
	cols=min(cols,limitcols);

	if (cols> controlBuffer.GetWidth()-aPoint.col)
		THROW(PrintOutOfHorizontalRange);

	if (aPoint.row>=GetHeight())
		THROW(PrintOutOfVerticalRange);
	
	Sint _i = limitcols;
	i=0;

	PRINT_TEXT((text[i]) && ((_i-=widths[i])>=0),text[i],
			   ADDWCHAR_PRECOMPUTED);

		
	_i+=width(*(text));

	return limitcols - _i;
}

Uint GenericScreen::AddTextCols(const std::wstring& text, Uint limitcols)
	throw(PrintOutOfRange,
		  IllegalCharacter)
{
	return AddTextCols(text.c_str(), limitcols);
}

void GenericScreen::AddSubscreenText(const char * text, Uint widthlimit)
	throw(PrintOutOfRange, IllegalCharacter)
{
	vector<char> widths;
	widths.reserve(controlBuffer.GetWidth());
	try
	{
		AddText(text,
			// number of columns needed for specific text (if throws exception,
			//		AddText(const char *, Uint) is not executed)	
			PrecomputeTextCharsWidth(text,widths,widthlimit), widths);
	}
	catch (RangeError & e)
	{
		throw(PrintOutOfHorizontalRange(string(e.what())+"\n " __WHERE_AM_I__));
	}
}

void GenericScreen::AddSubscreenText(const wchar_t * text, Uint widthlimit)
	throw(PrintOutOfRange, IllegalCharacter)
{
	vector<char> widths;
	widths.reserve(controlBuffer.GetWidth());
	Uint cols;
	try
	{
		cols=PrecomputeTextCharsWidth(text,widths,widthlimit);
	}
	catch (RangeError & e)
	{
		throw(PrintOutOfHorizontalRange(string(e.what())+"\n " __WHERE_AM_I__));
	}
	
	if (cols> controlBuffer.GetWidth()-aPoint.col)
		THROW(PrintOutOfHorizontalRange);
    if (aPoint.row>=GetHeight())
		THROW(PrintOutOfVerticalRange);
	
	int i = 0;// variable used by ADDWCHAR_PRECOMPUTED macro
	PRINT_TEXT(text[i],text[i],ADDWCHAR_PRECOMPUTED);
//		PRINT_TEXT(*text,*text,text++;ADDWCHAR_PRECOMPUTED);
}

void GenericScreen::HorizontalLine(char c, Uint n)
	throw(PrintOutOfRange,
		  IllegalCharacter)
{
	HorizontalLine(static_cast<wchar_t>(c),n);
}

void GenericScreen::HorizontalLine(wchar_t c, Uint n)
	throw(PrintOutOfRange,
		  IllegalCharacter)
{
	if (n> controlBuffer.GetWidth()-aPoint.col+1)
		THROW(PrintOutOfHorizontalRange);
    if (aPoint.row>=GetHeight())
		THROW(PrintOutOfVerticalRange);
	PRINT_TEXT(n--,c,ADDWCHAR);
}

void GenericScreen::VerticalLine(char c, Uint n)
	throw(PrintOutOfRange,
		  IllegalCharacter)
{
	VerticalLine(static_cast<wchar_t>(c),n);	
}

void GenericScreen::VerticalLine(wchar_t c, Uint n)
	throw(PrintOutOfRange,
		  IllegalCharacter)
{
	if (n> controlBuffer.GetHeight()-aPoint.row+1)
		THROW(PrintOutOfHorizontalRange);
    if (aPoint.col>=GetWidth())
		THROW(PrintOutOfVerticalRange);
	PRINT_TEXT(n--,c,ADDWCHAR_DEFAULT(aPoint.row++,aPoint.row++));
}

void GenericScreen::Rectangle(char c, const Size & s)
	throw(PrintOutOfRange,
		  IllegalCharacter)
{
	Uint n = s.height;
	while (n--)
	{
		HorizontalLine(c,s.width);
		aPoint.col-=s.width;
		aPoint.row++;
	}
}

void GenericScreen::Rectangle(wchar_t c, const Size & s)
	throw(PrintOutOfRange,
		  IllegalCharacter)
{
	Uint n = s.height;
	while (n--)
	{
		HorizontalLine(c,s.width);
		aPoint.col-=s.width*width(c);
		aPoint.row++;
	}
}

void GenericScreen::AddCharacter(char c)
    throw(PrintOutOfRange)
{
    if (aPoint.col>=GetWidth())
		THROW(PrintOutOfHorizontalRange);
    if (aPoint.row>=GetHeight())
		THROW(PrintOutOfVerticalRange);

    controlBuffer[aPoint.row][aPoint.col].c=c;
	if (properties.GetFgColor() !=Fg::Transparent)
		controlBuffer[aPoint.row][aPoint.col].style.SetFgColor(properties.GetFgColor());
	controlBuffer[aPoint.row][aPoint.col].style.SetFgStyle(properties.GetFgStyle());
	if (properties.GetBgColor() !=Bg::Transparent)
		controlBuffer[aPoint.row][aPoint.col].style.SetBgColor(properties.GetBgColor());
    ADDWCHAR;
}
void GenericScreen::AddCharacter(wchar_t c)
    throw(PrintOutOfRange,
		  IllegalCharacter)
{
    if (aPoint.col>=GetWidth())
		THROW(PrintOutOfHorizontalRange);
    if (aPoint.row>=GetHeight())
		THROW(PrintOutOfVerticalRange);
    if (c >= (1<<21))
		THROW(CharacterExceedsUTF8Range);
    controlBuffer[aPoint.row][aPoint.col].c=c;
	
	if (properties.GetFgColor() !=Fg::Transparent)
		controlBuffer[aPoint.row][aPoint.col].style.SetFgColor(properties.GetFgColor());
	controlBuffer[aPoint.row][aPoint.col].style.SetFgStyle(properties.GetFgStyle());
	if (properties.GetBgColor() !=Bg::Transparent)
		controlBuffer[aPoint.row][aPoint.col].style.SetBgColor(properties.GetBgColor());
    ADDWCHAR;
}

void GenericScreen::ForceCursorPosition(Position p )throw(RangeError)
{
    if (p.row>=GetHeight() || p.col>=GetWidth())
		THROW(RangeError);
	
	cursorFlags|=cursorForced;
	cursorPosition=p;
}
			

void GenericScreen::HideCursor()throw(CursorVisibilityNotSupported)
{
	cursorFlags &=~ cursorVisible;
}

void GenericScreen::ShowCursor()throw(CursorVisibilityNotSupported)
{
	cursorFlags |=  cursorVisible;
}

void GenericScreen::Refresh() // just a dumb proc to produce
throw(ConnectionError)    // basic debug printout
{
    for (Uint i=0;i<controlBuffer.GetHeight();i++)//for each row
    {
		for (Uint j=0;j<controlBuffer.GetWidth();j++)
		{// print raw character if printable low ascii
			unsigned char c = controlBuffer[i][j].c;
			output << (char)(((c>31)&&(c<128))?c:'.');
			// or leave dot otherwise
		}
		output << endl;
		// flush each row
    }
}

Screen * GenericScreen::
CreateSubScreen(Uint _y_offset, Uint _x_offset, Uint _h,
				Uint _w)throw(SubscreenOutOfRange)
{
    SubScreenRangeCheck();
    // if no exceptional conditions, just create and return new subscreen
    return new SubScreen(*this, _y_offset, _x_offset, _h, _w);
}

void GenericScreen::Resize(Uint rows, Uint cols)
	    throw()
{
    controlBuffer.Resize(rows,cols);
}

const char * Scr::GenericScreen::GetType() const throw(TerminalTypeUnknown)
{
	THROW (TerminalTypeUnknown);
	// this implementation does not support type.
}

Uint GenericScreen::GetHeight() const throw()
{
    return controlBuffer.GetHeight();
}

Uint GenericScreen::GetWidth() const throw()
{
    return controlBuffer.GetWidth();
}

bool GenericScreen::GetCursorVisibility() const throw()
{
	return cursorFlags bitand cursorVisible;
}

void GenericScreen::CleanUp() throw(ConnectionError)
{
    ;
}

GenericScreen::~GenericScreen()throw()
{
    ;
}
