#include <iostream>
#include"screen.h++"
#include"screenbuffer.h++"
#include"subscreen.h++"
#include"throw.h++"
#include"utf8.h++"
using namespace std;
using namespace Scr;

Scr::SubScreen::SubScreen(GenericScreen & _parent, Uint _y_offset,
			     Uint _x_offset, Uint _h,
			     Uint _w) throw()   
    : ScreenBase(),
      parent(_parent),
      offset(_y_offset,_x_offset),      
      s(_h,_w)
{
    ;//nothing
}

void Scr::SubScreen::Clear()throw()
{
	parent.GotoYX(offset.row,offset.col);
	parent.Rectangle(static_cast<wchar_t>(' '),s);
}

void Scr::SubScreen::SetBgColor(Bg::Color col)throw()
{
    parent.SetBgColor(col);
}

void Scr::SubScreen::SetFgColor(Fg::Color col)throw()
{
    parent.SetFgColor(col);
}

void Scr::SubScreen::SetFgStyle(Fg::Style s)throw()
{
    parent.SetFgStyle(s);
}

void Scr::SubScreen::GotoYX(Uint y, Uint x)
    throw(GotoOutOfRange)
{
    aPoint.row=y;
    aPoint.col=x;
}

inline void Scr::SubScreen::ParentGotoYXForPrinting()throw(PrintOutOfRange)	
{
	try
	{
		parent.GotoYX(aPoint.row+offset.row,aPoint.col+offset.col);
	}
	catch (GotoOutOfRange)
	{
		THROW(PrintOutOfRange);
	}	
}

void Scr::SubScreen::AddText(const char * text)throw(PrintOutOfRange,
							IllegalCharacter)
{
	ParentGotoYXForPrinting();
	parent.AddSubscreenText(text,s.width-aPoint.col);
	//let any exception from parent function propagate to caller.
	aPoint.col=parent.GetX()-offset.col;
	//set position after successful execution (assume strong exception safety
	//guarantee, that is "if exception is thrown, state remains unchanged")
}

void Scr::SubScreen::AddText(const std::string & text)
	throw(PrintOutOfRange, IllegalCharacter)
{
	AddText(text.c_str());	
}

void Scr::SubScreen::AddText(const wchar_t * text)throw(PrintOutOfRange,
							IllegalCharacter)
{
	ParentGotoYXForPrinting();
	parent.AddSubscreenText(text,s.width-aPoint.col);
	aPoint.col=parent.GetX()-offset.col;
}

void Scr::SubScreen::AddText(const std::wstring & text)
	throw(PrintOutOfRange,
		  IllegalCharacter)
{
	AddText(text.c_str());	
}

Uint Scr::SubScreen::AddTextCols(const wchar_t * text, Uint limitcols)
	throw(PrintOutOfRange, IllegalCharacter)
{
	try
	{
		parent.GotoYX(aPoint.row+offset.row,aPoint.col+offset.col);
	}
	catch (GotoOutOfRange)
	{
		THROW(PrintOutOfRange);
	}
    if ((aPoint.col+limitcols)>s.width)
		THROW(PrintOutOfRange);

	Uint printedlen = parent.AddTextCols(text, limitcols);
    aPoint.col += printedlen;

//    aPoint.col+=len;
	return printedlen;
}

Uint SubScreen::AddTextCols(const std::wstring & text, Uint limitcols)
	throw(PrintOutOfRange, IllegalCharacter)
{
	return AddTextCols(text.c_str(), limitcols);	
}

void Scr::SubScreen::HorizontalLine(char c, Uint n)
	throw(PrintOutOfRange, IllegalCharacter)
{
    parent.GotoYX(aPoint.row+offset.row,aPoint.col+offset.col);
    parent.HorizontalLine(c,n);
	aPoint.col+=n;
}

void Scr::SubScreen::HorizontalLine(wchar_t c, Uint n)
	throw(PrintOutOfRange, IllegalCharacter)
{
    parent.GotoYX(aPoint.row+offset.row,aPoint.col+offset.col);
    parent.HorizontalLine(c,n);
	aPoint.col+=n;
}

