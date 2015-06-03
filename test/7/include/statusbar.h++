#ifndef SB_HPP_DEF__
#define SB_HPP_DEF__
#include<rexio/tk/toolkit.h++>
#include <string>
#include <map>
#include <boost/function.hpp>
//! displays basic information
//! about... anything
class StatusBar: public Scr::Tk::Widget
{
private:
	//! message
	static std::string Status;
public:
	typedef std::map<Scr::Uint,boost::function<void ()> > KeyMap;
	void OnRedraw(Scr::Screen &screen)throw();
	//!status is global
	inline static void SetStatus(std::string s)
		{
			Status.assign(s);
		}
	void   OnKeyDown(Scr::Key key)throw();
};

#endif
