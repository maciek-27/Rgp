#include <rexio/net.h++>
#include <signal.h>
#include <cstdlib>
#include "gamewindow.h++"
#include "globalgamestatus.h++"
using namespace std;

// global server object
namespace
{
RexIO::Networking::Server<MUD::GameWindow> s;
void StopServerImmediately(int x)
{
	cout << "OK,ok... I'm done." << endl;
	exit(4);
}
void StopServer(int x)
{
	signal(SIGINT, StopServerImmediately);
	cout << "signal caught. stopping." << endl;
	s.Stop();
}
}


int main (int argc,char ** argv) {

    signal(SIGPIPE, SIG_IGN);	//disable signal (app has other ways
				//of detecting connection errors)
    signal(SIGINT, StopServer);
    int port = 5000;
	cout << "setting up core ruleset:" << endl;
	{
		MUD::GlobalGameStatus GGS1;
		cout << "opening port " << port << '.' << endl;
		cout << "^C or kill -INT {pid} to stop server." << endl;
		s.Start(port);
		cout << "Server stopped" << endl;
	}
	cout << "All global services stopped" << endl;
    cout << "Game over (clean shutdown)" << endl;
    return 0;
}
/*end of main function of program*/
