/* 
 * File:   dungeonview.h++
 *
 * Created on March 29, 2009, 6:11 PM
 */

#ifndef _DUNGEONVIEW_H__
#define	_DUNGEONVIEW_H__
#include <rexio/tk/activewidget.h++>
#include "character.h++"
namespace MUD
{
typedef std::pair<int,int> ccoords;
/**
 * DungeonView is class implementing core roguelike gamefunctionality:
 * it presents map and all other character-related features (including
 * inventory etc). This class is responsible for input processing during
 * realtime gameplay
 */
class DungeonView:public Scr::Tk::ActiveWidget
{
	// position of player character in in-game coordinate system
	pCharacter pChr;
public:
	DungeonView(pCharacter pch);
	void OnStart() throw();

	// arrows and so on. notify other instances of DungeonView when game status
	// is changed
	void OnKeyDown(Scr::Key  key) throw ();

	void OnRedraw(Scr::Screen & scr) throw ();
	~DungeonView()throw ();
};
}

#endif	/* _DUNGEONVIEW_H__ */

