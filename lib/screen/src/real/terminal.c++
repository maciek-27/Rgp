
#include <termios.h>
#include <iostream>
#include"screen.h++"
#include"throw.h++"
#include"screenbuffer.h++"
#include"genericscreen.h++"
#include"vt100compatible.h++"
#include"vt100codes.h++"
#include"terminfoenabled.h++"
using namespace std;

Scr::Terminal::Terminal()throw()
	:copyBuffer(25,80,ScreenCharacter(0,DisplayStyle(Fg::System,Fg::Dark,Bg::System)))
{;}
