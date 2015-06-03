#include<iostream>
#include "mapdata.h++"
#include "statusbar.h++"
#include "hintbox.h++"
#include "pathfinder.h++"
#include <vector>
#include <cstdlib>
#include <ctime>
#include<sstream>
#include <algorithm>
#include <functional>
#include <memory>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/if.hpp>
#include <boost/lambda/algorithm.hpp>
#include "playerassets.h++"
#include<ext/functional>

#include <boost/thread/mutex.hpp>
#include <rexio/keyboard.h++>

#include "entity_types.h++"
#include "entity.h++"

using namespace std;
using namespace boost::lambda;
#if defined _GNU_SOURCE
using __gnu_cxx::compose2;
using __gnu_cxx::compose1;
#endif

Scr::Uint Distance(Scr::Position s, Scr::Position f)
{
	// a bit more complex formula than sqrt((Ax-Bx)^2+(Ay-By)^2), as
	// we are not in relam of Euclidean geometry when processing hex
	// board. (luckily no roots here: only basic arithmetic and logic)
	using namespace Scr;
	Sint i_result_x = s.col-f.col;
	Sint i_result_y = s.row-f.row;
	Uint result_x = abs(i_result_x);
	Uint result_y = abs(i_result_y);
	
	bool column_odd = s.col & 1;

	result_y+=// increment if expression evaluates true
		( ( ( f.col & 1 ) != column_odd ) // both odd or both even
		  and ( ( f.row < s.row ) == column_odd ) );
			

	Uint rx2 = (result_x>>1) + (result_x&1);// ceil(result_x/2.0);
	if (rx2<=result_y)
		result_y-=rx2;
	else
		result_y=0;
	return result_x+result_y;
}



// 	const std::string & GetHPDesc() const  // damage level
// 		{
// 			switch (hp)
// 			{
// 			case Nothing:
// 				static const string S1("OK");
// 				return S1;
// 			case LightlyDamaged:
// 				static const string S2("wounded");
// 				return S2;
// 			case HeavilyDamaged:
// 				static const string S3("heavily wounded");
// 				return S3;
// 			case Destroyed:
// 			default:
// 				static const string S4("mortally wounded");
// 				return S4;
// 			}			
// 		}	

const char MapData::Oil = 'o';
const char MapData::Road = '=';

MapData::MapField::MapField():resources(' '),terrain_type(' '),ZoI1(0)
{

}

void MapData::MapField::RemoveEntity()
{
	pE1.reset();
}

void MapData::MapField::SetZoI (PlayerAssets * ZoI)
{
	if (pE1.get() and ZoI!=ZoI1)
		if (not pE1->GetMorale())
	{// for entities w/o Morale: surrender to new player
		pE1->SetAllegiance(ZoI);
		ZoI->InsertEntity(pE1);
		if (ZoI1)
			ZoI1->RemoveEntity(pE1);
	}
	ZoI1=ZoI;
	
}

const std::string MapData::MapField::GetStatus() const
{
	if (pE1)
		return pE1->GetName() +
			((pE1->GetAllegiance())?
			 ( string(" (")+pE1->GetAllegiance()->GetName() + ')' ):(""))
			+ ((resources==MapData::Oil)?(", oilfield."):(""));
	if (resources==MapData::Oil)
		return "Oilfield here!";
	else
		return "Nothing worth looking:(";

	
}

HP MapData::MapField::Attack(int damage)
{
	if (not pE1)
		return Nothing;
	else
	{
		HP hp=pE1->AcceptAttack(damage);
		return hp;
	}
}


MapData::MapData(): Fields(boost::extents[0][0])
{
	// initialize as empty map (must manually fill it or randomize to
	// get anything playable
}

