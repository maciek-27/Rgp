#include "window.h++"

using namespace Scr;
using namespace Scr::Tk;

Window::Window(Uint _height,	
			   Uint _width,
			   const DisplayStyle &  _style)throw()
    :Widget(_height, _width, _style)
{
    activeWidget = elements.end();
}

Screen& Window::GetScreen()throw(ParentNotDefined)
{
    return GetParent().GetScreen();
}

Uint Window::GetAbsoluteColumn()throw(ParentNotDefined)
{
    return GetParent().GetAbsoluteColumn() + position.col;
}

Uint Window::GetAbsoluteRow()throw(ParentNotDefined)
{
    return GetParent().GetAbsoluteRow() + position.row;
}

void Window::SetStylesheet(Stylesheet* _styleSheet)throw()
{
	Widget::SetStylesheet(_styleSheet);
    for(WidgetList::iterator i=elements.begin(); i!=elements.end();++i)
		(*i)->SetStylesheet(_styleSheet);
}

void Window::AddWidget(Widget& widget)
	throw(ParentAlreadySet, WidgetAlreadyAdded)
{
	if(elements[&widget] != elements.end())
		THROW(WidgetAlreadyAdded);

	widget.SetParent(*this);
	if(styleSheet)
		widget.SetStylesheet(styleSheet);
    elements.push_back(&widget);
	// each widget must recv. OnStart. activeWidget is set after
	// OnStart, so when it is set, certainly Window::OnStart() was
	// already called!
	if(activeWidget != elements.end())
		widget.OnStart();
}

void Window::DelWidget(Widget& widget)
	throw(WidgetNotPresent)
{
	if (&widget==*activeWidget)
		PassFocusRequest(TabFocus);
	if(elements[&widget] == elements.end())
		THROW(WidgetNotPresent);
    elements.remove(&widget);
	widget.ReParent(NULL);
}

RootWindow& Window::GetRootWindow()throw(ParentNotDefined)
{
	return GetParent().GetRootWindow();
}

void Window::RedrawRequest(Widget& widget)throw()
{
	if(IsHidden())
		return;

	if (parentWindow != this) // this is not a root win
	{
		GetParent().RedrawRequest(*this);
	} 
	else
	{
		Screen & screen=GetScreen();// main screen

		
		WidgetList::iterator i;
		for (i=elements[&widget]; i!=elements.end ();++i)
		{
			if((*i)->IsHidden ())
				continue;
			
			// working subscreen
			Screen * ss;
			try
			{
				ss = screen.CreateSubScreen ((*i)->GetRow (), (*i)->GetCol (),
						(*i)->GetHeight (), (*i)->GetWidth ());
			}
			catch (Scr::Screen::SubscreenOutOfRange)
			{
				continue;
			}
			(*i)->OnRedraw (*ss);
			delete ss;
		}
		
		screen.Refresh();
		
	}	
}

void Window::RedrawRequest()throw()
{
	if (! elements.empty())
	RedrawRequest(**(elements.begin()));
}

void Window::PassFocusRequest(FocusPolicy focustype)throw() 
{	
  return ;
  if (!elements.empty()) {		
    if(activeWidget != elements.end()) {			
      (*activeWidget)->OnUnFocus(focustype);
      ++activeWidget;
    }
		
   		
    if (activeWidget == elements.end()) {
      if(focustype == TabFocus) {
	parentWindow->PassFocusRequest(focustype);				
	return;
      }
      // activeWidget = elements.end();
      // return;
    }
    
    if((*activeWidget)->IsHidden ())
      PassFocusRequest(focustype);
    else
      (*activeWidget)->OnFocus(focustype);
  }
}

void Window::SetActiveWidget(Widget &w)
	throw(WidgetNotPresent)
{
	if(activeWidget != elements.end())
	{
		if (&w==*activeWidget)
			return;
		(*activeWidget)->OnUnFocus(AllFocus);
	}
	activeWidget = elements[&w];
	if(activeWidget == elements.end())
		THROW(WidgetNotPresent);
	(*activeWidget)->OnFocus(AllFocus);
}

Widget& Window::GetActiveWidget()const throw(WidgetNotPresent)
{
	if(activeWidget == elements.end())
	   THROW(WidgetNotPresent);
	return **activeWidget;
}

void Window::OnFocus(FocusPolicy focustype)throw()
{
  if (elements.empty())
    return;
	if(activeWidget == elements.end()) {
		activeWidget = elements.begin();
		if(activeWidget != elements.end())
			(*activeWidget)->OnFocus(focustype);
		else
			parentWindow->PassFocusRequest(focustype);
	}
	else {
		(*activeWidget)->OnFocus(focustype);
	}
}

void Window::OnUnFocus(FocusPolicy focustype)throw()
{
	if(activeWidget != elements.end()) {
		(*activeWidget)->OnUnFocus(focustype);
	}
}

void Window::OnStart()throw()
{
    WidgetList::iterator i;
    for (i=elements.begin(); i!=elements.end();++i)
		(*i)->OnStart();
    activeWidget=elements.begin();
}

void Window::OnResize()throw()
{
    for(WidgetList::iterator i=elements.begin(); i!=elements.end();++i)
		(*i)->OnResize();
}

void Window::OnRedraw(Screen& screen)throw()
{
    screen << GetStyle() << Control::Clear;

    WidgetList::iterator i;
    for (i=elements.begin(); i!=elements.end();++i)
    {
		if((*i)->IsHidden())
			continue;
		if ( (*i)->GetCol()+(*i)->GetWidth()>GetWidth())
 			continue;
 		if ( (*i)->GetRow()+(*i)->GetHeight()>GetHeight())
 			continue;
		Screen * ss;
		try
		{
			ss = screen.CreateSubScreen((*i)->GetRow(), (*i)->GetCol(),
										(*i)->GetHeight(), (*i)->GetWidth());
		}
		catch (Scr::Screen::SubscreenOutOfRange)
		{
			continue;
		}
		(*i)->OnRedraw(*ss);
		delete ss;
    }
}

void Window::OnKeyDown(Key key)throw()
{
	if (activeWidget!=elements.end()) {
		(*activeWidget)  ->OnKeyDown(key);//pass to activeWidget element; 
    }
}

void Window::SetSize(const Size& _size)throw()
{
	if(!parentWindow)
		return;
	Widget::SetSize(_size);
}
