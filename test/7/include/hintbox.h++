#ifndef HBX_HPP_DEF
#define HBX_HPP_DEF
#include<rexio/tk/toolkit.h++>
#include <list>

class HintBox: public Scr::Tk::Window
{
public:
	typedef std::list <std::string> Hints;
	static Hints H1;
	HintBox() : Scr::Tk::Window(1,1) {}
	// height depends on content, while width is set by parent
	
	void OnRedraw(Scr::Screen &screen)throw();

};
#endif
