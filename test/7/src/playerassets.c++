#include "playerassets.h++"
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include "entity.h++"
using namespace std;
using namespace boost::lambda;

using namespace std;
const std::string PlayerAssets::GetStatus() const
{
	return name+" turn";
}

void PlayerAssets::InsertEntity(std::tr1::shared_ptr<Entity> e)
{
	EL1.push_back(e);
	balance-=e->GetCostPT();
}
void PlayerAssets::RemoveEntity(std::tr1::shared_ptr<Entity> e)
{
	EL1.remove(e);
	balance+=e->GetCostPT();
}

void  PlayerAssets::TurnBegin()
{
	oil+=balance;
	for_each(EL1.begin(),EL1.end(),
			 bind(&Entity::TurnBegin,bind(&std::tr1::shared_ptr<Entity>::get,_1)));
}

void  PlayerAssets::TurnEnd()
{
	for_each(EL1.begin(),EL1.end(),
			 bind(&Entity::TurnEnd,bind(&std::tr1::shared_ptr<Entity>::get,_1)));
}
