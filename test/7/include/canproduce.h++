#ifndef CAN_PRODUCE_HPP_DEF__
#define CAN_PRODUCE_HPP_DEF__
#include "entity.h++"
#include "playerassets.h++"

class CanProduceBase : virtual public Entity
{
protected:
	static const int ProductionCost;
	void TryProduce(const Scr::Position & p);
public:
	bool CanProduceHere(const Scr::Position & p);
	void DefaultAction(const Scr::Position & p);
};

//CanProduce:: can construct facilities (worker) or train warriors
//(barracks, factory)
template<typename List>
class CanProduce : public CanProduceBase
{
protected:
public:
	CanProduce(){}
	//! Display dialog
	void ProduceDefault(const Scr::Position & p)
		{
			TryProduce(p);
			LocalPlayer * lp = dynamic_cast<LocalPlayer*>(allegiance);
			if (lp)
			{
				lp->ProductionDialog<List>(p,this);
			}
			else		
				throw ActionNotSupported(
					"void Produce(const Scr::Position & p); is only"
					" for local human players, not for AI.");
		}

	//!insert entity whatproduce into position p
	void Produce(const Scr::Position & p, SPE whatproduce)
		
		{
			TryProduce(p);
			
			if (whatproduce->GetCost()
				>
				pMD1->GetActive()->GetOil())
			{
				throw ActionNotSupported(
					"Sorry! you can't afford it");
			}
			try
			{
				pMD1->InsertEntity(p.row,p.col,whatproduce,
								   pMD1->GetZoI(p.row,p.col));
			}
			catch (Entity::IncorrectPosition & e)
			{
				throw ActionNotSupported(e.what());
			}
			pMD1->GetActive()->AddOil(-whatproduce->GetCost());
			ap=ProductionCost;
		}
};
#endif
