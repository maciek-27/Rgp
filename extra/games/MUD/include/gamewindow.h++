#ifndef __MUD__GAMEWINDOW_H__
#define __MUD__GAMEWINDOW_H__

#include <utility>


#include <rexio/tk/rootwindow.h++>
namespace MUD {

class GameWindow:public Scr::Tk::RootWindow
{
public:
	GameWindow(std::istream & in,
			   std::ostream & out);
	void OnStart() throw ();
	void OnKeyDown(Scr::Key  key) throw ();
	void OnResize() throw ();
	void OnRedraw(Scr::Screen & scr) throw ();
	~GameWindow()throw ();
};



}
#endif
