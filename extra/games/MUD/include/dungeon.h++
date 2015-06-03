#ifndef __DUNGEON_BASIC_H__
#define __DUNGEON_BASIC_H__
#include <utility>
#include <string>
#include <memory>
#include <rexio/screen.h++>
#include "character.h++"
#include "evil_creatures.h++"
namespace MUD
{
	class DungeonMap;
	typedef std::pair<int,int> coord2d;
	// Dungeon class represents The Most Basic RPG concept, the
	// dungeon, or rather a game board. Dungeon is a massive structure
	// with multiple levels of concrete. Or a game board.
	class Dungeon
	{
		friend class GlobalGameStatus;
	private:
		static Dungeon * instance;
		Dungeon();
		std::auto_ptr<DungeonMap> DMap;
	public:
		// dungeon levels are numbered. 0 = ground level. 1= lower; 100 = even
		// lower MAX_INT = lowest; -1: theoretical buildings (not supported by
		// software; provided by API only in case of upgrade.

		// markers - in order of preference. "DEG" means, that entity
		// shall be labeled "D", but if "D" can't be allocated, label
		// "E" will be used.
		void AddMarker(pCharacter pChr);
		// remove marker from board.
		void DelMarker(pCharacter pChr);
		bool IsObstacle(coord2d coord, int level);

		// MARKERS
		// markers are all entities on board, that can be moved and
		// are saved between sessions
		
		void DrawScreen(Scr::Screen & scr, pCharacter pChr);
		static Dungeon & Instance();
		~Dungeon();
	};
}

#endif