void Scr::SubScreen::VerticalLine(char c, Uint n)
	throw(PrintOutOfRange, IllegalCharacter)
{
    parent.GotoYX(aPoint.row+offset.row,aPoint.col+offset.col);
    parent.VerticalLine(c,n);
	aPoint.row+=n;
}

void Scr::SubScreen::VerticalLine(wchar_t c, Uint n)
	throw(PrintOutOfRange, IllegalCharacter)
{
    parent.GotoYX(aPoint.row+offset.row,aPoint.col+offset.col);
    parent.VerticalLine(c,n);
	aPoint.row+=n;
}

void Scr::SubScreen::Rectangle(wchar_t c, const Size & s)
	throw(PrintOutOfRange, IllegalCharacter)
{
    parent.GotoYX(aPoint.row+offset.row,aPoint.col+offset.col);
    parent.Rectangle(c,s);
	aPoint+=s;
}

void Scr::SubScreen::Rectangle(char c, const Size & s)
	throw(PrintOutOfRange, IllegalCharacter)
{
    parent.GotoYX(aPoint.row+offset.row,aPoint.col+offset.col);
    parent.Rectangle(c,s);
	aPoint+=s;
}

void Scr::SubScreen::AddCharacter(char c)throw(PrintOutOfRange)
{
	try
	{
		parent.GotoYX(aPoint.row+offset.row,aPoint.col+offset.col);
	}
	catch(Scr::Screen::GotoOutOfRange & e)
	{
		throw(PrintOutOfRange(string(e.what())+"\n " __WHERE_AM_I__));
	}
    parent.AddCharacter(c);
    aPoint.col++;
}

void Scr::SubScreen::AddCharacter(wchar_t  c)throw(PrintOutOfRange,
							    IllegalCharacter)
{
	try
	{
    parent.GotoYX(aPoint.row+offset.row,aPoint.col+offset.col);
	}
	catch(Scr::Screen::GotoOutOfRange & e)
	{
		throw(PrintOutOfRange(string(e.what())+"\n " __WHERE_AM_I__));
	}
    parent.AddCharacter(c);
    aPoint.col++;
}

void Scr::SubScreen::ForceCursorPosition(Position p)throw(RangeError)
{
	if (p.col>=GetWidth()) THROW(RangeError);
	if (p.row>=GetHeight()) THROW(RangeError);
	parent.ForceCursorPosition(p + offset);
}

void Scr::SubScreen::HideCursor()throw(CursorVisibilityNotSupported)
{
	parent.HideCursor();
}

void Scr::SubScreen::ShowCursor()throw(CursorVisibilityNotSupported)
{
	parent.ShowCursor();
}
void Scr::SubScreen::Refresh()
    throw(ConnectionError) // parent object may throw this
				       // exception, and then calling
				       // function will have to catch it.
    
{
    parent.Refresh();
}

void Scr::SubScreen::Resize(Uint rows, Uint cols)
    throw(SubscreenResize)
{
    THROW(SubscreenResize);
}

const char * Scr::SubScreen::GetType() const throw(TerminalTypeUnknown)
{
	return parent.GetType();
}

Scr::Uint Scr::SubScreen::GetHeight() const throw()
{
    return s.height;
}

Scr::Uint Scr::SubScreen::GetWidth() const throw()
{
    return s.width;
}

Scr::Screen * Scr::SubScreen::
CreateSubScreen(Uint _y_offset, Uint _x_offset, Uint _h,
				Uint _w)throw(SubscreenOutOfRange)
{
    SubScreenRangeCheck();
    // if no exceptional conditions, just create and return new
    // subscreen

	// no difference whenever it will be another subscreen of parent
	// or subscreen of subscreen... but less recursive calls needed to
	// refresh this subscreen or print anything. 
    return new SubScreen(parent,
						 offset.row+_y_offset,
						 offset.col+_x_offset,
						 _h, _w);
}

bool Scr::SubScreen::GetCursorVisibility() const throw()
{
	return parent.GetCursorVisibility();
}

Scr::SubScreen::~SubScreen()throw()
{
	;//nothing
}

