#include <rexio/keyboard.h++>
#include<rexio/tk/toolkit.h++>
#include<iostream>
#include "map.h++"
#include "alert.h++"
#include "statusbar.h++"
#include "playerassets.h++"
#include "hintbox.h++"
#include <vector>

#include <sys/wait.h>
#include<sstream>
#include<algorithm>
#include<iomanip>
#include<functional>
#include<ext/functional>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/if.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/lambda/exceptions.hpp>
#include "entity.h++"
#include "pathfinder.h++"
#include "entity_types.h++"
using namespace std;
using namespace Scr;
using namespace Scr::Tk;
using namespace Scr::Control;
using namespace boost::lambda;
#if defined _GNU_SOURCE
using __gnu_cxx::compose2;
using __gnu_cxx::compose1;
#endif
namespace
{
	// as comments unicode alternatives, that look way better,
	// but are not availble within most of popular fonts
	const wchar_t slash = '/';// 0x2571;
	const wchar_t backslash = '\\';// 0x2572;
	const wchar_t underline = '_';// 0x2581;
}
void Map::DrawHexGrid()

// Draw hex grid
{
	unsigned xpos = 0;
	unsigned ypos = 1;

	for ( int j = D1S.width-1 ; j>= 0 ; j-- )
		for ( int i = D1S.height-1 ; i>= 0 ; i-- )
			D1[i][j]='U'; // stands for UnInitialized
	// topmost line
	for ( int j = pMD1->GetWidth() ; j> 0 ; j-=2 )
	{
		D1[0][xpos++]=' ';
		D1[0][xpos++]=underline;
		D1[0][xpos++]=underline;
		if (j>1)
		{
			D1[0][xpos++]=' ';
			D1[0][xpos++]=' ';
			D1[0][xpos++]=' ';
		}
	}
	// finish all lines w/ 0
	for ( unsigned int i = 0 ; i!= D1S.height ; ++i )
		D1[i][D1S.width]=0;
	D1[0][D1S.width-1]=' ';
	D1[D1S.height-1][D1S.width-1]=' ';

	// right line
	if (not (pMD1->GetWidth()&1))
	{
		D1[ypos++][D1S.width-1]=' ';
		for ( unsigned int i = pMD1->GetHeight() ; i!= 0 ; --i )
		{
			D1[ypos++][D1S.width-1]=backslash;
			D1[ypos++][D1S.width-1]=slash;
			
		}
		ypos=1;
	}
	
	for ( unsigned int i = pMD1->GetHeight() ; i> 0 ; --i )
	{
		xpos=0;
		unsigned ypos1=ypos+1;
		for ( int j = pMD1->GetWidth() ; j> 0 ; j-=2 )
		{
			D1[ypos ][xpos]  =slash;
			D1[ypos1][xpos++]=backslash;
			D1[ypos ][xpos]  =				
				pMD1->GetTerrainType(pMD1->GetHeight()-i,
								   pMD1->GetWidth()-j);
			D1[ypos1][xpos++]=underline;
			D1[ypos ][xpos]  =
				pMD1->GetResources(pMD1->GetHeight()-i,
								   pMD1->GetWidth()-j);
				
			D1[ypos1][xpos++]=underline;
			D1[ypos1][xpos]  =slash;
			D1[ypos][xpos++]=backslash;
			if (j>1)
			{
				D1[ypos1][xpos]  = 
				pMD1->GetTerrainType(pMD1->GetHeight()-i,
									 pMD1->GetWidth()-j+1);
				D1[ypos ][xpos++]=underline;
				D1[ypos1][xpos]  =
					pMD1->GetResources(pMD1->GetHeight()-i,
									   pMD1->GetWidth()-j+1);
				D1[ypos ][xpos++]=underline;
			}
		}
		ypos+=2;
	}
	ypos=D1S.height-1;
	xpos=1;
	D1[ypos ][0]=' ';

	for (  int j = pMD1->GetWidth() ; j> 0 ; j-=2 )
	{
		D1[ypos][xpos++]  =' ';
		D1[ypos][xpos++]  =' ';
		if (j>1)
		{
			D1[ypos][xpos++]  =backslash;
			D1[ypos][xpos++]  =underline;
			D1[ypos][xpos++]  =underline;
			D1[ypos][xpos++]  =slash;
		}
	}
}// hex grid ready

