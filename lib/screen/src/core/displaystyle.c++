#include"screen.h++"

Scr::DisplayStyle::DisplayStyle(Fg::Color _fgColor,
								Fg::Style _fgStyle,
								Bg::Color _bgColor)throw()
{
	style=0;
	SetFgColor(_fgColor);
	SetFgStyle(_fgStyle);
	SetBgColor(_bgColor);
}

Scr::DisplayStyle::DisplayStyle(const DisplayStyle & base)throw()
{
	style=base.style;
}

Scr::DisplayStyle::DisplayStyle()throw(){style=0;}

