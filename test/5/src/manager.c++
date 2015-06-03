#include <rexio/screen.h++>
#include "manager.h++"
#include "main.h++"
#include <iostream>
#include <iomanip>

using namespace Scr;
using namespace Scr::Tk;

WelcomeWindow::WelcomeWindow()throw()
	:FramedWindow(20, 50), topmsg("Welcome to RexIO demo application!")
//, 
//									DisplayStyle(Fg::Red, Fg::Dark, Bg::Yellow),
//									FrameStyle(DisplayStyle(Fg::Red, Fg::Dark, Bg::Green)))
{
	topmsg.objectName="welcome";
	AddWidget(topmsg);
	topmsg.SetPosition(1, 1);

	info[0].SetText("This demo will show a sample network chat");
	info[1].SetText("application with console user interface");
	info[2].SetText("streamed over ordinary telnet application.");
	info[3].SetText("");
	info[4].SetText("");
	info[6].SetText("Use telnet to connect to the above port.");
	info[7].SetText("");
	info[8].SetText("");
	info[9].SetText("");
	info[10].SetText("");
	info[11].SetText("");
	info[12].SetText("");
	info[13].SetText("");
	info[14].SetText("");
	info[15].SetText("Enjoy!");

	for(int i = 0;i< 16;i++) {
		info[i].SetStyle(DisplayStyle(Fg::Transparent, Fg::Dark, 
									  Bg::Transparent));
		AddWidget(info[i]);
		info[i].SetPosition(i + 3, 1);
		info[i].SetSize(1, 47);
		std::stringstream ss;
		ss << "info" << i;
		info[i].objectName = ss.str();
	}

	AddWidget(logo);
	logo.SetPosition(11, 9);
}

void WelcomeWindow::OnRedrawInside(Scr::Screen &scr)throw()
{
	FramedWindow::OnRedrawInside(scr);
//	scr << Control::Refresh;
}

Manager::Manager(): RootWindow(std::cin, std::cout),
					welcome()
{
	
	AddWidget(welcome);
}

void Manager::OnResize()throw()
{
	try
	{
	welcome.SetPosition((GetHeight() - welcome.GetHeight())/2, 
						(GetWidth() - welcome.GetWidth())/2);   
	}catch(...){;}// exception may be thrown if OnResize called   before OnStart()
}

void Manager::OnRedraw(Scr::Screen &scr)throw()
{
	RootWindow::OnRedraw(scr);
}

void Manager::OnStart()throw()
{
	std::stringstream ss;
	ss << ::port;	
	welcome.info[5].SetText("Port: " + ss.str());

    pthread_mutex_unlock(&startSequenceMutex);
}

void Manager::OnKeyDown(Scr::Key key)throw()
{
    if (key=='q') Exit(0);
}
