#include"screen.h++"
#include<iostream>
#include"screenbuffer.h++"
#include"genericscreen.h++"
#include"vt100compatible.h++"
#include"localscreen.h++"
#include"remotescreen.h++"
#include"screenconnection.h++"
#include<sys/ioctl.h>
#include"fileno_hack.h++"
#include"core.h++"
#include"throw.h++"
#include"keyboard.h++"
#include"terminfo.h++"
#include"terminfoenabled.h++"

#ifndef	TIOCGWINSZ
#error "TIOCGWINSZ not supported"
#endif	/* TIOCGWINSZ */

Scr::Connection::Connection(std::istream & _input, std::ostream & _output)
		throw()
{
    int ofd = fileno_hack(_output);
    // ... 
    // terminal connection type detection here

	try
	{
		TI::TerminfoCore::Initialize();
		if (isatty(ofd)) {
			screen = std::auto_ptr<Screen>
				(
					new RScreen<LocalScreen,TerminfoEnabledScreen>
					(*this,_input,_output)
					);
			/* make sure that terminfo entry exists - otherwise screen ops may fail */
			const TI::TerminfoEntry * entry =
				const_cast<Scr::TI::TerminfoEntry*>
				(&TI::TerminfoCore::GetTerminfo(
					screen->GetType()));
		} else {
			screen = std::auto_ptr<Screen>
					(
						new RScreen<RemoteScreen,TerminfoEnabledScreen>
							(*this,_input,_output)
					);
		}
	}
	catch (TI::DatabaseException)
	{
		//Failed to initialize TERMINFO driver, use fallback implementation
		//instead (VT100Compatible is class implementing screen operations
		//using hardcoded I/O sequences - works quite well for most popular
		//terminal types.)
		if (isatty(ofd))
			screen = std::auto_ptr<Screen>
					(
						new RScreen<LocalScreen,VT100Compatible>
							(*this,_input,_output)
					);
		else
			screen = std::auto_ptr<Screen>
					(
						new RScreen<RemoteScreen,VT100Compatible>
							(*this,_input,_output)
					);
	}
	catch (...)
	{
		//other posible reason of failure - no idea what to do. leave 
		//screen==NULL - Connection::Start will return StartFailed,
		//but nothing bad will happen. Nothing at all will happen  if
		//Connection::Start will never be called.
		abort();
		// (it's auto_ptr, and by default it points "to nothing")
	}
}
//end of Scr::Connection::Connection
//   (std::istream & _input, std::ostream & _output)
		
int Scr::Connection::Start(int argc, char **argv)
	throw(StartFailed,Screen::IllegalCharacter)
{
	return Start();
}

int Scr::Connection::Start()
	throw(StartFailed,Screen::IllegalCharacter)
{
    int code;
    __ScreenConnection * sc = dynamic_cast<__ScreenConnection*>( screen.get());
    if (sc)
    {
		try
		{
			code =sc->ProcessConnection();
		}
		catch (Broken)
		{
			throw; // controlled rethrow
		}
    }
    else
		THROW(StartFailed);
	
    OnExit(code);
    return code;
}

void Scr::Connection::Exit(int code)throw(StopFailed)
{
    __ScreenConnection * sc = dynamic_cast<__ScreenConnection*>(screen.get());
    if (sc)
    {
		sc->ExitConnection(code);
    }
    else
		THROW(StopFailed);
}

void Scr::Connection::OnStart()throw()
{
    ;
}

void Scr::Connection::OnResize(Uint rows, Uint cols)throw()
{
    ;
}

void Scr::Connection::OnKeyDown(Key key)throw()
{
    ;
}

void Scr::Connection::OnExit(int code)throw()
{
    ;
}

Scr::Connection::~Connection()throw()
{
    ;//screen destroyed by auto_ptr
//	TI::TerminfoCore::FreeTerminfoEntry(); called by destructor of *screen, if
//  it is TerminfoEnabledScreen
}
