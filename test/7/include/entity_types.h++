#ifndef ENTITY_TYPES_HPP_DEF__
#define ENTITY_TYPES_HPP_DEF__
#include "entity.h++"
#include "canproduce.h++"
#include "boost/mpl/list.hpp"

class Barrack;
class Factory;
class Tower;
class OilWell;
class Airfield;
typedef boost::mpl::list<
	Airfield,Barrack,Factory,Tower,OilWell
	> FacilitiesList;

#define DefEntity(cname,base,ctor,name,symbol)	\
	class cname: base							\
	{											\
	public:										\
		cname()									\
		{										\
			ctor								\
		}										\
		const std::string & GetName() const		\
		{										\
		static const std::string n(#name);		\
		return n;								\
		}										\
	const char GetSymbol() const				\
		{										\
		return symbol;							\
		}										\
	}
//! Most basic soldier type
class Infantry: public Warrior
{
public:
	Infantry();
	const std::string & GetName() const;
	const char GetSymbol() const;
};


//! in future versions of game will have additional capabilities, such
//! as placing/removing mines, destroying buildings, placing
//! explosives etc. (now equals infantry + is more expansive, so does
//! not make big sense :(
class Engineer: public Infantry
{
public:
	Engineer();
	const std::string & GetName() const;
	const char GetSymbol() const;
};

//! basic unit: can construct barracks and factories
class Worker: public CanMove, public CanProduce<FacilitiesList>
{
public:
	Worker();
	const std::string & GetName() const;
	const char GetSymbol() const;
	void DefaultAction(const Scr::Position&p)
		{ CanMove::DefaultAction(p);}
};

//! all aircraft are militaty ones.
class Aircraft: public Warrior
{
public:	
	Aircraft();
	tribool GetMorale() const {return boost::logic::indeterminate; }
	PathFinder & ComputePath(const Scr::Position & f);
};
//! AA weapons 
class AntiAir: virtual public CanAttack
{
public:
	Scr::Uint ChanceToHit(const Entity * target)
		{
			Scr::Uint res(CanAttack::ChanceToHit(target));
			if (dynamic_cast<const Aircraft*>(target))
			{
				res+=60;
				if (res>100) res=100;
			}
			return res;
		}
	
};

//! Wheeled vehicles
class Wheeled : public Warrior
{
public:
	int MovementCost(const Scr::Position & p) const;
	
};

class AirFighter: public Aircraft, public AntiAir {};

//! interceptor fighter: primary anti air weapon
DefEntity(Interceptor,
		  public AirFighter, 
		  armor=1;size=1;apmax=60;ap=0;cost=30;
		  costpt=15;range=4;damage=4;skill=10;,
		  Interceptor, 'i');

//! Assult fighter fights ground units: it is designed to support your
//! ground forces in advancing through enemy lines
DefEntity(AFighter,
		  public Aircraft, 
		  armor=4;size=2;apmax=40;ap=0;cost=30;
		  costpt=15;range=7;damage=13;skill=25;,
		  Assult fighter, 'f');
//! Strategic bomber delivers impressive payload of MOABs :) :)
DefEntity(SBomber,
		  public Aircraft, 
		  armor=2;size=2;apmax=30;ap=0;cost=30;
		  costpt=25;range=2;damage=100;skill=25;,
		  Strategic bomber, 'b');
//! Achtung! Panzer!
DefEntity(Tank,
		  public Warrior, 
		  armor=9;size=10;apmax=12;ap=0;cost=40;
		  costpt=15;range=12;damage=15;skill=25;,
		  Tank, 't');

//! Anti air gun
class AAG: public AntiAir, public Warrior
{											
public:										
	AAG()									
		{										
			
			armor=1;size=2;apmax=2;ap=0;cost=10;
			costpt=3;range=15;damage=11;skill=0;								
		}								
	const std::string & GetName() const		
		{											
			static const std::string n("Anti aircraft gun");		
			return n;								
		}											
	const char GetSymbol() const				
		{											
			return 'a';							
		}											
};
DefEntity(Cannon,
		  public Warrior, 
		  armor=1;size=2;apmax=2;ap=0;cost=10;
		  costpt=3;range=15;damage=20;skill=30;,
		  Cannon, 'c');

DefEntity(SPCannon,
		  public Wheeled, 
		  armor=5;size=6;apmax=13;ap=0;cost=20;
		  costpt=6;range=15;damage=20;skill=29;,
		  Self propelled cannon, 's');

DefEntity(MG,
		  public Warrior, 
		  armor=1;size=2;apmax=2;ap=0;cost=7;
		  costpt=2;range=9;damage=7;skill=50;,
		  Machine gun, 'm');

DefEntity(Jeep,
		  public Wheeled, 
		  armor=6;size=5;apmax=21;ap=0;cost=10;
		  costpt=2;range=9;damage=7;skill=40;,
		  Armored car, 'j');

//! each of theese units may be constructed in factory
typedef boost::mpl::list<
	Tank,MG,Cannon,SPCannon,Jeep,AAG,Interceptor,AFighter,SBomber
	> MachinesList;


//! for buildiings
class Facility: virtual public Entity{};

//! Headquaters
class HQ: public Facility, public CanProduce<
	boost::mpl::list<
	Worker
	> >
{
public:	
	HQ() ;
	const std::string & GetName() const;
	const char GetSymbol() const;
	tribool GetMorale() const {return true; }
};

//! Brings profit
class OilWell: public Facility
{
public:	
	OilWell() ;
	const std::string & GetName() const;
	const char GetSymbol() const;
	virtual void SetPosition(const Scr::Position & p)
		throw(IncorrectPosition);
};

//! to train soldiers
class Barrack: public Facility,public CanProduce<
	boost::mpl::list<
		Worker,Infantry,Engineer
	> >
{
public:	
	Barrack() ;
	const std::string & GetName() const;
	const char GetSymbol() const;
};

//! to produce machines
class Factory: public Facility, public CanProduce<MachinesList>
{
public:	
	Factory() ;
	const std::string & GetName() const;
	const char GetSymbol() const;
};
//! sentry tower is main defensive structure
class Tower: public Facility, public CanAttack
{
public:	
	Tower() ;
	const std::string & GetName() const;
	const char GetSymbol() const;
};
//! airfields are required for proper operation of aircraft
DefEntity(Airfield,
		  public Facility, 
		  armor=3;size=90;apmax=0;ap=0;cost=50;
		  costpt=30;,
		  Airfield, 'A');


#endif
