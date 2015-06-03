#ifndef MAP_DATA_HPP_DEF__
#define MAP_DATA_HPP_DEF__
#include<rexio/tk/toolkit.h++>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>
#include "keymap.h++"
#include "pathfinder.h++"
#include "entity.h++"
class MapData;
class PlayerAssets;




//! minimum distance between two positions on hex grid
Scr::Uint Distance(Scr::Position s, Scr::Position f);

//! execute F1 within range r around p, limited by board size pmax
template < typename F >
void for_each_position (Scr::Position p, Scr::Size pmax, Scr::Uint r, F F1)
{
	for (int i = (int)p.row-(int)r ; i <= (int)p.row+(int)r ; i++)
		for (int j = (int)p.col-(int)r ; j <= (int)p.col+(int)r ; j++)
			if (i>=0 and j>=0 and i<(int)pmax.height and j<(int)pmax.width)
			{
				Scr::Position p2(i,j);
				if (Distance(p,p2)<=r)
					F1(p2);
			}// to be optimized
}



//! board, tiles, entities and so on
class MapData
{
public:
	static const char Oil;
	static const char Road;
private:
	//! 
	class MapField
	{
	private:
		char resources; //' ' - nothing
		// 'o' oil resources here!
		// 'w' oasis!
		SPE pE1; // something is here!

		char terrain_type; // '-' - road!
		// ' ' - plain wasteland
		//

		
		PlayerAssets * ZoI1; // Zone of Influence
	public:

		//! default ctor
		MapField();

		//! removes entity from specific field
		void RemoveEntity();
		//! insert entity
		void InsertEntity(SPE pE) {pE1=pE;}
		//! get SPE to entity (SPE is shared pointer)
		const SPE GetEntity() const
			{return pE1;}
		//! set influence
		void SetZoI (PlayerAssets * ZoI);
		//! who influences this field?
		PlayerAssets * GetZoI() {return ZoI1;}
		const std::string GetStatus() const; //description
		char GetResources() const {return resources;}
		void SetResources(char r) {resources=r;}
		char GetTerrainType() const {return terrain_type;}
		void SetTerrainType(char t) {terrain_type=t;}
		HP Attack(int damage);
	};
	boost::multi_array<MapField,2> Fields;
public:
	typedef std::list<std::tr1::shared_ptr<PlayerAssets> > PlayerList;
private:
	PlayerList PL1;
	PlayerList::iterator active; // who plays??
	int turn;
public:
	MapData();
	void Randomize(PlayerList& PL,int h, int w); // generate random  map

	//! Size of map (number of fields vertical/horizontal)
	Scr::Size GetSize();
	//! height of map
	Scr::Uint GetHeight();
	//! width of map
	Scr::Uint GetWidth();

	//! resources on specific field ('o' = oil, ' ' = nothing)
	char GetResources(int y, int x) const {return Fields[y][x].GetResources();}
	//! Statusbar description for field
	const std::string GetStatus(int y, int x) const {return Fields[y][x].GetStatus();}

	char GetTerrainType(int y, int x) const {return Fields[y][x].GetTerrainType();}
	void SetTerrainType(int y, int x,char t)
		{return Fields[y][x].SetTerrainType(t);}
	//! Cast attack on specific field
	//! \param y
	//! \param x coordinates of target
	//! \param damage - power of attack
	HP Attack(int y, int x,int damage);

	//! return entity on specific field
	const SPE GetEntity(int y, int x) const
		{
			return Fields[y][x].GetEntity();
		}
private:

	template <typename F>
	void ForEachEntityInZoI(Scr::Position p, F F1);
	// recalculate ZoI for entity of cordinates p
	void AcquireZoI (Scr::Position p);
	void VerifyZoI (Scr::Position p);
public:
	//! Insert entity to map
	void InsertEntity(int y, int x, SPE e,PlayerAssets * a)
		throw(Entity::IncorrectPosition);
	
	//! Remove entity from map
	void RemoveEntity(int y, int x);

	//! to != end() !!!!
	void MoveEntity(PathFinder::Path::iterator from,
					PathFinder::Path::iterator to);

	
	void RecalculateZoI(int y, int x, PlayerAssets * ZoI);
	
	void SetZoI(int y, int x, PlayerAssets * ZoI);
	
	PlayerAssets *  GetZoI(int y, int x)
		{
			return Fields[y][x].GetZoI();
		}

	//! end turn (pass control to next player)
	void SwitchTurn();


	//! Name of active player
	const std::string GetPlayerName() const ;

	//! full record of active player
	PlayerAssets * GetActive()
		{
			if (active==PL1.end())
				return 0;
			else
				return active->get();
		}
	//! turn number
	int GetTurn() {return turn;}
};



#endif