//key map assignment
#define KMA(Name, Function, Key)										\
	KM1.AddActionAssignKey(Name, KeyMap::Action(bind(&Map::Function,this)),Key)	\
	;

#define KMA2(Name, Function, Key1, Key2)		\
	KMA(Name, Function, Key1);					\
	KM1.AssignKey(Key2,Name)
	
void Map::SetUpKeyMap()
{
	KMA2("Move cursor South",MoveS,       '2',Key::Down);
	KMA2("Move cursor North",MoveN,       '5',Key::Up);
	KMA("Move cursor West",  MoveW,       Key::Left);
	KMA("Move cursor East",  MoveE,       Key::Right);
	KMA("Move cursor NW",    MoveNW,      '4');
	KMA("Move cursor NE",    MoveNE,      '6');
	KMA("Move cursor SW",    MoveSW,      '1');
	KMA("Move cursor SE",    MoveSE,      '3');

	
	KMA2("Scroll down", ScrollDown,  's',Key::CtrlDown);
	KMA2("Scroll up",   ScrollUp,    'w',Key::CtrlUp);
	KMA2("Scroll left", ScrollLeft,  'a',Key::CtrlLeft);
	KMA2("Scroll right",ScrollRight, 'd',Key::CtrlRight);
	KMA2("Show help",ShowHelp,'h',Key::F1);
//
//  uncomment this, if want to add facilities w/o workers  
//   	KM1.AddActionAssignKey(
//   		"Construct Facility",
// 		// must address bind explicitly to prevent name conflict
// 		// w/ boost::mpl::bind (for GCC)
//   		boost::lambda::bind(new_ptr<AddFacilitiy>(),
//   			 ll_const_cast<Window&>(
//  				 bind(&Map::GetParent,this)),
//   			 bind(&Widget::GetHeight,this)/2,
//   			 bind(&Map::coords,this),
//   			 bind(&std::auto_ptr<MapData>::get,
//   				  bind(&Map::pMD1,this))
//   			),
//   		'c');


	
	KM1.AddActionAssignKey("End turn", KeyMap::Action(
							   (
								   bind(&MapData::SwitchTurn,pMD1.get()),
								   bind((void (SPE::*)())&SPE::reset,&selected)
								   )
							   ),'e');

	
	
	
	KMA("(De)Select unit",SelectUnit,Key::Enter);
	
	KM1.AddActionAssignKey("Force screen redraw", KeyMap::Action(
							   bind(&RootWindow::ForceRepaint,
									&GetParent().GetRootWindow())
							   ),'r');
}

Map::Map () : coords(0,0), translation (0,0), cursorpos (0,0),
			  D1S (0,0),
			  D1(boost::extents[0][0]), EdgeWarning(false)
			  
{
	UpdateCursor();
	SetStyle(DisplayStyle(Fg::Yellow,Fg::Dark, Bg::Black));
	// +1 : last column filled w/ '\0'
}
void Map::OnStart () throw()
{

	pMD1.reset(new MapData);
	MapData::PlayerList PL;
  	tr1::shared_ptr<PlayerAssets> p1
  		(new LocalPlayer(GetParent().GetRootWindow(),pMD1.get(),"Player 1",300));
  	PL.push_back(p1);
  	tr1::shared_ptr<PlayerAssets> p2
  		(new LocalPlayer(GetParent().GetRootWindow(),pMD1.get(),"Player 2",300));
 	PL.push_back(p2);
	pMD1->Randomize(PL,18,37);
	// Declare drawable
	D1S.height= 2 * pMD1->GetHeight() + 2;
	D1S.width = 3 * pMD1->GetWidth() + 1;// last == NULL
	D1.resize(boost::extents[D1S.height][D1S.width+1]);
	DrawHexGrid();
	SetUpKeyMap();
}

struct HighlightTile
{
private:
	Scr::Screen &screen;
	Position translation;
public:
	HighlightTile(Scr::Screen &_screen,
				  Position _translation)
		: screen(_screen), translation(_translation) {}

