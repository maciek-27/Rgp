#include<rexio/tk/rootwindow.h++>
#include<rexio/tk/scrollbar.h++>
#include<iostream>
#include "map.h++"
#include "statusbar.h++"
#include "hintbox.h++"
#include "playerassets.h++"
#include "alert.h++"
#include <numeric>
#include <algorithm>
#include <boost/lambda/lambda.hpp>

#include <boost/lambda/casts.hpp>
#include <boost/lambda/bind.hpp>

using namespace std;
using namespace Scr;
using namespace Scr::Tk;
using namespace Scr::Control;

//! used by Alert function
class AlertWindow: public Window
{
	Label * msg;//!< proper message
	Label * anyk; //!< any key message
public:
	//! constructor
	//! \param w parent
	//! \param message alert message
	//! \param _height  as for Scr::Tk::Window
	//! \param _width  as for Scr::Tk::Window
	//! \param _style as for Scr::Tk::Window
	AlertWindow(Scr::Tk::Window & w,
				const std::string message,
				Uint _height,	
				Uint _width,
				const DisplayStyle &  _style
				= DisplayStyle(Fg::White,Fg::Dark,Bg::Black))
		:Window(_height,_width,_style)
		{
			w.AddWidget(*this);
			w.SetActiveWidget(*this);
			msg = new Label(GetWidth(),message,_style);
			AddWidget(*msg);
			static const string anykey("Press any key to continue.");
			anyk=new Label(anykey.length(),anykey,_style);
			AddWidget(*anyk);
			OnResize();
		}
	//! OnKeyDown: any key to continue (object commits suicide)
	void OnKeyDown(Scr::Key key)throw()
		{
			GetParent().DelWidget(*this);
			delete this;
		}
	//! On Focus do nothing (that is: keep focus!)
	virtual void OnFocus(FocusPolicy focustype)throw()
	// noop OnFocus is method to disable tab focus switching w/ RexIO.
		{}
	//!  adjust messages
	void OnResize()throw()
		{
			msg->SetRow(0);
			msg->SetCol(0);
			anyk->SetRow(1);
			anyk->SetCol(GetWidth()-anyk->GetWidth());
		}
	//! clear resources
	~AlertWindow()throw()
		{
			delete msg;
			delete anyk;
		}
};//AlertWindow

void Alert(Scr::Tk::Window & parent,const std::string &message,
		   const Scr::DisplayStyle &  _style)
{
	new AlertWindow(parent,
					message,
					2,parent.GetWidth(),_style);
}

#if defined _GNU_SOURCE
// other platforms supply compose2 as part of std namespace in
// functional header;
#include <ext/functional>
using __gnu_cxx::compose2;
#endif


using namespace boost::lambda;
//! Contains map, scrollbars and info area on left/right margin
class MapWindow: public Window
{
	//! margin infoarea;
	class : public Widget
	{
	public:
		//! pointer to entity, that will be described
	    Entity * ToDescribe;
		//! OnRedraw: display info on  selected (or focused) entity.
		//! \param screen as for any other widget
		void OnRedraw(Scr::Screen &screen)throw()
			{
				screen.Clear();
				if (!ToDescribe)
					return;
				screen.GotoYX(0,0);
				if (ToDescribe->GetAllegiance())
					screen << ToDescribe->GetAllegiance()->GetColor();
				string name(ToDescribe->GetName());
				name.resize(GetWidth(),'.');
				screen << name;
				if (ToDescribe->GetAPMax())
				{
					screen.GotoYX(screen.GetY()+1,0);
					screen << "AP: "
						   << ToDescribe->GetAP()
						   << '/' << ToDescribe->GetAPMax();
				}
			}
		
	}Margin;
	//! Map
	Map M1;
	//! scroll bar
	HorizontalScrollbar HS1;
	//! scroll bar
	VerticalScrollbar VS1;
public:
	//! Constructor
	//! \param _height  as for Scr::Tk::Window
	//! \param _width  as for Scr::Tk::Window
	//! \param _style as for Scr::Tk::Window
	MapWindow(Uint _height, Uint _width,
			  const DisplayStyle &_style
			  =DisplayStyle(Fg::White,Fg::Dark, Bg::Black))throw()
		:Window (_height,  _width,  _style),
		 HS1(_width-1,_style),VS1(_height-1,_style)
		{
			AddWidget(M1);
			AddWidget(HS1);
			AddWidget(VS1);
			AddWidget(Margin);
			Margin.SetWidth(20);
			Margin.ToDescribe=0;
		}

	
// 	void OnStart()throw()
// 		{
// 			Window::OnStart();
// 		}

	//! return active map entity
	PlayerAssets * GetActive()
		{
			return M1.GetActive();
		}

