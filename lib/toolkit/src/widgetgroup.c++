#include <iostream>
#include "widgetgroup.h++"

using namespace Scr;
using namespace Scr::Tk;

WidgetGroup::WidgetGroup(Uint _height,	
						 Uint _width,
						 const DisplayStyle &  _style)throw()
    :Window(_height,_width,_style) {;}

WidgetGroup::WidgetGroup(const WidgetGroup & base)throw()
	: Window(base.size.height,base.size.width,base.style)
{
	;
}

WidgetGroup::~WidgetGroup()throw(){;}
void WidgetGroup::SwapWidgets(Widget& widget1, Widget& widget2)throw()
{	
	// swap positions - in case of really rumb WidgetGroup :)
	Position pos = widget1.GetPosition();
	widget1.SetPosition(widget2.GetPosition());
	widget2.SetPosition(pos);

	// swap on the list
	elements.swap(&widget1, &widget2);
	
	ArrangeContents();
}

void WidgetGroup::ArrangeContents()throw()
{
	;
}

// fugly, but works :)
void WidgetGroup::ShiftFWidget(Widget& widget)throw()
{
	WidgetList::iterator i = elements[&widget];
	WidgetList::iterator end = elements.end();	
	if(i != (--end))
		SwapWidgets(widget, **(++i));
	else
		SwapWidgets(widget, **(elements.begin()));
}

// fugly, but works :)
void WidgetGroup::ShiftBWidget(Widget& widget)throw()
{
	WidgetList::iterator i = elements[&widget];
	WidgetList::iterator end = elements.end();
	if(i != elements.begin())
		SwapWidgets(widget, **(--i));
	else
		SwapWidgets(widget, **(--end));
}
