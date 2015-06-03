
#include <termios.h>
#include <iostream>
#include"screen.h++"
#include"throw.h++"
#include"screenbuffer.h++"
#include"genericscreen.h++"
#include"vt100compatible.h++"
#include"vt100codes.h++"
using namespace std;

Scr::VT100Compatible::VT100Compatible(std::istream & _input,
		std::ostream & _output)throw()
    :GenericScreen(_input,_output),
	 Terminal()
{
    RexIOLog(LogLevelModerate) << "VT100Compatible(std::ostream & _output)"<<endl;
}
	
Scr::Key Scr::VT100Compatible::DecodeKeyPressed()
		throw(Connection::UnsupportedKey,Screen::InvalidUTF8)
{
	std::string DebugInfo(input.DebugInfo());
	Uint c = input.Get();

	if (c==Key::LF)
		return Key::Enter;

	if (c==127)
		return Key::Backspace;
	if (c==8)
		return Key::Backspace;
	
	if (c!= Key::Escape)
	{
		input.UnGet();
		return DecodeBasicKeyPressed();// go back and process UTF-8 input
	}
	try
	{
		c = input.TryGet();
	}
	catch (Scr::BufferedInput::BufferEmpty)
	{
		return Key::Escape;
	}
	
	if (c == 'O')  // capital letter O, not digit 0
		// <ESC>OP,<ESC>OQ,<ESC>OR,<ESC>OS = F1..F4
	{
		c=input.Get();
		if (c<'P' || c>'S')
			THROWP(Scr::Connection::UnsupportedKey,DebugInfo);
		return Key::F1+ c-'P';
	}
	
	if (c == '[')
	{
		switch (input.Get()) // key after [
		{
		case 'A':
			return Key(Key::Up);
		case 'B':
			return Key(Key::Down);
		case 'C':
			return Key(Key::Right);
		case 'D':
			return Key(Key::Left);
		case '1': // 1,2,3,4,5, 7,8,9 =  F1..F8
			c=input.Get();
			EASSERTP(input.Get()==0x7e, /* != '~'*/
					 Scr::Connection::UnsupportedKey,DebugInfo);
			if (c=='6')
				THROWP(Scr::Connection::UnsupportedKey,DebugInfo);
			if (c>'9')
				THROWP(Scr::Connection::UnsupportedKey,DebugInfo);
			if (c>'6')c--; // code sequence 1,2,3,4,5, 7,8,9 (note,
						   // that 6 is omitted)
			return Key(Key::F1+ c-'1');
		case '2': // 0,1,3,4 =  F9..F12; only '~' - INS
			c=input.Get();
			if (c=='~')
				return Key(Key::Insert);
			if (c=='2')
				THROWP(Scr::Connection::UnsupportedKey,DebugInfo);			
			if (c>'4')
				THROWP(Scr::Connection::UnsupportedKey,DebugInfo);
			if (c>'2')
				c--;			
			if (input.Get()!=0x7e) // != '~'
				THROWP(Scr::Connection::UnsupportedKey,DebugInfo);
			return Key(Key::F9+ c-'0');
			
		case '3': // delete <ESC>[3~ - delete
			EASSERTP(input.Get()==0x7e, /* != '~'*/
					Scr::Connection::UnsupportedKey,DebugInfo);
			return Key(Key::Delete);
		case '5':
			EASSERTP(input.Get()==0x7e, /* != '~'*/
					Scr::Connection::UnsupportedKey,DebugInfo);
			return Key(Key::PageUp);

		case '6':
			EASSERTP(input.Get()==0x7e, /* != '~'*/
					Scr::Connection::UnsupportedKey,DebugInfo);
			return Key(Key::PageDown);
		case '7':// alternative coding
			EASSERTP(input.Get()==0x7e, /* != '~'*/
					Scr::Connection::UnsupportedKey,DebugInfo);
			return Key(Key::Home);
		case '8':// alternative coding
			EASSERTP(input.Get()==0x7e, /* != '~'*/
					Scr::Connection::UnsupportedKey,DebugInfo);
			return Key(Key::End);
		case 'F':
			return Key(Key::End);
		case 'H':
			return Key(Key::Home);

			
		default:
			;
				THROWP(Scr::Connection::UnsupportedKey,DebugInfo);
		}
	}
	
	input.UnGet();
	return Key(Key::Escape);
}

void Scr::VT100Compatible::RealGotoYX(const Position &  p)throw(ConnectionError)
{
	output
		<< CURSOR_YX(p.row+1,p.col+1);
}

void Scr::VT100Compatible::Refresh()throw(ConnectionError)
{
	boost::mutex::scoped_lock lck(refreshMutex);
	output << HIDE_CURSOR;
    output << CURSOR_HOME;
    DisplayStyle p=copyBuffer[0][0].style;
    Uint I=0,
	J=0;
    
    for (Uint i=0;i<controlBuffer.GetHeight();i++)//for each row
		for (Uint j=0;j<controlBuffer.GetWidth();j++)// for each col
		{
			if (controlBuffer[i][j].c != 0)
			{
				if (controlBuffer[i][j]!=copyBuffer[i][j])
				{
					if (i!=I || j!=J)
					{
						RealGotoYX(Position(i,j));
						I=i;
						J=j;
					}
					if (p!= controlBuffer[i][j].style)
					{
						p= controlBuffer[i][j].style;
						output << SET_ATTR(ATTR_RESET);
						output << SET_ATTR(static_cast<int>(p.GetFgColor()));
						output << SET_ATTR(static_cast<int>(p.GetBgColor()));
						if ((p.GetFgStyle()==Fg::Bright) !=0)
							output << SET_ATTR(ATTR_BRIGHT);
		    
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
			RealGotoYX(cursorPosition);
		output << SHOW_CURSOR;
	}
	
	output<<flush;
    if (!output.good())
    	THROW(ConnectionError);
    copyBuffer=controlBuffer;
}

void Scr::VT100Compatible::Resize(Uint rows, Uint cols)
    throw()
{
    GenericScreen::Resize(rows,cols);
    copyBuffer.Resize(controlBuffer.GetHeight(),
		      controlBuffer.GetWidth());
    copyBuffer.Fill(ScreenCharacter(0,DisplayStyle()));
}

void Scr::VT100Compatible::CleanUp()  throw(ConnectionError)
{
	output << SHOW_CURSOR;
	output<<SET_ATTR(ATTR_RESET)<<ERASE_SCREEN  << CURSOR_HOME;
}

Scr::VT100Compatible::~VT100Compatible()throw()
{
    RexIOLog(LogLevelModerate)<<"~VT100Compatible()"<<endl;
}

