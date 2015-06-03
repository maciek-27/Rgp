#include "scrollbar.h++"

using namespace Scr;
using namespace Scr::Tk;

ScrollbarBase::ScrollbarBase(Uint _width, Uint _height,
							 const ScrollbarStyle &_scrollbarStyle)throw()
	:Widget(_width, _height)
{
	scrollbarStyle = _scrollbarStyle;
}

void ScrollbarBase::SetScrollSize(Uint _scrollSize)throw()
{
	scrollSize = _scrollSize;
}

Uint ScrollbarBase::GetScrollSize() const throw()
{
	return scrollSize;
}

void ScrollbarBase::SetScrollOffset(Uint _scrollOffset)throw()
{
	scrollOffset = _scrollOffset;
}

Uint ScrollbarBase::GetScrollOffset() const throw()
{
	return scrollOffset;
}

void ScrollbarBase::SetScrollProgress(float progress)throw()
{
	SetScrollOffset(static_cast<Uint>(progress *
									  static_cast<float>(GetScrollSize())));
}

float ScrollbarBase::GetScrollProgress() const throw()
{
	return static_cast<float>(GetScrollOffset())/
		static_cast<float>(GetScrollSize());
}

void ScrollbarBase::SetScrollbarStyle(const ScrollbarStyle& _scrollbarStyle)
	throw()
{
	RedrawRequest();
	scrollbarStyle = _scrollbarStyle;
}

const ScrollbarStyle& ScrollbarBase::GetScrollbarStyle() const throw()
{
	return scrollbarStyle;
}

HorizontalScrollbar::HorizontalScrollbar(
	Uint _width, const ScrollbarStyle& _scrollbarStyle)throw()
	: ScrollbarBase(1, _width, _scrollbarStyle) { ; }

void HorizontalScrollbar::OnRedraw(Screen& screen)throw()
{
   	screen<< Control::GotoYX(0, 0)
		  << scrollbarStyle.button << scrollbarStyle.buttonLeft
		  << Control::GotoYX(0, 1) << scrollbarStyle.scrollBg;

	screen.HorizontalLine(scrollbarStyle.scrollField, GetWidth() - 2);

	Uint drawOffset = 0;
	Uint handleSize = 1;
	if(scrollSize <= GetWidth()) {
		handleSize = GetWidth() - 2;
	}
	else if(scrollSize > GetWidth() && scrollSize <= 2*GetWidth() - 3) {
		handleSize = 2*GetWidth() - scrollSize - 2;
		drawOffset = scrollOffset;
	}
	else
		drawOffset = float(scrollOffset) /
			float(scrollSize - GetWidth()) * (GetWidth() - 2);

	screen << Control::GotoYX(0, 1 + drawOffset) << scrollbarStyle.scrollFg;
	screen.HorizontalLine(scrollbarStyle.scrollHandleH, handleSize);

	screen << Control::GotoYX(0, GetWidth() - 1)
		   << scrollbarStyle.button << scrollbarStyle.buttonRight;
}

VerticalScrollbar::VerticalScrollbar(
	Uint _height, const ScrollbarStyle& _scrollbarStyle)throw()
	: ScrollbarBase(_height, 1, _scrollbarStyle) { ; }

void VerticalScrollbar::OnRedraw(Screen& screen)throw()
{
	screen << Control::GotoYX(0, 0)
		   << scrollbarStyle.button << scrollbarStyle.buttonUp
		   << Control::GotoYX(1, 0) << scrollbarStyle.scrollBg;
	
	screen.VerticalLine(scrollbarStyle.scrollField, GetHeight() - 2);

	Uint drawOffset = 0;
	Uint handleSize = 1;
	if(scrollSize <= GetHeight()) {
		handleSize = GetHeight() - 2;
	}
	else if(scrollSize > GetHeight() && scrollSize <= 2*GetHeight() - 3) {
		handleSize = 2*GetHeight() - scrollSize - 2;
		drawOffset = scrollOffset;
	}
	else
		drawOffset = float(scrollOffset) /
			float(scrollSize - GetHeight()) * (GetHeight() - 2);

	screen << Control::GotoYX(1 + drawOffset, 0) << scrollbarStyle.scrollFg;
	screen.VerticalLine(scrollbarStyle.scrollHandleV, handleSize);

	screen << Control::GotoYX(GetHeight()-1, 0)
		   << scrollbarStyle.button << scrollbarStyle.buttonDown;
}