void MapData::Randomize(PlayerList & PL,int h, int w)
{
	// Function object, that creates roads
	class
	{
	private:
		auto_ptr<PathFinder> PF1;
	public:
		void operator() (MapData * pMD1,
						 const Scr::Position & P1,
						 const Scr::Position & P2)
			{
				using Scr::Position;
				if (!PF1.get())
					PF1.reset(new
							  PathFinder(
								  pMD1,P1,P2,
								  // "Can pass?" predicate
								  not bind(&SPE::get,
										   bind(&MapData::GetEntity,
												pMD1,
												bind(&Position::row,_1),   
												bind(&Position::col,_1)   
											   )),
								  // cost calculation (build as few
								  // roads as possible)
								  if_then_else_return(
									  bind(&MapData::GetTerrainType,
										   pMD1,
										   bind(&Position::row,_1),  
										   bind(&Position::col,_1) 
										  )==MapData::Road,1,8)));
				else
					PF1->ConditionalReset(P1,P2);
				// for each tile in path
				for_each(PF1->GetPath().begin(),PF1->GetPath().end(),
						 // set terrain type to road
						 bind(&MapData::SetTerrainType,
							  pMD1,
							  bind(&Position::row,_1),  
							  bind(&Position::col,_1),
							  MapData::Road
							 ));
			}
// 		void operator() (MapData * ,
// 						 const Scr::Position &,
// 						 const Scr::Position &);
	}Road;
	unsigned int RNGState = time(0);
	Fields.resize(boost::extents[h][w]);
	using Scr::Position;
	// insert up to n interesting items;
	for ( unsigned int
			  i = 0,
			  n = max<unsigned>(1,(GetWidth()*GetHeight())/200) ;
		  i!= n ;
		  ++i )
	{
		Position P1(rand_r(&RNGState)%GetHeight(),
					rand_r(&RNGState)%GetWidth());
		Position P2(GetHeight()-P1.row-1,GetWidth()-P1.col-1);
		Fields[P1.row][P1.col].SetResources('o');
		Fields[P2.row][P2.col].SetResources('o');
		
		Road(this,P1,P2);

		InsertEntity(P1.row,P1.col,
					 SPE(new OilWell),0);
		InsertEntity(P2.row,P2.col,
					 SPE(new OilWell),0);
	}
	Position pla1start(3,3);
	Position pla2start(GetHeight()-4,GetWidth()-4);
	Road(this,pla1start, pla2start);
	
	turn = 1;
	PL1=PL;
	assert(PL1.size()==2);
	PlayerList::iterator pla2 = PL1.begin();
	PlayerList::iterator pla1 = pla2++;// pla1 contains first element,
									   // while pla2 is incremented
	// Destruction of HQ is key to winning the game!
	InsertEntity(pla1start.row, pla1start.col,
				 tr1::shared_ptr<Entity>(new HQ), pla1->get());
	
 	InsertEntity(pla2start.row, pla2start.col,
				 tr1::shared_ptr<Entity>(new HQ), pla2->get());
	
 	(*pla1)->SetColor(Scr::Fg::Green);
 	(*pla2)->SetColor(Scr::Fg::Cyan);
	active=PL1.begin();
	(*active)->TurnBegin();
}


Scr::Size MapData::GetSize()
{
	return Scr::Size(GetHeight(),GetWidth()); 
}
//! height of map
Scr::Uint MapData::GetHeight()
{
	return Fields.shape()[0];
}
//! width of map
Scr::Uint MapData::GetWidth()
{
	return Fields.shape()[1];
}


HP MapData::Attack(int y, int x,int damage)
{
	HP result(Fields[y][x].Attack(damage));
	if (result==Destroyed)
	{
		RemoveEntity(y,x);
	}
	return  result;
}

//! range of character influence
static const int zoi_size = 5;

template <typename F>
void MapData::ForEachEntityInZoI(Scr::Position p, F F1)
{
	using namespace Scr;
	SPE pE1;
	for_each_position(
		p, GetSize(), zoi_size,
		if_then( /* explicitly refer to the operator=, as it is overloaded*/
			bind ( (SPE &(SPE::*)(const SPE &))&SPE::operator=,
				   &pE1, bind (&MapData::GetEntity,this,
							   bind(&Position::row,_1),
							   bind(&Position::col,_1))
				)	// if there is an entity on
					// specific field
			,
			if_then(
				bind (&Entity::GetMorale,
					  bind(&SPE::get,&pE1))
					// and it has Morale
				and bind (&Entity::GetAllegiance,
						  bind(&SPE::get,&pE1))
					// and it is not neutral
				,	// then
				F1//finally: execute function
				)
			)
		);
}//void MapData::ForEachEntityInZoI(Scr::Position p, F F1)


void MapData::RecalculateZoI(int y, int x, PlayerAssets * ZoI)
{
	using namespace Scr;
	typedef vector < pair < Scr::Position, PlayerAssets * > > ZoIFinder;
	typedef ZoIFinder::value_type Tag;
	ZoIFinder ZF1;
	ZF1.reserve(zoi_size*zoi_size+zoi_size*2+1);
	// FIND ALL ENTITIES INFLUENCING SPECIFIC FIELD
	Position p(y,x);

	ForEachEntityInZoI(// add coordinates and their owner to ZF1
		p, bind(&ZoIFinder::push_back,&ZF1,
				bind(constructor<Tag>(), _1,// boost functor to
											// construct object on-the-fly
					 bind(&Entity::GetAllegiance,
						  bind(&SPE::get,// raw pointer required
										 // when referencing
										 // member function
							   bind (&MapData::GetEntity,this,
									 bind(&Position::row,_1),
									 bind(&Position::col,_1)))))));

	if (ZF1.empty())// if nothing found, just SetZoi :)
		Fields[y][x].SetZoI(ZoI);
	else
	{
		// otherwise SELECT CLOSEST ONE
		ZoIFinder::iterator i =
			min_element(ZF1.begin(),ZF1.end(),// strict weak ordering
											  // of map coordinates
											  // based on distance and ZoI
						  (bind(Distance,Position(y,x),bind(&Tag::first,_1))<<1)
						 +(bind(&Tag::second,_1)==ZoI)
					<	  (bind(Distance,Position(y,x),bind(&Tag::first,_2))<<1)
						 +(bind(&Tag::second,_2)==ZoI));
		// if two units have equal distance, old owner win (+Tag::second)
		
		// IT'S OWNER WILL NOW INFLUENCE THIS FIELD
		Fields[y][x].SetZoI(i->second);
	}	   
}
//void MapData::RecalculateZoI(int y, int x, PlayerAssets * ZoI)

