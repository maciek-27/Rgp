#ifndef PLAYER_ASSETS_HPP_DEF__
#define PLAYER_ASSETS_HPP_DEF__
#include <list>
#include "addentitydialog.h++"
#include "mapdata.h++"
#include <boost/shared_ptr.hpp>
#include <rexio/screen.h++>
enum PlayerType {Local, Remote,  AI};
class PlayerAssets
{
protected:
	// only two resources in game
	MapData * pMD1;
	int oil;
	int balance; // income
	std::string name;
	typedef std::list<std::tr1::shared_ptr<Entity> > EntitiesList;
	EntitiesList EL1;
	Scr::Fg::Color color;
public:
	PlayerAssets(MapData * md, std::string n, int o)
		: pMD1(md)
		, oil(o)
		, balance(0)
		, name(n)
		{ ; }
	void SetColor(Scr::Fg::Color c) {this->color=c;}
	Scr::Fg::Color  GetColor() {return color;}
	const std::string GetName() const {return name;}
	const std::string GetStatus() const; //description in statusbar

	//! change income (if want to decreese it, pass negative value
	void AddIncome(int i) {balance +=i;}
	int GetOil() {return oil;}
	void AddOil(int o) {oil+=o;}
	int GetIncome() {return balance;}
	void InsertEntity(std::tr1::shared_ptr<Entity> e);
	void RemoveEntity(std::tr1::shared_ptr<Entity> e);
	void TurnBegin();
	void TurnEnd();
	
	virtual PlayerType GetPlayerType() = 0;
	
};

class LocalPlayer: public PlayerAssets
{
	Scr::Tk::Window & w;//! window associated w/ this player
public:
	LocalPlayer(Scr::Tk::Window & _w,
				MapData * md, std::string n, int o):
		PlayerAssets(md,n,o),w(_w){}

	PlayerType GetPlayerType() {return Local;}

 	template<typename List>
 	void ProductionDialog(const Scr::Position & p,Entity * _whoproduce)
 		{
 			new AddEntityDialog<List>(w,w.GetHeight()/2,p,pMD1,_whoproduce);
 		}
};
#endif