	void operator()( const Scr::Position &t)
	{
			
		Position TilePos(t.row*2+(t.col&1),t.col*3);// left-top
		// piece
		TilePos-=translation;

		bool rows[3];
		bool cols[4];
		for (Uint i = 0 ; i<4 ; i++ )
			cols[i]= (TilePos.col+i < screen.GetWidth());
		for (Uint i = 0 ; i<3 ; i++ )
			rows[i]= (TilePos.row+i < screen.GetHeight());

		
#define PC(y,x,s)													\
			if (cols[x] and rows[y])								\
				screen << GotoYX(TilePos.row+y, TilePos.col+x) << s
		
		PC(1,0,slash);
		PC(2,0,backslash);
		PC(1,3,backslash);
		PC(2,3,slash);
		PC(2,2,underline);
		PC(2,1,underline);
		PC(0,2,underline);
		PC(0,1,underline);
	}
};

void PlotPath(Screen &screen, CanMove * warrior,
			  Position coords,
			  Position translation)throw()
{

	// display precomputed path
	const wchar_t arrow_n  = 0x2191;  
	const wchar_t arrow_nw = 0x2196; 
	const wchar_t arrow_ne = 0x2197;  
	const wchar_t arrow_s  = 0x2193;  
	const wchar_t arrow_sw = 0x2199;  
	const wchar_t arrow_se = 0x2198;  
			
			
	PathFinder & PF1 = warrior->ComputePath(coords);
			
	PathFinder::Path::iterator i = PF1.GetPath().begin();
			
#define PTFORMULA i->row*2+(i->col&1)+1 - translation.row,	\
		i->col*3+2 - translation.col
			
	Position pt1(PTFORMULA); // "old" position
	Position pt2(pt1); // "new" position

	int TurnCounter=0;
	int APCounter=warrior->GetAP();
	for (++i;i!= PF1.GetPath().end() ; ++i )
	{
				
		pt2=Scr::Position (PTFORMULA) ;
		bool CountTurns = false;
		int cost=warrior->MovementCost(*i);
		if (APCounter<cost)
		{
			APCounter = warrior->GetAPMax();
			TurnCounter++	;
			if (TurnCounter<10)
				CountTurns=true;
		}
		APCounter-=cost;
		if (pt2.col < screen.GetWidth()
			and pt2.row< screen.GetHeight())
		{
			try{
				screen.GotoYX(pt1.row, pt1.col);

				if (CountTurns)
				{
					screen << TurnCounter;
					CountTurns=false;
				}
				else if (pt1.col == pt2.col)
				{
					if (pt1.row<pt2.row)
						screen.AddCharacter(arrow_s);
					else
						screen.AddCharacter(arrow_n);
				}
				else if (pt1.col < pt2.col)
				{
					if (pt1.row<pt2.row)
						screen.AddCharacter(arrow_se);
					else
						screen.AddCharacter(arrow_ne);
				}
				else
				{
					if (pt1.row<pt2.row)
						screen.AddCharacter(arrow_sw);
					else
						screen.AddCharacter(arrow_nw);
				}
				//screen.AddCharacter();
			}catch(...) {;}
		}
		pt1=pt2;
	}
	if (TurnCounter>1)
	{
		try{
			if (TurnCounter>9)
				screen.GotoYX(pt1.row, pt1.col-1);
			else
				screen.GotoYX(pt1.row, pt1.col);
			screen << TurnCounter+1;
		}catch(...) {;}
	}
#undef PTFORMULA
}
// End of
//
// void PlotPath(Screen &screen, tr1::shared_ptr<Entity> warrior,
// 			  Position coords,
// 			  Position translation)throw()