void MapData::SetZoI(int y, int x, PlayerAssets * ZoI)
{
	if (GetZoI(y,x) and GetZoI(y,x)!=ZoI)
		RecalculateZoI(y,x,ZoI);
	else
		Fields[y][x].SetZoI(ZoI);
}

inline void MapData:: AcquireZoI (Scr::Position p)
{
	for_each_position(p,GetSize(),zoi_size,
					  bind(&MapData::SetZoI,this,
						   bind(&Scr::Position::row,_1),
						   bind(&Scr::Position::col,_1),
						   GetEntity(p.row,p.col)->GetAllegiance()));
}

inline void MapData:: VerifyZoI (Scr::Position p)
{
	using namespace Scr;
	ForEachEntityInZoI(p,bind(&MapData::AcquireZoI,this,_1));

}
void MapData:: InsertEntity
(int y, int x, std::tr1::shared_ptr<Entity> e,PlayerAssets * a)
	throw(Entity::IncorrectPosition)
{
	using namespace Scr;
	Position p(y,x);
	e->SetMap(this);
	e->SetPosition(p);
	e->SetAllegiance(a);
	Fields[y][x].InsertEntity(e);
	if (e->GetAllegiance())
	{
		a->InsertEntity(e);
		if (e->GetMorale())
			AcquireZoI(p);
	}
}

void MapData::RemoveEntity(int y, int x)
{
	using namespace Scr;
	const SPE pE1 = GetEntity(y,x);
	Fields[y][x].RemoveEntity();
	if (PlayerAssets * pPA1 = pE1->GetAllegiance())
	{
		pPA1->RemoveEntity(pE1);
		VerifyZoI(Position(y,x));
	}
}
// file-static space (that is not avail. outside of
// this file even if declared somewhere)
namespace {
typedef vector<Scr::Position> PositionSequence;
//! Strict weak ordering of Scr::Positions by their 
//! position (for unique)
struct PosComp
{
	bool operator() (const Scr::Position&a,const Scr::Position&b)
		{
			return (a.col==b.col)?(a.row<b.row):(a.col<b.col);
		}
};
//! implements sort-unique-remove idiom
//! \param PS1 Reference to ector to transform
void UniquePositionSequence ( PositionSequence & PS1 )
{
	using namespace Scr;
	sort (PS1.begin(), PS1.end(),PosComp());
	PS1.erase ( unique (PS1.begin(), PS1.end()), PS1.end());
	// if want to replace vector w/ list, PS1.sort();PS1.unique() will do the job 
}
}
void MapData::MoveEntity(PathFinder::Path::iterator from,
						 PathFinder::Path::iterator to)
{
	using namespace Scr;
	const SPE pE1 = GetEntity(from->row,from->col);
	Fields[from->row][from->col].RemoveEntity();
	Fields[to->row][to->col].InsertEntity(pE1);
	if (pE1->GetMorale())
	{
	
	// PS1 = list of fields, that will be out of our range after
	// movement
	PositionSequence PS1;
	do
		for_each_position(*from,GetSize(),zoi_size,
						  bind(&PositionSequence::push_back,&PS1,_1));
	while (to != from++);
	UniquePositionSequence(PS1);

	// For each position in sequence verify its allegiance
  	for_each(PS1.begin(), PS1.end(),
  			 bind(&MapData::RecalculateZoI,this,bind(&Position::row,_1),
  				  bind(&Position::col,_1),pE1->GetAllegiance()));
	}
}


void MapData::SwitchTurn()
{
	if (active!=PL1.end())
	{
		(*active)->TurnEnd();
		active++;
	}
	if (active==PL1.end())
	{
		active=PL1.begin();
		turn++;
	}
	if (active!=PL1.end())
		(*active)->TurnBegin();
}

const std::string MapData::GetPlayerName() const
{
	return (*active)->GetName();
}
	
