#include <rexio/keyboard.h++>
#include<rexio/tk/toolkit.h++>
#include<iostream>
#include "statusbar.h++"
#include<sstream>
#include<algorithm>
using namespace std;
using namespace Scr;
using namespace Scr::Tk;
using namespace Scr::Control;


string StatusBar::Status("");

 const wchar_t separatorleft  =0x2561;
 const wchar_t separatorright =0x255E;
 const wchar_t line =0x2550;

void StatusBar::OnRedraw(Scr::Screen &screen)throw()
{
	screen << GetStyle();
	screen.GotoYX( 0 , 0 );
	screen.HorizontalLine(line,GetWidth());
	if (not Status.empty() and GetWidth()>8)
	{
		screen.GotoYX( 0 , 2 );
			screen
				<< separatorleft ;
		if (GetWidth()>Status.length()+5)
			screen
				<< ' ' <<  Status <<' ' 
				<< separatorright;
		else
		{
			try
			{
				screen << ' ' << Status.substr(0,GetWidth()-9)
					   <<  "... "
					   << separatorright;
			}
			catch (...)//InvalidUTF8
			{
				screen << GotoYX( 0 , 2 ) << line << line;
				;
			}// we may have damaged u8 code while cutting
			// Status. Don't care.
		}
				
    }
}


void StatusBar::OnKeyDown(Scr::Key key)throw()
{
}
