#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <iostream>
#include <signal.h>
#include"screen.h++"
#include"screenbuffer.h++"
#include"genericscreen.h++"
#include"localscreen.h++"
#include <stdlib.h>
using namespace std;
#include <boost/thread/mutex.hpp>

namespace {
	boost::mutex M;
	Scr::LocalScreen * ls=0;
	sighandler_t osh=0;
	void sh(int i)
	{
		ls->TestForResize();
	}
	
}



Scr::LocalScreen::LocalScreen(Connection & _connection,
							  std::istream & _input,
							  std::ostream & _output)throw()
    :GenericScreen(_input,_output),__ScreenConnection(_connection,_input)
{
    
    tcgetattr(fileno_hack(_output), &term);
	term.c_cc[VMIN] = 1;
    term.c_lflag &= ~(ECHO | ICANON); // disable echo on terminal
    tcsetattr(fileno_hack(_output), 0, &term);
    _input.sync_with_stdio(false);
    _output.sync_with_stdio(false);
	/*!
	  please note, that, turning sync. off for cin  may be detected as memory
	  leak by valgrind debugger. According to GNU folks thic behaviour
	  is normal (since desynchronizing means allocating special memory
	  block, which is never freed as standard streams are never deleted)
	  http://gcc.gnu.org/ml/gcc-bugs/2006-06/msg00824.html
	 */
    RexIOLog(LogLevelModerate)<<"LocalScreen(std::ostream & _output)"<<endl;
	boost::mutex::scoped_lock Lock(M);

	if (!ls)
	{
		ls=this;
#ifdef SIGWINCH
		osh=signal(SIGWINCH,sh);
#endif
	}
}

/*function copied from original telnet client*/
void Scr::LocalScreen::TestForResize() {
	boost::mutex::scoped_lock Lock(M);
	int infd=input.FD();
    struct winsize ws;
    bool result;
    if (ioctl(infd, TIOCGWINSZ, (char *)&ws) >= 0) {
		if (GetHeight()==ws.ws_row && GetWidth()==ws.ws_col)
			result= false;
		Resize(ws.ws_row,ws.ws_col);
		result= true;
    }
	
    result= false;
	connection.OnResize(GetHeight(),GetWidth());
}

const char * Scr::LocalScreen::GetType() const throw()
{
	return getenv("TERM");
}

int Scr::LocalScreen::ProcessConnection()
{
    connection.OnStart();
    active=true;
    char counter = 0;
    while (input.Stream().good() && active)
    {
		usleep(1);
		if (!counter) // test at most in every 255 000 useconds
			TestForResize(); // (CPU savings without
		//  usability loss)
		// - this is required, when system does not provide SIGWINCH

//		boost::mutex::scoped_lock Lock(M);
		// returns true if we have anything to read
		if (input.KbHit())
		{
			input.Buffer();
			do
			{
				try
				{
					Key k = DecodeKeyPressed();
					if (k==0x1b)
					{
						input.UnGet();
						k = DecodeKeyPressed();
					}
					connection.OnKeyDown(k);
				}
				catch (Scr::Connection::UnsupportedKey)
				{
					while (input.HasBufferedText())
						input.Get();
				}
				catch (Scr::Screen::InvalidUTF8)
				{
					active = false;
					break;
				}
			}
			while (input.HasBufferedText());
		}			
		counter++;
    }
    CleanUp();
    return exitcode; // OnExit is called by Connection::Start()
}

Scr::LocalScreen::~LocalScreen()throw()
{
    term.c_lflag |= ECHO | ICANON; // reenable echo on terminal
    tcsetattr(fileno_hack(output), 0, &term);
    RexIOLog(LogLevelModerate)<<"~LocalScreen()"<<endl;
}

