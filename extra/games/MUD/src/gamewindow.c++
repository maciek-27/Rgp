#include "gamewindow.h++"
#include "dungeonview.h++"
#include <algorithm>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include "welcomescreen.h++"
const int MUD::WelcomeScreen::LoginForm::min_height=7;
const int MUD::WelcomeScreen::LoginForm::min_width=20;
using namespace MUD;
using namespace Scr;
using namespace boost::lambda;


GameWindow::GameWindow(std::istream & in, std::ostream & out) :
	RootWindow(in, out)
{
}
void GameWindow::OnStart() throw ()
{
	AddWidget(* new
			  WelcomeScreen(GetHeight(), GetWidth(), GetStyle()));
}

void GameWindow::OnKeyDown(Scr::Key  key) throw ()
{
	if (key==4)
		Exit(1);
	RootWindow::OnKeyDown(key);
}
void GameWindow::OnResize() throw ()
{
}
void GameWindow::OnRedraw(Scr::Screen & scr) throw ()
{
	if (! elements.empty())
	{
		elements.back()->SetPosition(0,0);
		elements.back()->SetSize(GetSize());
	}

	RootWindow::OnRedraw(scr);
}

GameWindow::~GameWindow()throw ()
{
	std::for_each(elements.begin(),elements.end(), delete_ptr());
}
