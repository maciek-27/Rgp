#include <termios.h>
#include <iostream>
#include"screen.h++"
#include"subscreen.h++"
#include"screenbuffer.h++"
#include"genericscreen.h++"
#include"vt100compatible.h++"
#include"localscreen.h++"
#include"throw.h++"

using namespace std;
using namespace Scr;

Control::_PositionYX Control::GotoYX(Uint _y, Uint _x)
{
    return _PositionYX(_y,_x);
}

Screen::Screen()throw()//:ss(new std::stringstream)
{
    RexIOLog(LogLevelModerate) << "Screen - base" << endl;
}

Screen::~Screen()throw()
{
    RexIOLog(LogLevelModerate) << "~Screen - base" << endl;
}

template<class T>
inline Screen& operatorIOS(Screen & screen,const T & whatto)
{
	std::ostringstream ss;
	ss <<whatto;

	screen.AddText((ss.str().c_str()));
	return screen;
}
namespace Scr{
Screen& operator<<(Screen & screen,const  wchar_t(&  whatto)[9])
{
	screen.AddText(whatto);
	return screen;
}

/*Screen& operator<<(Screen & screen,const EString & whatto)
{
	screen.AddText(whatto);
	return screen;
}*/

Screen& operator<<(Screen & screen,const std::wstring & whatto)
{
	screen.AddText(whatto);
	return screen;
}

Screen& operator<<(Screen & screen,wchar_t const * const & whatto)
{
	screen.AddText(whatto);
	return screen;
}

Screen& operator<<(Screen & screen,const std::string & whatto)
{
	screen.AddText(whatto);
	return screen;
}

Screen& operator<<(Screen & screen,char const * const & whatto)
{
	screen.AddText(whatto);
	return screen;
}

Screen& operator<<(Screen & screen,char * const & whatto)
{
	screen.AddText(whatto);
	return screen;
}

Screen& operator<<(Screen & screen,const Fg::Color & whatto)
{
	screen.SetFgColor(whatto);
	return screen;
}

Screen& operator<<(Screen & screen,const Fg::Style & whatto)
{
	screen.SetFgStyle(whatto);
	return screen;
}

Screen& operator<<(Screen & screen,const Bg::Color & whatto)
{
	screen.SetBgColor(whatto);
	return screen;
}

Screen& operator<<(Screen & screen,const Control::_PositionYX & whatto)
{
	screen.GotoYX(whatto.row,whatto.col);
	return screen;
}

Screen& operator<<(Screen & screen,const Control::_Refresh & whatto)
{
	screen.Refresh();
	return screen;
}

Screen& operator<<(Screen & screen,const Control::_Clear & whatto)
{
	screen.Clear();
	return screen;
}

Screen& operator<<(Screen & screen,const DisplayStyle & whatto)
{
	screen.SetFgStyle(whatto.GetFgStyle());
	screen.SetFgColor(whatto.GetFgColor());
	screen.SetBgColor(whatto.GetBgColor());

	return screen;
}	

Screen& operator<<(Screen & screen, unsigned int whatto)
{
	return operatorIOS(screen,whatto);
}
Screen& operator<<(Screen & screen,int  whatto)
{
	return operatorIOS(screen,whatto);
}
Screen& operator<<(Screen & screen,std::_Setw  whatto)
{
	return operatorIOS(screen,whatto);
}
Screen& operator<<(Screen & screen, unsigned long whatto)
{
	return operatorIOS(screen,whatto);
}
Screen& operator<<(Screen & screen, long  whatto)
{
	return operatorIOS(screen,whatto);
}

Screen& operator<<(Screen & screen, char  whatto)
{
	screen.AddCharacter(whatto);
	return screen;
}
Screen& operator<<(Screen & screen, wchar_t  whatto)
{
	screen.AddCharacter(whatto);
	return screen;
}

}

