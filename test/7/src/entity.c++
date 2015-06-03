


#include <boost/thread/mutex.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include "entity.h++"
#include "mapdata.h++"
using namespace boost::lambda;
using namespace std;
#define KMA(Name, Function, Key)							\
	AddActionAssignKey(										\
		Name,												\
		KeyMap::Action(										\
		bind(&Entity::Function,								\
			 var(pE1),										\
			 *var(pP1) )),Key)								\
;

Entity::KMSE::KMSE()
{
	// initialize keymap
	using namespace Scr;
 	KMA("Planned movement",Move,'m');
 	KMA("Fire",Attack,'f');
	KMA("Construct",ProduceDefault,'c');
 	KMA("Default action",DefaultAction,Key::Enter);
}

bool Entity::KMSE::ExecuteFor(Entity * pE,
							  const Scr::Position & P,Scr::Uint key)
{
	static boost::mutex M;
	boost::mutex::scoped_lock Lock(M);
	pP1=&P;
	pE1=pE;
	return Execute(key);
}

const std::string &Entity::GetHPDesc() const
{

	
	switch (hp)
	{
	case Nothing:
		static const string S1("in perfect condition");
		return S1;
	case LightlyDamaged:
		static const string S2("lightly damaged");
		return S2;
	case HeavilyDamaged:
		static const string S3("heavily damaged");
		return S3;
	case Destroyed:
	default:
		static const string S4("destroyed");
		return S4;
	}
}


HP Entity::AcceptAttack(int damage)
{
	if (damage<armor or hp==Destroyed)
		return hp;
	else if (damage<armor*2 and hp<LightlyDamaged)
		return hp=LightlyDamaged;
	else if (damage<armor*4 and hp<HeavilyDamaged)
		return hp=HeavilyDamaged;
	else
		return hp=Destroyed;
}

void Entity::Scrap()
{
	// remove it from all lists, where it is;
}

class DefaultObstacleDetector : public std::unary_function<Scr::Position,bool>
{
private:
	int sizelimit;
	MapData * pMD1;
	PlayerAssets * allegiance;
public:
	DefaultObstacleDetector(int sl,
							MapData * pMD,
							PlayerAssets * a)
		: sizelimit(sl),
		  pMD1(pMD),
		  allegiance(a){}
	bool operator() (Scr::Position p)
		{
			SPE pE1(pMD1->GetEntity(p.row, p.col));
			return 
				(not pE1)
				or
				(
					(pE1->GetSize() <= sizelimit)
					and
					(
						(not pE1->GetAllegiance())
						or pE1->GetAllegiance()==allegiance
					)
				)
					;
		}	
};




void CanMove::ProcessMovement()
{
	std::tr1::shared_ptr<Entity> pE1=		
		pMD1->GetEntity(position.row,position.col);
	Scr::Position np(position+target);
	//PathFinder PF1(pMD1,position,np,GetObstacleDetector());
	PathFinder & PF1 = ComputePath(np);
	PathFinder::Path::iterator i = PF1.GetPath().begin();
	if (PF1.GetPath().empty())
		throw ActionNotSupported("No way!");
	while ( i!=PF1.GetPath().end() and ap>=MovementCost(*i) )
	{
		ap-=MovementCost(*i);
		++i;
	}
	if (i==PF1.GetPath().end())
		--i,ap+=MovementCost(*i);
	while ((pMD1->GetEntity(i->row,i->col) and i!=PF1.GetPath().begin())
		or (not GetMorale()
			and GetAllegiance()!=pMD1->GetZoI(i->row,i->col)))
	{
		ap+=MovementCost(*i);
		--i;
	}

	if ( i != PF1.GetPath().begin() )
	{
		pMD1->MoveEntity( PF1.GetPath().begin(),i);
		position=*i;
		target=Scr::Vector((int)np.row-(int)position.row,
						   (int)np.col-(int)position.col);
	}	
}
Scr::Uint CanAttack::ChanceToHit(const Entity * target)
{

	Scr::Uint Dist=Distance(GetPosition(),target->GetPosition());
	return max<Scr::Sint>
		(0,min<Scr::Sint>
		 (100,  -2*Dist+skill+target->GetSize()));
}
PathFinder & CanMove::ComputePath(const Scr::Position & f)
{
	static const int sizelimit = 40;
	if (! (pPF1.get()))
	{
		pPF1.reset(
			new PathFinder
			(pMD1,position,f,
			 DefaultObstacleDetector(max(sizelimit,size),pMD1,allegiance),
			 bind(&CanMove::MovementCost,this,_1)
				)
			);
	}
	else
		pPF1->ConditionalReset(position,f);
	return *pPF1.get();
}


int CanMove::MovementCost(const Scr::Position & p)const
{
	return 1;
}
namespace
{
	template <typename T>
	T sqr(T a) {return a*a;}
}
void CanMove::Move(const Scr::Position & p)
{
		if (pMD1->GetEntity(p.row,p.col)
			and pMD1->GetEntity(p.row,p.col).get()!=this)
			throw ActionNotSupported("Can't move to ocupied field");
		target=Scr::Vector((int)p.row-(int)position.row,
						   (int)p.col-(int)position.col);
		ProcessMovement();
		O1=OrderNone;
}

void CanMove::DefaultAction(const Scr::Position & p)
{
	if (not pMD1->GetEntity(p.row,p.col))
		Move(p);		
}


void CanAttack::Attack(const Scr::Position & p)
{
	if (!GetAP())
		throw ActionNotSupported("Not enough AP!");
	if (p==position)
		throw ActionNotSupported("Can't commit suicide!");
	if (Distance(p,position) > GetRange())
		throw ActionNotSupported("Target Out Of Range");
	Entity * target = pMD1->GetEntity(p.row,p.col).get();
	if (!target)
		throw ActionNotSupported("No target?");
	if (target->GetAllegiance()==allegiance)
		throw ActionNotSupported("Friendly fire?");
	if (static_cast<Scr::Uint>(rand()%100)
		<ChanceToHit(target))
		pMD1->Attack(p.row,p.col,1+(rand()%damage));
	ap=0;
}

void CanAttack::DefaultAction(const Scr::Position & p)
{
	if (not (p==position))
		Attack(p);
}

void Warrior::DefaultAction(const Scr::Position & p)
{
	if (not pMD1->GetEntity(p.row,p.col))
	{
		Move(p);
	}
	else if (not (p==position))
		Attack(p);
}

void CanMove::TurnEnd()
{
	if (target.cols or target.rows)
	{
		try
		{
			ProcessMovement();
		}
		catch (...){;}
	}
	if (pPF1.get())
		pPF1.reset();
}


#include "entity_types.h++"
std::string EntityTraits(const Entity & e)
{
	std::string result;
	if (!e.GetMorale());// no morale
	else if (e.GetMorale())
		result.append("M+ "); // high morale
	else 
		result.append("M "); // moderate morale
	if (dynamic_cast<const Wheeled *> (&e))
		result.append("W ");
	if (dynamic_cast<const Aircraft *> (&e))
		result.append("Air ");
	if (dynamic_cast<const AntiAir *> (&e))
		result.append("AA ");
	if (dynamic_cast<const CanProduceBase *> (&e))
		result.append("P ");
	if (result.empty())
		return string("N/A");
	else
	{
		result.resize(result.size()-1);
		return result;
	}
}
