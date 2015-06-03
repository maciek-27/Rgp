#include <iostream>
#include<cstring>
#include "rootwindow.h++"
#include "trace.h++"

using namespace Scr::Tk;

void RootWindow::OnStart()throw()
{
    Window::OnStart();
    OnRedraw(*screen);
    *screen<<Control::Refresh;

    RexIOLog(LogLevelLow)<<"RootWindow::OnStart()throw()"<<std::endl;
}

void RootWindow::OnKeyDown(Key key)throw()
{
	if(key == Key::Tab) 
	{
		OnFocus(TabFocus);
	}
	Window::OnKeyDown(key);

    OnRedraw(*screen);
    //*screen<<Control::Refresh;
}

RootWindow::RootWindow(std::istream & _input, std::ostream & _output,
		       const DisplayStyle & _style)throw():
    Connection(_input, _output),
    Window(screen->GetHeight(), screen->GetWidth(), _style)
{
	SetParent(*this);
}

Scr::Screen& RootWindow::GetScreen()throw()
{
    return *screen;
}

Scr::Uint RootWindow::GetAbsoluteColumn()throw(){return 0;}
Scr::Uint RootWindow::GetAbsoluteRow()throw(){return 0;}

RootWindow& RootWindow::GetRootWindow()throw()
{
	return *this;
}

void RootWindow::OnRedraw(Screen& screen)throw()
{
	screen.HideCursor() ;
	Window::OnRedraw(screen);
	screen.Refresh();
}

void RootWindow::OnResize(Uint rows, Uint cols)throw()
{
	size = Size(rows,cols);
    OnResize();
	OnRedraw(*screen);
}

void RootWindow::LoadStylesheet(const char* filename)
	throw(FileNotOpened, Stylesheet::ParsingError)
{
	
	std::fstream fs(filename);
	if(!fs.is_open())
		THROW(FileNotOpened);
	SetStylesheet(new Stylesheet(fs));
}

int RootWindow::Start(int argc, char **argv)
	throw(StartFailed,Screen::IllegalCharacter)
{
	for(int i = 0;i<argc;i++) {
		// strlen("-style=") == 7
		if(strncmp(argv[i], "-style=", 7) == 0) {
			LoadStylesheet(argv[i]+7);
		}
	}

	return Connection::Start(argc, argv);
} 

int RootWindow::Start()throw(StartFailed,Screen::IllegalCharacter)
{
	return Connection::Start();
}

void RootWindow::ForceRepaint()throw()
{
	screen->GotoYX(0,0);
	screen->Rectangle((wchar_t)0xE007,size);
    screen->Refresh();
    OnRedraw(*screen);

}
RootWindow::~RootWindow()throw(){;}

