#include "canproduce.h++"
#include "mapdata.h++"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/if.hpp>

using namespace std;
using namespace boost::lambda;
const int CanProduceBase::ProductionCost = 3;
void CanProduceBase::TryProduce(const Scr::Position & p)
{
	if (GetAP() <ProductionCost)
		throw ActionNotSupported(
			"Not enough AP!");
	else if (Distance(p,position)>1)
		throw ActionNotSupported(
			"Please select adjacent field");
	else if (pMD1->GetZoI(p.row,p.col)!=allegiance)
		throw ActionNotSupported(
			"New entities may be placed only on tiles, we control.");
	else if (pMD1->GetEntity(p.row,p.col))
		throw ActionNotSupported(
			"New entities may be placed only on empty fields.");
}
bool CanProduceBase::CanProduceHere(const Scr::Position & p)
{
	if (GetAP() !=GetAPMax())
		return false;
	else if (pMD1->GetZoI(p.row,p.col)!=allegiance)
		return false;
	else if (pMD1->GetEntity(p.row,p.col))
		return false;
	return true;
}
void CanProduceBase::DefaultAction(const Scr::Position & p)
{
	if (p==GetPosition())
	{
 		bool done = false;
 		for_each_position(
			p,pMD1->GetSize(),1,
			if_then(bind(&CanProduceBase::CanProduceHere,this,_1)
					and not var(done),
					(
						bind(&Entity::ProduceDefault,this,_1),
						var(done)=true
						)
				));
		
	}
}
