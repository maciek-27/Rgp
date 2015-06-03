#ifndef MAP_HPP_DEF__
#define MAP_HPP_DEF__
#include<rexio/tk/toolkit.h++>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>
#include "mapdata.h++"
#include "keymap.h++"
class MapWindow;
class Map: public Scr::Tk::ActiveWidget
{
	friend class MapWindow;
private:
	Scr::Position coords;
	Scr::Position translation;
	Scr::Position cursorpos;
	std::auto_ptr<MapData> pMD1;

	Scr::Size D1S;
// 	unsigned D1Height;
// 	unsigned D1Width;
	
	typedef boost::multi_array<wchar_t,2 > Drawable;
	Drawable D1;
	std::tr1::shared_ptr<Entity> selected;
	void DrawHexGrid();
	void SetUpKeyMap();
public:
	using Widget::GetParent;
	Map();
	void OnStart()throw();
public:
	void OnRedraw(Scr::Screen &screen)throw();
private:
	void UpdateCursor();
	void UpdateTranslation();

	
	// warn if on edge
	bool EdgeWarning;
	// commands, that may be assigned to various keys
	// cursor movement
	void MoveN()throw();
	void MoveS()throw();
	void MoveW()throw();
	void MoveE()throw();
	void MoveSW()throw();
	void MoveNW()throw();
	void MoveSE()throw();
	void MoveNE()throw();
	// scrolling
	void ScrollUp()throw();
	void ScrollDown()throw();
	void ScrollLeft()throw();
	void ScrollRight()throw();
	// other
	void ShowHelp()throw();
	void SelectUnit()throw();
	void UpdateStatus();
	
public:
	void OnKeyDown(Scr::Key key)throw();
	virtual void OnFocus(FocusPolicy focustype)throw();
	PlayerAssets * GetActive()
		{
			if (pMD1.get())
				return pMD1->GetActive();
			else
				return 0; 
		}
			/*!
			  \return real height

			*/
	 Scr::Uint GetDrawableHeight() const throw();
			/*!
			  \return real width
			*/
	 Scr::Uint GetDrawableWidth() const throw();
	~Map()throw();
};


#endif
