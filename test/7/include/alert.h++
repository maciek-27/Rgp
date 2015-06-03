#ifndef ALERT_HPP_DEF__
#define ALERT_HPP_DEF__
#include<string>
#include<rexio/tk/window.h++>

//! display simple nonmodal text window with message and button "OK"
//! \param parent where do we want to place message?
//! \param message text to display
//! \param _style as for Window constructor
void Alert(Scr::Tk::Window & parent, const std::string & message,
		   const Scr::DisplayStyle &  _style
		   = Scr::DisplayStyle(Scr::Fg::White,
							   Scr::Fg::Dark,
							   Scr::Bg::Black));
#endif
