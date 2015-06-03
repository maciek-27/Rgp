#include "boxgroup.h++"

using namespace Scr;
using namespace Scr::Tk;

// TODO: with error accumulator
#define PRECISION 1000

BoxGroup::BoxGroup(Uint _height, Uint _width,
				   const DisplayStyle & _style)throw()
	: WidgetGroup(_height, _width, _style),
	  alignPolicy(Begin)
{
	;
}

BoxGroup::BoxGroup(const WidgetGroup & base)throw()
	: WidgetGroup(base),
	  alignPolicy(Begin)
{
	;
}

BoxGroup::~BoxGroup()throw(){;}

void BoxGroup::SwapWidgets(Widget& widget1, Widget& widget2)throw()
{
	elements.swap(&widget1, &widget2);
	OnResize();
	
}

void BoxGroup::AddWidget(Widget& widget)throw()
{
	elementsLayout[&widget] = LayoutData(1);
	Window::AddWidget(widget);
	ArrangeContents();
}

void BoxGroup::AddWidget(Widget& widget, Uint stretchFactor)throw()
{
	elementsLayout[&widget] = LayoutData(stretchFactor);
	WidgetGroup::AddWidget(widget);
	ArrangeContents();
}

void BoxGroup::DelWidget(Widget& widget)throw()
{
	elementsLayout.erase(&widget);
	WidgetGroup::DelWidget(widget);
	ArrangeContents();
}

void BoxGroup::OnStart()throw()
{
//	ArrangeContents();
	WidgetGroup::OnStart();
}

void BoxGroup::OnResize()throw()
{
	ArrangeContents();
	WidgetGroup::OnResize();
}

void BoxGroup::SetAlignPolicy(AlignPolicy _alignPolicy)throw()
{
	if(alignPolicy != _alignPolicy)
		WidgetGroup::OnResize();
	alignPolicy = _alignPolicy;
}

BoxGroup::AlignPolicy BoxGroup::GetAlignPolicy()throw()
{
	return alignPolicy;
}
