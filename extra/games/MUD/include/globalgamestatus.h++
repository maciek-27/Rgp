#ifndef __GLOBAL_GAME_STATUS_H__
#define __GLOBAL_GAME_STATUS_H__
#include "dungeon.h++"
#include "autorefreshfacility.h++"
namespace MUD
{
	struct GlobalGameStatus
	{
		Dungeon s1; // global map
		SupportForEvilCreatures s2; // management for AI
		StoredCharacters s3; // saving/storing characters
		AutoRefreshFacility s4; // screen refresh routined
	};

}

#endif
