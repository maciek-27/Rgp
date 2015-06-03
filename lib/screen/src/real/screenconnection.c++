#include "screen.h++"
#include "screenbuffer.h++"
#include "genericscreen.h++"
#include "throw.h++"
#include "keyboard.h++"
#include "screenconnection.h++"

Scr::__ScreenConnection::__ScreenConnection(Connection & _connection,std::istream & _input)
    throw()
	:
    exitcode(0),connection(_connection),active(false){;}

void Scr::__ScreenConnection::ExitConnection(int _code)
{
	exitcode=_code;
	active = false;
}

Scr::__ScreenConnection::~__ScreenConnection()throw(){;}

