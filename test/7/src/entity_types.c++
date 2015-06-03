#include "entity_types.h++"
#include <algorithm>
#include "entity.h++"
#include "entity_types.h++"

#include <sstream>
#include "mapdata.h++"
#include "playerassets.h++"
#include <rexio/tk/button.h++>
#include <boost/typeof/typeof.hpp>
#include "boost/mpl/for_each.hpp"
#include "boost/mpl/list.hpp"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>


using namespace boost::lambda;
using namespace Scr::Tk;
using namespace Scr;
using Scr::Control::Clear;
using std::string;
using std::cout;
namespace mp=boost::mpl;


//// INFANTRY
Infantry::Infantry() 
{
	armor=1;
	size=1;
	apmax=4;
	ap=0;
	cost=3;
	costpt=1;
	range=5;
	damage=3;
	skill=50;
}
const std::string & Infantry::GetName() const  // name of entity
{
	static const string name("Infantry");
	return name;
}
const char Infantry::GetSymbol() const  // map symbol
{
	return '.';
}
//// ENGINEER
Engineer::Engineer() 
{
	cost=6;
}
const std::string & Engineer::GetName() const  // name of entity
{
	static const string name("Engineer");
	return name;
}
const char Engineer::GetSymbol() const  // map symbol
{
	return 'e';
}

//// WORKER
Worker::Worker() 
{
	armor=0;
	size=1;
	apmax=3;
	ap=0;
	cost=3;
	costpt=1;
}
const std::string & Worker::GetName() const  // name of entity
{
	static const string name("Worker");
	return name;
}
const char Worker::GetSymbol() const  // map symbol
{
	return 'w';
}

///// WHEELED
int Wheeled::MovementCost(const Scr::Position & p)const
{
	if (pMD1->GetTerrainType(p.row,p.col) == MapData::Road)
		return 1;
	else
		return GetAPMax();
}


//// AIRCRAFT
Aircraft::Aircraft() {}
PathFinder & Aircraft::ComputePath(const Scr::Position & f)
{
	if (! (pPF1.get()))
	{
		pPF1.reset(
			new PathFinder
			(pMD1,position,f,
			 constant(1),
			 bind(&CanMove::MovementCost,this,_1)
				)
			);
	}
	else
		pPF1->ConditionalReset(position,f);
	return *pPF1.get();
}


//// BARRACK
Barrack::Barrack() 
{
	armor=6;
	size=50;
	apmax=3;
	ap=0;
	cost=30;
	costpt=5;
}
const std::string & Barrack::GetName() const  // name of entity
{
	static const string name("Barrack");
	return name;
}
const char Barrack::GetSymbol() const  // map symbol
{
	return 'B';
}

//// FACTORY
Factory::	Factory() 
{
	armor=5;
	size=70;
	apmax=3;
	ap=0;
	cost=50;
	costpt=25;
}
const std::string & Factory::GetName() const  // name of entity
{
	static const string name("Factory");
	return name;
}
const char Factory::GetSymbol() const  // map symbol
{
	return 'F';
}

//// SENTRY TOWER
Tower::Tower() 
{
	armor=7;
	size=20;
	apmax=1;
	ap=0;
	cost=30;
	costpt=1;
	range=6;
	damage=7;
	skill=60;
}
const std::string & Tower::GetName() const  // name of entity
{
	static const string name("Sentry Tower");
	return name;
}
const char Tower::GetSymbol() const  // map symbol
{
	return 'T';
}

//// HEADQUATERS
HQ::HQ() 
{
	armor=0;
	size=50;
	apmax=3;
	ap=0;
	cost=0;
	costpt=0;
}
const std::string & HQ::GetName() const  // name of entity
{
	static const string name("Headquaters");
	return name;
}
const char HQ::GetSymbol() const  // map symbol
{
	return 'H';
}
//// OIL WELL
OilWell::OilWell() 
{
	armor=0;
	size=50;
	apmax=0;
	ap=0;
	cost=20;
	costpt=-10;
}
const std::string & OilWell::GetName() const  // name of entity
{
	static const string name("Oil well");
	return name;
}
const char OilWell::GetSymbol() const  // map symbol
{
	return 'W';
}


void OilWell::SetPosition(const Scr::Position & p)
	throw(IncorrectPosition)
{
 	if (pMD1->GetResources(p.row,p.col)=='o')
 		position=p;
 	else
 		throw IncorrectPosition("Oil wells may be placed only on oilfields!");
}
// AddItem
