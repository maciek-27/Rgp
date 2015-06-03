#ifndef __EVIL__CREATURES__H__
#define __EVIL__CREATURES__H__

namespace MUD {

	// by its name this class is responsible for supporting all kinds
	// of evil creatures (mobs) and therefore for making this game alive.
struct SupportForEvilCreatures
{
	// create object of this class to enable support of evil creatures
	SupportForEvilCreatures();
	// delete this object to disable support for evil creatures;
	~SupportForEvilCreatures();
	// this heavily depends on Dungeon object and should be its
	// member. 
};

}
#endif