void Map::OnRedraw(Scr::Screen &screen)throw()
{
	screen << GetStyle() << Fg::Dark;

	//Postprocess grid and display it
	try
	{
		for (unsigned int j = 0 ; j!=GetHeight()
				 and j!=D1S.height-translation.row ; ++j )
		{
			screen.GotoYX(j,0);
			screen.AddTextCols(
				&D1[j+translation.row][translation.col],
				GetWidth());
		}
	}
	catch(...)
	{
		// programming error must have happened - can't
		// fix :(
		GetParent().GetRootWindow().Exit(1);
	}

	HighlightTile HT1(screen,translation);
	screen << Fg::Bright;
	// draw warriors;
	for (Uint i = min ( (GetHeight()+translation.row-1)/2, pMD1->GetHeight()-1),
			 j1 = min ( (GetWidth()+translation.col-2)/3, pMD1->GetWidth()-1),
			 x1 = 1+3*j1-translation.col,
			  y = 2*i+1-translation.row;
		 (int)y >= -1 and static_cast<int>(i)>=0 ; i--, y-=2)
	{
		Uint x = x1;
		Uint j=j1;
		for (; static_cast<int>(x)>=0; j--,x-=3)
		{
			Uint y_ = y+(j&1);
			std::tr1::shared_ptr<Entity> pE1 = pMD1->GetEntity(i,j);
			if ( pE1 and y_<GetHeight() )
			{
				screen.GotoYX(y_,x);
				if (PlayerAssets * pPA = pE1->GetAllegiance())
					screen.SetFgColor(pPA->GetColor());
				else
					screen.SetFgColor(Fg::Red);
					
				screen.AddCharacter(pE1->GetSymbol());
			}
			if ( PlayerAssets * ZoI = pMD1->GetZoI(i,j) )
			{
				screen.SetFgColor(ZoI->GetColor());
				HT1(Position(i,j));
			}
		}
		if ( static_cast<int>(j)>=0 )
			if ( PlayerAssets * ZoI = pMD1->GetZoI(i,j) )
			{
				screen.SetFgColor(ZoI->GetColor());
				HT1(Position(i,j));
			}
	}

	if (selected)
	{
		screen << Fg::Bright << Fg::White;
		Position p(selected->GetPosition());

		if (CanMove * cm = dynamic_cast<CanMove*>(selected.get()))
		{
			if (!pMD1->GetEntity(coords.row,coords.col))
				PlotPath(screen,cm,coords,translation);
		}

		if (CanAttack* w = dynamic_cast<CanAttack*>(selected.get()))
		{
			if (selected->GetAP()// can we attack enemies ?? 
				// if so, plot range
				and (Distance(coords,selected->GetPosition())<=w->GetRange()))
				for_each_position(p,pMD1->GetSize(),
								  w->GetRange(),
								  if_then(
									  bind(&MapData::GetZoI
										   ,pMD1.get()
										   ,bind(&Position::row,_1)
										   ,bind(&Position::col,_1))
									  !=selected->GetAllegiance(),
									  //ret<void>(
									  bind(&HighlightTile::operator(),
										   &HT1,_1)));
		}
		else if (dynamic_cast<CanProduceBase*>(selected.get()))
		{
			if (selected->GetAP()==selected->GetAPMax())
				for_each_position(p,pMD1->GetSize(),
								  1,
								  if_then(
									  bind(&MapData::GetZoI
										   ,pMD1.get()
										   ,bind(&Position::row,_1)
										   ,bind(&Position::col,_1))
									  ==selected->GetAllegiance()
									  and
									  not bind(&SPE::get,
									  bind(&MapData::GetEntity
										   ,pMD1.get()
										   ,bind(&Position::row,_1)
										   ,bind(&Position::col,_1)))
									  ,
									  //ret<void>(
									  bind(&HighlightTile::operator(),
										   &HT1,_1)));

		}
		HT1(p);

	}
	if (active)
	{
		try{
			screen.ForceCursorPosition(cursorpos-translation);
			screen.ShowCursor();
		}
		catch (Screen::RangeError)
		{
			screen.HideCursor();
		}
		catch (Screen::CursorVisibilityNotSupported)
		{
			;// in most cases it means, that RexIO can't hide cursor,
			//so it is already visible
		}

	}
	
}

void Map::UpdateCursor()
{	
	cursorpos.row=2*coords.row + (coords.col & 1)+1;
	cursorpos.col=3*coords.col+1;
}




