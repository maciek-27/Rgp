
#include <termios.h>
#include <iostream>
#include"screen.h++"
#include"throw.h++"
#include"screenbuffer.h++"
#include"genericscreen.h++"
#include"vt100compatible.h++"
#include"vt100codes.h++"
#include"terminfoenabled.h++"
#include"terminfokeymap.h++"
using namespace std;

Scr::TerminfoEnabledScreen::TerminfoEnabledScreen(
		std::istream & _input,std::ostream & _output)throw()
    :GenericScreen(_input,_output),
	 Terminal(),ti(NULL),p(Scr::Fg::System,Scr::Fg::Dark,Scr::Bg::System)
{
    RexIOLog(LogLevelModerate)
		<< "TerminfoEnabledScreen(std::ostream & _output)"<<endl;
	p=copyBuffer[copyBuffer.GetHeight()-1]
				[copyBuffer.GetWidth()-1].style;
}
Scr::Key Scr::TerminfoEnabledScreen::DecodeKeyPressed()
		throw(Connection::UnsupportedKey,Screen::InvalidUTF8)
{
	try
	{
		return GenericScreen::DecodeKeyPressed();
	}
	catch(Scr::Connection::UnsupportedKey)
	{// special keys are exceptional, so treat them like exceptionals
		// implement terminfo-driven algorithm here
		input.UnGet();
		{
			char c = input.Peek();
			if (c!=0x1b and c<' ')
			{			
				RexIOLog(LogLevelModerate) << "Processing input " << static_cast<int>(c)<< endl;
				c=input.Get();
				if (c==9)
					return Key(Key::Tab);
				else
					return Key(c);
			}
		}
		std::string code;
		code.push_back(input.Get());
		while (true)
		{
			using namespace TI;
			RexIOLog(LogLevelModerate) << "Processing input " << code << endl;
			Keymap::validity v = ti->GetKeymap().TestCode(code.c_str());
			if (v == Dictionary<Key>::iterator::VALID)
				return ti->GetKeymap().GetCode(code.c_str());
			else
			{		
				try
				{
					if( not input.HasBufferedText() )
					{
						return ti->GetKeymap().GetCode(code.c_str());
					}
				}
				catch(...)	
				{
					throw;
				}	
				code.push_back(input.Get());
				continue;		
			}
		}		
		THROW(LogicError);
	}
	catch(...)
	{
		throw;
	}
}

void Scr::TerminfoEnabledScreen::Refresh()throw(ConnectionError)
{
	boost::mutex::scoped_lock lck(refreshMutex);
	if (!ti)
	{
		try
		{
		ti= const_cast<Scr::TI::TerminfoEntry*>
			(& (TI::TerminfoCore::GetTerminfo(GetType()) ));
		}
		catch (Scr::TI::NotSupportedTerminalType)
		{
			THROW(TerminalTypeUnknown);
		}
		// test if all required requests are supported
		try
		{
			ti->CursorHome();
			ti->GotoYX(Position(7,7));// any coordinate
		}
		catch (Scr::TI::TerminfoEntry::CapabilityNotSupported)
		{
			THROW(TerminalTypeUnknown);
		}	
		catch (...)
		{
			THROW(LogicError);
		}
	}
	
	try
	{
		output << ti->HideCursor();
	}
	catch(...){;}
	output << ti->CursorHome();
    Uint I=0,
	J=0;
    
// 						// (but don't do it when only foreground of
// 						// space changed
// 						and not (controlBuffer[i][j].c == ' '
// 								 and copyBuffer[i][j].c == ' ' 
// 								 and controlBuffer[i][j].style.GetBgColor()
// 								 == p.GetBgColor())
    for (Uint i=0;i<controlBuffer.GetHeight();i++)//for each row
		for (Uint j=0;j<controlBuffer.GetWidth();j++)// for each col
		{
			if (controlBuffer[i][j].c != 0)
			{
				if (controlBuffer[i][j]!=copyBuffer[i][j])
				{
					if (i!=I || j!=J)
					{
						output<< ti->GotoYX(Position(i,j));
						I=i;
						J=j;
					}
					if (// If writing style have changed, update it
						p!= controlBuffer[i][j].style)						 
					{
						output << ti->SetDisplayStyle(
							controlBuffer[i][j].style,p);
						p= controlBuffer[i][j].style;
					}
						
					// print character itself only if it is printable
					if (controlBuffer[i][j].c == 0x7f)
						EncodeUTF8(output,0x2421);// D E L
					else if (controlBuffer[i][j].c >= ' ')
						EncodeUTF8(output,controlBuffer[i][j].c);
					else
						EncodeUTF8(output,0x2400+controlBuffer[i][j].c);
					// Unicode characters representing teletype mnemonics for
					// first 31 ASCII characters
		
					J++;
				}
			}// controlBuffer[i][j].c == 0  -- do not print anything
			else// but mention, that last character was 2-column CJK.
			{
				if (j!=0 && j==(J+1))
					J++;
			}
				
		}
	// display cursor if requested
	if (cursorFlags&cursorVisible)
	{
		if (cursorFlags&cursorForced)
			output<< ti->GotoYX(cursorPosition);
		try
		{
    	output << ti->ShowCursor();
		}
		catch(...){;}
	}
	
	output<<flush;
    if (!output.good())
    	THROW(ConnectionError);
    copyBuffer=controlBuffer;

}

void Scr::TerminfoEnabledScreen::Resize(Uint rows, Uint cols)
    throw()
{
    GenericScreen::Resize(rows,cols);
    copyBuffer.Resize(controlBuffer.GetHeight(),
		      controlBuffer.GetWidth());
    copyBuffer.Fill(ScreenCharacter(0,DisplayStyle()));
}

void Scr::TerminfoEnabledScreen::CleanUp()  throw(ConnectionError)
{
	output << SHOW_CURSOR;
	output<<SET_ATTR(ATTR_RESET)<<ERASE_SCREEN  << CURSOR_HOME;
}

Scr::TerminfoEnabledScreen::~TerminfoEnabledScreen()throw()
{

	if (ti)
		TI::TerminfoCore::FreeTerminfoEntry();
    RexIOLog(LogLevelModerate)<<"~TerminfoEnabledScreen()"<<endl;
}

