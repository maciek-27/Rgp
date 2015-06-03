#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <rexio/tk/toolkit.h++>

using namespace Scr;
using namespace Scr::Tk;

class RexLogo:public Widget
{
public:
	void OnRedraw(Screen &scr)throw() {
		scr << GetStyle() << Control::GotoYX(0, 0) <<
			"  ________  ^__^" << Control::GotoYX(1, 0) <<
			" / RexIO? \\ (oo)\\_______" << Control::GotoYX(2, 0) <<
			" ---------- (__)\\       )\\/\\" <<Control::GotoYX(3, 0) <<
			"                ||----w |" <<Control::GotoYX(4, 0) <<
			"                ||     ||" ;
	}
	RexLogo()throw() : Widget(5, 30) { ; }
	RTTI_OBJ(RexLogo, Widget);
};

class WelcomeWindow:public FramedWindow
{
public:
	Label topmsg;
	Label info[16];
	RexLogo logo;

	WelcomeWindow()throw();
	void OnRedrawInside(Screen &scr)throw();
};

class Manager:public RootWindow
{
	WelcomeWindow welcome;
public:
    Manager();
    void OnRedraw(Screen &scr)throw();
	void OnResize()throw();
    void OnStart()throw();
    void OnKeyDown(Key key)throw();
};

#endif // __MANAGER_H__