	//! redraw all elements, but firstly verify conditions
	void OnRedraw(Scr::Screen &screen)throw()
		{
			// to prevent scrollbar crash
			// (Scrollbar*::OnRedraw crashes when doesn't have at
			// least 3 character fields - this is bug in RexIO, that
			// will be fixed someday)
			if (GetWidth()<Margin.GetWidth()+3)
				return; 
			if (GetHeight()<3)
				return;
			if (!(Margin.ToDescribe=
				  M1.pMD1->GetEntity(M1.coords.row,M1.coords.col).get()))
				Margin.ToDescribe=M1.selected.get();
			Window::OnRedraw(screen);
		}
	//! manual layout processing
	void OnResize()throw()
		{
			Uint WidthAvail=GetWidth()-Margin.GetWidth();
			if (M1.GetDrawableWidth()>WidthAvail)
				M1.SetHeight(GetHeight()-1);
			else
				M1.SetHeight(GetHeight());
			
			if (M1.GetDrawableHeight()>GetHeight())
				M1.SetWidth(WidthAvail-1);
			else
				M1.SetWidth(WidthAvail);
			HS1.SetWidth(WidthAvail-1);
			HS1.SetRow(M1.GetHeight());
			VS1.SetHeight(GetHeight()-1);
			VS1.SetCol(M1.GetWidth());
			Margin.SetHeight(GetHeight());
			Margin.SetCol(WidthAvail);
		}
};//MapWindow

//! As name suggests: main game window
class GameMainWindow: public RootWindow
{
private:
	StatusBar SB1;
	HintBox HB1;
	MapWindow MW1;
public:
	//! Default ctor - does lots of setup
	GameMainWindow()throw()       :RootWindow(cin,cout), MW1(0,0)
	{

		SetStyle(DisplayStyle(Fg::Yellow,Fg::Bright, Bg::Black));
		
		HintBox::H1.push_back("Welcome to dtactics. If you are new to "
			"this game, press h for help");
		
		AddWidget(MW1);
		AddWidget(SB1);
		AddWidget(HB1);
		SB1.SetHeight(1);
		for_each(elements.begin(), elements.end(),
				 (bind(&Widget::SetCol,_1,0)
				  ,bind(&Widget::SetStyle,_1,
						DisplayStyle(Fg::Yellow,Fg::Bright, Bg::Black)))
			);

		//these commands are processed only when map is active

		//Statusbar may be moved upwards, up to 1/2 of screen height
		KM1.AddActionAssignKey("move statusbar up",
			KeyMap::Action
					  ( bind(&HintBox::SetHeight,&HB1,
							 bind(std::min<unsigned,less<unsigned> >,
								  bind(&HintBox::GetHeight,&HB1)+1,
								  bind(&HintBox::GetHeight,this)/2,					   
								  less<unsigned>())
						  )),'\''
			);
		
		//It can also be moved down, and leave only 1 line for HintBox
		KM1.AddActionAssignKey("move statusbar down",
							   KeyMap::Action
							   (bind(&HintBox::SetHeight,&HB1,
									 bind(std::max<unsigned,less<unsigned> >,
										  bind(&HintBox::GetHeight,&HB1)-1,
										  1,less<unsigned>())
								   )),'/'
			);
		KM1.AddActionAssignKey("End game",bind(&RootWindow::Exit,this,0),
							   Key::F10);
		KM1.AssignKey('q',"End game");
		
		PassFocusRequest(TabFocus);
		for_each(elements.begin(), elements.end(),
				 bind(&Widget::SetWidth,_1,
					  bind(&Widget::GetWidth, this) ) );
	}
	
	// recalculate layout (widths only, as heights are invalidated
	// also w/o resize of main window)
	void OnResize()throw()
    {
		for_each(elements.begin(), elements.end(),
				 bind(&Widget::SetWidth,_1,
					  bind(&Widget::GetWidth, this) ) );


		RootWindow::OnResize();
    }
	//! On redraw: paint on screen
    void OnRedraw(Scr::Screen &screen)throw()
    {
		try
        {
			// Verify MW1 height (if other elements grow, MW1 must
			// shrink and vice-versa
			MW1.SetHeight(
				std::accumulate(elements.begin(),elements.end(),
								GetHeight()+MW1.GetHeight(),
								_1-bind(&Widget::GetHeight,_2))
				);
			// set disp. style
			SB1.SetStyle(DisplayStyle(MW1.GetActive()->GetColor(),
									  Fg::Bright, Bg::Black));
			// must recalculate positions, as HintBox may change its
			// size to display some interesting tips :)
			int fill=0;
			// set row for each element
			for_each(elements.begin(), elements.end(),
					(
						bind(&Widget::SetRow,_1,var(fill)),
						fill+=bind(&Widget::GetHeight,_1)

					));
			// call parent procedure
			RootWindow::OnRedraw(screen); // (clear and redraw)
			
		}
        catch (...)
        {
            Exit(1);// if anything bad happens
        }
    }
	//! empty
    ~GameMainWindow()throw(){;}
};//GameMainWindow


int main (int argc,char ** argv)
{
    RootWindow * app = new GameMainWindow;
    int result = app->Start(argc, argv);
    delete app;
    return result;
}
/*end of main function of program*/
