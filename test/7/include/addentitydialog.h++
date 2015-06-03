#ifndef ADD_ENTITY_DIALOG_HPP_DEF__
#define ADD_ENTITY_DIALOG_HPP_DEF__
#include<string>
#include<rexio/tk/button.h++>
#include<rexio/tk/rootwindow.h++>
#include<rexio/tk/window.h++>
#include<boost/array.hpp>
#include "boost/mpl/for_each.hpp"
#include <boost/type_traits/is_base_of.hpp>
#include "mapdata.h++"
#include "alert.h++"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
class MapData;
#include "entity.h++"
class CanAttack;
class CanMove;
class Warrior;
//! Magic dialog for adding new entities: typelist independent base
class AddEntityDialogBase: public Scr::Tk::Window
{
	template<typename T>
	friend class Selector;

private:
	int selection;

public:
	//! number of columns
	static const size_t num_columns=9;
	//! default display style for selector
	static const Scr::DisplayStyle SelectorDisplayStyle1;
	//! default display style for focused selector
	static const Scr::DisplayStyle SelectorDisplayStyle2;

	//! selector button (used to select entity to produce; one button
	//! for one row of table, one row for one unit)
	class SelectorBase:public Scr::Tk::Button
	{
		friend class AddEntityDialogBase;
	protected:
		//! member  array: contents of row
		boost::array<std::string,
					 AddEntityDialogBase::num_columns> columns;
	public:
		//! constructor - basic setup. All  parameters have default
		//! values and meaning the same as for Scr::Tk::Button
		explicit
		SelectorBase(const Scr::DisplayStyle& _style = SelectorDisplayStyle1,
					 const Scr::DisplayStyle& _activeStyle
					 = SelectorDisplayStyle2);

		//! display button 
		void OnRedraw(Scr::Screen &screen)throw();
	  ~SelectorBase()throw() {}
	};
protected:
	//!widths of columns
	boost::array<int,num_columns> columns;
	//! header (usually empty)
	std::string header;
	//! additional setup - may be used by constructors of derived classes
	void AfterConstructorSetup()throw();
public:
	//! places dialog in window w.
	AddEntityDialogBase( Scr::Tk::Window & w,
						 Scr::Uint  hmax);

	//! default onredraw action
	void OnRedraw(Scr::Screen &screen)throw();

	//! On key pressed actions: arrows to select option, enter to
	//! accept it. overrides default window behaviour
	void OnKeyDown(Scr::Key key)throw();
private:
	//! Unregister and destroy self
	void Suicide()throw();
public:
	//! process resize event
	virtual void OnResize()throw();
	virtual void OnUnFocus(FocusPolicy focustype)throw();
	virtual void OnFocus(FocusPolicy focustype)throw();
	~AddEntityDialogBase()throw();
};

/*!
 MPL based implementation of AddEntityDialog:
 constructor fills table with properties of all types listed within
 EntitiesList

 EntitiesList is boost::mpl meta-container for all types, that will be availble
*/
template <typename EntitiesList>
class AddEntityDialog: AddEntityDialogBase
{
private:
	//! where we want to add that entity
	Scr::Position p1;
	//! which map does position above refer to
	MapData * pMD1;
	Entity * whoproduce;
public:

	//! object, that represents specific class from EntitiesList
	template<typename T>
	class Selector:public SelectorBase
	{
	public:
		static const boost::true_type boost_true;
		//! print value in column, where static condition (refer to
		//! doc. for constructor) is fulfilled
		template <typename F>
		void PrintValue(F valuesrc, int col, boost::true_type = boost_true)
			{
				std::stringstream ss;
				ss << valuesrc();
				columns[col].assign(ss.str());					
			}
		//! print value in column, where static condition (refer to
		//! doc. for constructor) is not fulfilled
		template <typename F>
		void PrintValue(F valuesrc, int col, boost::false_type)
			{
				columns[col].assign("N/A");					
			}
	public:
		//! Sets values of all columns. uses PrintValue function's
		//! tird parameter to call methods, which aren't present for
		//! all entity typed (using boost::is_base_of static condition)
		Selector(const Scr::DisplayStyle& _style = SelectorDisplayStyle1,
				 const Scr::DisplayStyle& _activeStyle
				 = SelectorDisplayStyle2)
			:SelectorBase( _style,	_activeStyle)
			{
				namespace bl=boost::lambda;
				// must refer to bl::bind explicitly fo prevent name
				// conflict against boost::mpl::bind
				T  E1;			
				boost::is_base_of<CanAttack,T> can_attack;
				boost::is_base_of<CanMove,T> can_move;
				columns[0].assign(E1.GetName());
				PrintValue(bl::bind(&Entity::GetCost,&E1),1);
				{
					std::stringstream ss;
					if (E1.GetCost()>=0)
						ss << E1.GetCost();
					else
						ss << "0 (profit: "<< -E1.GetCost()<<')';
					columns[2].assign(ss.str());					
				}
				PrintValue(bl::bind(&Entity::GetCostPT,&E1),2);
				columns[3].push_back(E1.GetSymbol());
				PrintValue(bl::bind(&Entity::GetArmor,&E1),4);
				PrintValue(bl::bind(&Warrior::GetDamage,&E1),5,can_attack);
				PrintValue(bl::bind(&Warrior::GetRange,&E1),6,can_attack);
				PrintValue(bl::bind(&Warrior::GetAPMax,&E1),7,can_move);
				columns[8].assign(EntityTraits(E1));
			}
		//! inserts item
		virtual void OnAction()throw()
			{
				T *  pE1 = new T;
				AddEntityDialog&AF1(static_cast<AddEntityDialog&>(GetParent()));
 				try
 				{
					AF1.whoproduce->Produce(AF1.p1, SPE(pE1));
				}
 				catch (Scr::Exception & e)
 				{
  					Alert(GetParent().GetRootWindow(),
  						  e.what(),
  						  AddEntityDialogBase::SelectorDisplayStyle1);
 					return;
 				}
				RedrawRequest();
			}
	};
	//! mpl::foreach functor is only purpose for this class
	class DescribeEntity
	{
		AddEntityDialog&AF1;
	public:
		DescribeEntity(AddEntityDialog&AF)
			:AF1(AF){}
		template <typename T>
		void operator()(T e)
			{
				AF1.AddWidget(*(new Selector<T>));
			}
	};


	//!\param w: root window
	//!\param hmax: maximum height of window
	//!\param p: position on map, where facility will be constructed
	//!\param pMD: map, where position p will be verified
	//!
	AddEntityDialog( Scr::Tk::Window & w,
					 Scr::Uint  hmax,
					 Scr::Position p,
					 MapData * pMD,
					 Entity * _whoproduce)
		:AddEntityDialogBase(w,hmax),p1(p),pMD1(pMD),whoproduce(_whoproduce)
		{
			boost::mpl::for_each<EntitiesList>(DescribeEntity(*this));
			AfterConstructorSetup();
		}
  ~AddEntityDialog()throw() {}
};

#endif