void Map::UpdateTranslation()
{
	if (cursorpos.row < translation.row)
		translation.row =
			cursorpos.row>GetHeight()/2?
			cursorpos.row-GetHeight()/2:0;
	else if (cursorpos.row > translation.row + GetHeight())
		translation.row =
			D1S.height-cursorpos.row>GetHeight()/2?
			cursorpos.row-GetHeight()/2:
			D1S.height-GetHeight();

	if (cursorpos.col < translation.col)
		translation.col =
			cursorpos.col>GetWidth()/2?
			cursorpos.col-GetWidth()/2:0;
	else if (cursorpos.col > translation.col + GetWidth())
		translation.col =
			D1S.width-cursorpos.col>GetWidth()/2?
			cursorpos.col-GetWidth()/2:
			D1S.width-GetWidth();
}

void Map::MoveN()throw()
{
	if ( coords.row > 0 )
		coords.row--;
	else EdgeWarning=true;
	UpdateCursor();
	if ((cursorpos.row-translation.row)<5)
	{
		if (translation.row>2)
			translation.row-=2;
		else
			translation.row=0;			
	}
	UpdateTranslation();	
}
void Map::MoveS()throw()
{
	if ( coords.row < pMD1->GetSize().height-1 )
		coords.row++;
	else EdgeWarning=true;
	UpdateCursor();
	if ((cursorpos.row-translation.row)>GetHeight()-5)
	{
		translation.row+=2;
		if (D1S.height<translation.row+GetHeight())
			translation.row=
				D1S.height>GetHeight()
				?D1S.height-GetHeight():0;
	}
	UpdateTranslation();
}
void Map::MoveW()throw()
{

	if ( coords.col > 0 )
		coords.col--;
	else EdgeWarning=true;
	UpdateCursor();
	if ((cursorpos.col-translation.col)<10)
	{
		if (translation.col>6)
			translation.col-=6;
		else
			translation.col=0;			
	}
	UpdateTranslation();	
}
void Map::MoveE()throw()
{

	if ( coords.col < pMD1->GetSize().width-1 )
		coords.col++;
	else EdgeWarning=true;
	UpdateCursor();
	if ((cursorpos.col-translation.col)>GetWidth()-10)
	{
		translation.col+=6;
		if (D1S.width<translation.col+GetWidth())
			translation.col=D1S.width>GetWidth()?
				D1S.width-GetWidth():0;
	}
	UpdateTranslation();	
}


void Map::MoveSW()throw()
{
	MoveW();
	if (not (coords.col & 1) )
		MoveS();
}

void Map::MoveNW()throw()
{
	MoveW();
	if ( (coords.col & 1))
		MoveN();
}

void Map::MoveSE()throw()
{
	MoveE();
	if (not ( coords.col & 1) )
		MoveS();
}

void Map::MoveNE()throw()
{
	MoveE();
	if (coords.col & 1)
		MoveN();
}


void Map::ScrollUp()throw()
{
	if ( translation.row > 0 )
		translation.row--;
	else EdgeWarning=true;
	
}
void Map::ScrollDown()throw()
{
	
	if ( D1S.height>GetHeight() &&
		 translation.row < D1S.height-GetHeight() )
		translation.row++;
	else EdgeWarning=true;
}
void Map::ScrollLeft()throw()
{
	if ( translation.col > 1 )
		translation.col-=2;
	else if ( translation.col == 1 )
		translation.col=0;
	else EdgeWarning=true;
	
}
void Map::ScrollRight()throw()
{
	if (D1S.width>GetWidth() &&
		translation.col < D1S.width-GetWidth())
		translation.col+=2;
	else EdgeWarning=true;
}

