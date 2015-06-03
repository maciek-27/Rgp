#include "hintbox.h++"
using namespace Scr;
using namespace Scr::Tk;
using namespace Scr::Control;
using std::string;

HintBox::Hints HintBox::H1;

void HintBox::OnRedraw(Scr::Screen &screen)throw()
{
	screen << GetStyle() << Clear << GotoYX(0,0);
	Uint y = 0;
	for ( std::list <std::string> :: iterator i
			  = H1.begin() ; i!=H1.end() ; ++i)
	{
		try
		{
			screen <<' ' << *i;
		}
		catch (Screen::PrintOutOfRange)
		{
			if (y<GetHeight())
			{
				screen << GotoYX ( ++y , 0 );
				--i;
			}
			else
			{
				static const string
					msg_expand("... move statusbar to see more hints");
				if (GetWidth()>=msg_expand.length())
					screen << GotoYX ( GetHeight()-1,
									   GetWidth()-msg_expand.length() )
						   << msg_expand;
			}
		}
		catch (...)
		{
			return;
		}			
	}
}
