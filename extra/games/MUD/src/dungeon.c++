#include "dungeon.h++"
#include <map>
#include <iostream>
#include <cstdlib>
#include <boost/thread/mutex.hpp>
using namespace MUD;
namespace
{
	const int array_dimension = 3;

	struct field
	{
	private:
		bool obstacle;
		pCharacter pChar;
	public:
		field() :obstacle (rand()%100>84) {}
		pCharacter GetCharacter() const { return pChar ; }
		void SetCharacter(pCharacter _pChar) { pChar=_pChar; }
		void UnsetCharacter() {pChar.reset();}
		wchar_t TerrainClass()
			{
				if (obstacle) return '#'; else return ' ' ;
			}
		bool IsObstacle() {return pChar.get()!=0 or obstacle;}
	};

	struct multi_dimensional_array
	{
		field array [1<<array_dimension] [1<<array_dimension] ;
	};
	// primary in-memory format  of map data is group of sub-arrays of
	// 2^array_dimension^2 elements (square tiles). Logically array is
	// infinite.
}
namespace MUD
{
	struct DungeonMap
	{
	private:
		std :: map < std :: pair < int , int > , multi_dimensional_array > a;
	public:
		boost::mutex mtx;
		field & get_field(int y, int x)
			{
				return 
				a[std::make_pair(y>>array_dimension, x>>array_dimension)]
					.array[y- ((y>>array_dimension)<<array_dimension)]
					[x- ((x>>array_dimension)<<array_dimension)];
			}
		~DungeonMap()
			{
				boost::mutex::scoped_lock lock(mtx);
			}
	};
		
}
Dungeon * Dungeon::instance=0;
Dungeon::Dungeon():DMap(new DungeonMap)
{
//	boost::mutex::scoped_lock lock(mtx);
	if (instance!=0)
	{
		using namespace std;
		cout << "Violation od Dungeon SINGLETON pattern. Aborting program." << endl;
		abort();
	}
	instance=this;
}
Dungeon::~Dungeon()
{
//	boost::mutex::scoped_lock lock(mtx);
	instance=0;
}

Dungeon & Dungeon::Instance()
{
	if (instance==0)
	{
		using namespace std;
		cout << "Instance() won't initialize this Dungeon object, "
			"ad it is - by design - to be manually initialized from"
			" GlobalGameStatus constructor." << endl;
		abort();
	}
	return *instance;
}

void Dungeon::AddMarker(pCharacter pChr)
{
	boost::mutex::scoped_lock lock((*DMap).mtx);
	(*DMap).get_field(
		pChr->GetPosition().first,pChr->GetPosition().second).SetCharacter(pChr);
}


void Dungeon::DelMarker(pCharacter pChr)
{
	boost::mutex::scoped_lock lock((*DMap).mtx);
	(*DMap).get_field(
		pChr->GetPosition().first,pChr->GetPosition().second).UnsetCharacter();	
}

bool Dungeon::IsObstacle(coord2d coord, int level)
{
	boost::mutex::scoped_lock lck((*DMap).mtx);
	return (*DMap).get_field(coord.first, coord.second).IsObstacle();
}

void Dungeon::DrawScreen(Scr::Screen & scr, pCharacter pChr)
{
	boost::mutex::scoped_lock lock((*DMap).mtx);

	coord2d left_top_corner_in_game_coordinates =
		pChr->GetPosition();
	Scr::Position center(scr.GetHeight()/2 , scr.GetWidth()/2);
	left_top_corner_in_game_coordinates.first -= scr.GetHeight() / 2;
	left_top_corner_in_game_coordinates.second -= scr.GetWidth() / 2;
	for ( unsigned y = 0 ; y < scr.GetHeight() ; ++y )
	{
		for ( unsigned x = 0 ; x < scr.GetWidth() ; ++x )
		{
			field & f=
				(*DMap).get_field(y+left_top_corner_in_game_coordinates.first,
									 x+left_top_corner_in_game_coordinates.second);
			pCharacter pChr1 = 
				f.GetCharacter();
			scr.GotoYX (y,x);
			
			if (pChr1.get())
			{
				scr.AddCharacter(pChr1->GetName()[0]);
			}
			else
				scr.AddCharacter(f.TerrainClass());
		}
	}
	scr.GotoYX(0,0);
  	scr << pChr->GetName() << " | " 
		<< "Coords: " << pChr->GetPosition().first << ':' << pChr->GetPosition().second
  		<< " | type 'Q' to leave";
	scr.ForceCursorPosition(center);
}
