#include "entity_types.h++" 
#include <algorithm>
#include "entity.h++"
#include "addentitydialog.h++"

#include <sstream>
#include "mapdata.h++"
#include "playerassets.h++"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>


using namespace boost::lambda;
using namespace Scr::Tk;
using namespace Scr;
using Scr::Control::Clear;
using std::string;

const DisplayStyle AddEntityDialogBase::SelectorDisplayStyle1
(Scr::Fg::Yellow,Scr::Fg::Bright,Scr::Bg::Black);

const DisplayStyle AddEntityDialogBase::SelectorDisplayStyle2
(Scr::Fg::Black,Scr::Fg::Dark,Scr::Bg::Yellow);


static boost::array<std::string,AddEntityDialogBase::num_columns> column_titles
= { {"Name", "Cost"/* to build"*/, "Maintainance"/* cost"*/, "Map symbol",
	 "Armor", "Attack", "Range", "Movement", "Traits"} };



	
AddEntityDialogBase::SelectorBase::SelectorBase
(const Scr::DisplayStyle& _style,
 const Scr::DisplayStyle& _activeStyle)
	:Button("", _style,	_activeStyle)
{

}		

void AddEntityDialogBase::SelectorBase::OnRedraw(Scr::Screen &screen)throw()
{
	Button::OnRedraw(screen);
	int j=0;
	AddEntityDialogBase
		&AF1(static_cast<AddEntityDialogBase&>(GetParent()));
	try{
		for (int i = 0 ; i!= columns.static_size ; i++)
		{
			screen.GotoYX(0,j);
			screen.AddText(columns[i]);
			j+=AF1.columns[i];
		}}
	catch(...){}
}

AddEntityDialogBase::AddEntityDialogBase( Window& w,
										  Scr::Uint  hmax)
	:Window(std::min<Scr::Uint>(8,hmax),w.GetWidth()),selection(0)
{
	w.AddWidget(*this);
	w.SetActiveWidget(*this);
}
void AddEntityDialogBase::AfterConstructorSetup()throw()
{

	if (!header.empty())		
		AddWidget(*(new Scr::Tk::Button("Sorry", SelectorDisplayStyle1,
										SelectorDisplayStyle2)));
	else				
		AddWidget(*(new Scr::Tk::Button("Cancel",
										SelectorDisplayStyle1,
										SelectorDisplayStyle2)));
	activeWidget=elements.begin();
	(*activeWidget)->OnFocus(AllFocus);
	SetHeight(std::min<Scr::Uint>(elements.size()+1,GetHeight()));
	
	OnResize();
}
void AddEntityDialogBase::OnRedraw(Scr::Screen &screen)throw()
{
	using Control::GotoYX;
	
	{//foreach element, set width, height ad position
		int i = -std::max<int>(0,selection-GetHeight()+2);
		for_each(elements.begin(), elements.end(),
			 (	 bind(&Widget::SetWidth,_1,GetWidth() ),
				 bind(&Widget::SetHeight,_1,1 ),
				 bind(&Widget::SetRow,_1,++var(i))));
	}
	Window::OnRedraw(screen);
	try{
	if (header.empty())
	{
		screen << SelectorDisplayStyle2 << GotoYX(0,0);
		screen.HorizontalLine(' ',GetWidth());
		for (int i = 0,j=0 ; i!= columns.static_size ; i++)
		{
		screen.GotoYX(0,j);
		screen.AddText(column_titles[i]);
		j+=columns[i];
		}
	}
	else
	{
		screen << Fg::Yellow << Bg::Black << GotoYX(0,0);
		screen.HorizontalLine(' ', GetWidth());
		screen << GotoYX(0,0)<<header;
	}
	}catch(...){};
}

void AddEntityDialogBase::OnKeyDown(Scr::Key key)throw()
{
	if (key==Scr::Key::Down)
	{
		(*activeWidget)->OnUnFocus(AllFocus);
		activeWidget++,selection++;
		if (activeWidget==elements.end())
			activeWidget=elements.begin(),
				selection=0;
		(*activeWidget)->OnFocus(AllFocus);
	}
	if (key==Scr::Key::Up)
	{
		(*activeWidget)->OnUnFocus(AllFocus);
		if (activeWidget==elements.begin())
			activeWidget=elements.end(),
				selection=elements.size();
		activeWidget--,selection--;
		(*activeWidget)->OnFocus(AllFocus);
	}
	if (key==Scr::Key::Enter)
	{
		static_cast<Button *>(*activeWidget)

			->OnAction();
		Suicide();
	}
}

void AddEntityDialogBase::Suicide()throw()
{
	GetParent().DelWidget(*this);
	delete this;
}

void AddEntityDialogBase::OnResize()throw()
{
	for (unsigned j = 0 ; j!=num_columns ; ++j)
		columns[j]=column_titles[j].length()+1;
 	WidgetList::iterator i = elements.begin();
 	for (; i!= elements.end() ; ++i)
	{
		SelectorBase*s=dynamic_cast<SelectorBase*>(*i);
		if (s)
			for (unsigned j = 0 ; j!=num_columns ; ++j)
				columns[j]=std::max<unsigned>(columns[j],
										  s->columns[j].length()+1);
 	}
	size_t sum = 
		std::accumulate(columns.begin(),columns.end(),0);
	std::for_each(columns.begin(),columns.end(),_1=_1*GetWidth()/sum);
}

void AddEntityDialogBase::OnFocus(Window::FocusPolicy focustype)throw()

{}
void AddEntityDialogBase::OnUnFocus(Window::FocusPolicy focustype)throw()
{}

AddEntityDialogBase::~AddEntityDialogBase()throw()
{
	for_each(elements.begin(), elements.end(), bind(delete_ptr(), _1));
	
}

