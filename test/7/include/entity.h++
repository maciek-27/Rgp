#ifndef ENTITY_HPP_DEF__
#define ENTITY_HPP_DEF__
#include <rexio/commons.h++>
#include "entity_decl.h++"
#include "keymap.h++"
#include <boost/function.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <boost/logic/tribool.hpp>
class PlayerAssets;
class MapData;
class PathFinder;
using boost::tribool;

//! Base entity class.
class Entity
{
public:
	enum Order {OrderNone, OrderMove, OrderAttack};
	__DE(ActionNotSupported,Scr::Exception);
	__DE(IncorrectPosition,Scr::Exception);
protected:
	Order O1;

	HP hp; //! Hit Points: 5= OK; 4=lightly damaged; 2= heavily
			//! damaged;
	int armor; //! specific to unit type. Man = 0, car=4, tank=10,
			   //! building=8-40
	int size; //! how it is difficult to hit specific entity
	//! 1-3 - aircraft, 1-man, 10 - vehicle, 40 - building
	int apmax; //! how much actions can entity perform each turn
	int ap; //! how much actions are left?
	PlayerAssets * allegiance ;
	int cost; //! construction cost
	int costpt; //! maintainance cost per turn
	Scr::Position position;
	MapData * pMD1;
public:
	//! KeyMap of Selected Entity (overrides default
	//! keymap if any entity is selected)
	//! (protected inheritance )
	class KMSE: protected KeyMap
	{
	private:
		Entity * pE1;
		const Scr::Position * pP1;
	protected:
		//! Construcred using boost::...::singleton
		KMSE();
	public:
		//! publish only selected functions
		using KeyMap::PrintHelp;
		// ( hide others)

		//! No default execute
		bool ExecuteFor(
			Entity * pE,
			const Scr::Position & P,
			Scr::Uint key);		
	};
	
public:

	//! Returns KMSE derrived object
	static KMSE & GetKeyMap()
		{
			return boost::detail::thread::singleton<KMSE>::instance();
		}


	//! Default constructor
	Entity (): O1(OrderNone),
			   hp(Nothing),
			   allegiance(0),
			   position(0,0){}
	//! Dtor
	virtual ~Entity() {}

	//! Return name of entity
	virtual const std::string & GetName() const = 0; // name of entity
	//! return map symbol
	virtual const char GetSymbol() const = 0; // map symbol
	//! textual HP level 
	virtual const std::string & GetHPDesc() const; // damage level
	//! Return false if no damage, true otherwise
	bool IsHit()const {return hp!=Nothing;}
	//! if entity is attacked, calculate damage
	//! \param damage damage amount
	//! \return damage level after attack
	virtual HP AcceptAttack(int damage) ;
	//! set allegiance
	void SetAllegiance(PlayerAssets * a) {allegiance=a;}
	//! return entity allegiance
	PlayerAssets * GetAllegiance()const {return allegiance;}
	//! return HP Level
	HP GetHP() const {return hp;}
	/*!
	  Armor quality
	 */
	int GetArmor() const {return armor;}
	/*!
	  Relative size
	 */
	int GetSize() const {return size;}
	/*!
	  Maximum AP
	 */
	int GetAPMax() const {return apmax;}
	/*!
	  Current AP level
	 */
	int GetAP() const {return ap;}
	//! construction cost
	int GetCost() const { return cost;}
	//! get maintainance cost
	int GetCostPT() const {return costpt;}


	
	//! morale  - structure is less likely to surrender
	virtual tribool GetMorale() const {return false; }
	//! set internal position record
	virtual void SetPosition(const Scr::Position & p)
		throw(IncorrectPosition)
		{position=p;}
	//! get internal position report
	Scr::Position GetPosition()const {return position;}

	//! get internal mapdata pointer
	MapData * GetMap() {return pMD1;}
	//! set internal mapdata pointer
	void SetMap(MapData * m) {pMD1=m;}
	