void Map::ShowHelp()throw()
{
#define BOLD "\x1b[1m"
#define UNBOLD "\x1b[0m"
	system("clear");
	FILE * help = popen("less -R","w");
	fprintf(help,BOLD "dtactics - experimental game written using RexIO "
			"library" UNBOLD "\n\n");
	fprintf(help,"You are viewing this memo thanks to \x1b[1mless\x1b[0m"
		" program, which\nis installed in your system.\n\n ");
	fprintf(help,"Command summary: \n");
	KM1.PrintHelp(help);
	fprintf(help,"When units are selected, their key mappings override "
			"defaults: \n");
	Entity::GetKeyMap().PrintHelp(help);

	fprintf(help,"\n\n"
			"When you will construct new units or facilities, you will\n"
			"see list of entity traits. They are very important part of\n"
			"this game, as they describe units' capabilities.\n"
			"\nFollowing traits may appear:\n"
			BOLD "M" UNBOLD " stands for Morale. units with M trait won't"
			"surrender,\n"
			"when enemies appear.\n"
			BOLD "M+" UNBOLD " trait also enables Zone of Influence expanxion"
			" (units \nwithout M can't even move out of your ZoI)\n"
			BOLD "Air" UNBOLD " stands for aircraft. Air units are very fast"
			" they pass \n"
			"all obstacles and bombers can deliver tons of destructive payload,\n"
			"but planes can operate only within specific range around your\n"
			"airbases., so operate them carefully\n"
			BOLD "W" UNBOLD " denotes wheeled units, which are very fast, when they"
			" use \n"
			"roads (otherwise they can move only by one tile per turn)\n"
			BOLD "AA" UNBOLD " means, that specific warrior carries anti-aircraft"
			" weapons,\n"
			"AA warfare gains unique +60 accuracy bonus, when they fight \n"
			"air units.\n"
			BOLD "P" UNBOLD " means, that this unit have production capabilities,"
			" that is\n"
			"it can construct new facilities (in case of worker) or train new\n"
			"warriors (in case of barracks and factories)\n"
			BOLD "T" UNBOLD " stands for Transport."
		);

	fflush(help);
	
	pclose(help);
	wait(0);
	GetParent().GetRootWindow().ForceRepaint();
}//void Map::ShowHelp()throw()


void Map::SelectUnit()throw()
{
	if (! selected.get())
	{
		selected = pMD1->GetEntity(coords.row, coords.col);
		if (selected.get() and GetActive()!=selected->GetAllegiance())
			selected.reset();
	}
	else
	{
		selected->GetKeyMap().ExecuteFor(selected.get(),coords,Key::Enter);
		selected.reset();
	}
}
void Map::UpdateStatus()
{
	stringstream ss;
	ss << pMD1->GetPlayerName() << " turn "<< pMD1->GetTurn()
	   <<" \342\224\202 oil reserves: " << pMD1->GetActive()->GetOil() << "/"
	   << pMD1->GetActive()->GetIncome();
	ss << " \342\224\202 coords:" << setw(3) << coords.col+1 << ',' << setw(2)<<coords.row+1;
	if (EdgeWarning)
		ss << " (edge of board)";
// 	ss << " \342\224\202 "
// 	   << (selected.get()?selected->GetName()+" selected":"No unit selected");
	StatusBar::SetStatus(ss.str());


	if ( Entity * pE = pMD1->GetEntity(coords.row,coords.col ).get())
		if ( selected and selected->GetAllegiance()!=pE->GetAllegiance() )
			if ( CanAttack* w = dynamic_cast<CanAttack*>(selected.get()) )
				if ( Distance(w->GetPosition(),coords) <= w->GetRange() )
				{
					stringstream ss;
					ss << "Chance to hit: "<<w->ChanceToHit(pE)<<'%';
					HintBox::H1.push_back(ss.str());
				}
}

void Map::OnKeyDown(Key key)throw()
{
	HintBox::H1.clear();
	EdgeWarning=false;
	try
	{
		if ( not KM1.Execute(key) and selected)
			selected->GetKeyMap()
				.ExecuteFor(selected.get(),coords,key);
	}
	catch (Entity::ActionNotSupported & e)
	{
		//HintBox::H1.push_front(string("Exception: ")+e.what());
		Alert(GetParent().GetRootWindow(),string("Exception: ")+e.what());
	}
	
	UpdateStatus();
}
void Map::OnFocus(FocusPolicy focustype)throw()
{
	if (pMD1.get())
		UpdateStatus();
	ActiveWidget::OnFocus(focustype);
}


 Uint Map::GetDrawableHeight() const throw()
{
	return D1.shape()[0];
}
 Uint Map::GetDrawableWidth() const throw()
{
	return D1.shape()[1];
}

Map::~Map()throw()
{
}