	// Actions table (most of theese actions are supported only by
	// small fraction of units (i.e. only cars and helicopters can
	// transport warriors), but all of them are executed as virtual
	// functions of Entity, so they have to be defined here.

#define DefAction(prot) virtual void  prot(const Scr::Position & p)\
		{throw ActionNotSupported(GetName() + " can't " + #prot);}
	//donothing
	virtual void  DefaultAction(const Scr::Position & p){}
	DefAction(Move)
	DefAction(Attack)
	DefAction(ProduceDefault) // display dialog to select what
							  // produce, and then eventually do it
	virtual void  Produce(const Scr::Position & p, SPE whatproduce)	\
		{throw ActionNotSupported(GetName() + " can't construct any entities");}
	void Scrap(); // remove


	//! return state
	Order GetOrder() {return O1;}
#undef DefAction
//	virtual PathFinder::ObstacleDetector GetObstacleDetector();
	//! update record on turn begin
	virtual void TurnBegin(){ap=apmax;}
	//! update record on turn end
	virtual void TurnEnd(){}
}; // end of class Entity

//! classe for entities, that can move
class CanMove : virtual public Entity
{
private:
protected:
	//! internal PathFinder pointer
	std::auto_ptr<PathFinder> pPF1;
	//! movement order (relative to current position)
	Scr::Vector target;
	//! Process movement: change position, update map, update AP
	//! etc. If not enough AP to fulfill full movement order, save
	//! remaining in target field and proceed in next turn (if order
	//! is maintained)
	void ProcessMovement();
public:
	//! base constructor: set target(0,0), that is no movement order
	CanMove(): target(0,0){}
	//! compute path (for both: ProcessMovement and UI)
	virtual PathFinder & ComputePath(const Scr::Position & f);
	//! calculate movement cost for specific field
	virtual int MovementCost(const Scr::Position & p) const;
	//! Movement order processing override.
	void Move(const Scr::Position & p);
	//! on turn end process movement with remaining AP if apropriate
	void TurnEnd();
	//! default action: move
	void DefaultAction(const Scr::Position & p);
};

//! Implementation of attack function and other battle-related features
class CanAttack : virtual public Entity
{
protected:
	Scr::Uint range; //! how far can this warrior aim? 1= only adjacent
			   //! field...
	
	Scr::Uint damage; //! min amount of damage inflicited by attack
	//! 1-pistol, 3- rifle, 30 - tank guns and mortars, 50 - howitzers
	//! and aerial bombs
	Scr::Uint skill; //! how good at shooting is specific warrior? (how small
			   //! units can he target)
	//! 1 - howitzers, bombers; 4- tanks, airfighters; 10 infantrymen;
	//! 50 - sniper
public:
	CanAttack(){;}
	//! TRUE!! Thats soldier, so its not a surprise :)
	tribool GetMorale() const {return true; }
	//! Yes! attack enemy in position p (or throw apropriate exception 
	void Attack(const Scr::Position & p);
	//! Attack range (can't reach too distant entities;
	Scr::Uint GetRange() const {return range;}
	//! Get maximum damage, this unit can inflict
	Scr::Uint GetDamage() const {return damage;}
	//! Get skill (that is general accuracy bonus)
	Scr::Uint GetSkill() const {return skill;}
	//! Total chance to hit computation. Returns integer between 0 and
	//! 100 depicting % of chance to hit. It takes into account
	//! Skill, target size and distance
	virtual Scr::Uint ChanceToHit(const Entity * target);
	void DefaultAction(const Scr::Position & p);
};//CanAttack
class Warrior : public CanMove, virtual public CanAttack
{
public:
	// commands
	//! default (and only! ) constructor
	Warrior (){;}
	// Move or attack
	void DefaultAction(const Scr::Position & p);
};// Warrior
//! Print string describing entity properties
std::string EntityTraits(const Entity & e);
#endif
