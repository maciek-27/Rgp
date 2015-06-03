#include "button.h++"

using namespace Scr;
using namespace Scr::Tk;

Button::Button(Uint _height, Uint _width, const std::string& _label,
			   const DisplayStyle& _style,
			   const DisplayStyle& _activeStyle)throw() :
	ActiveWidget(_height, _width, _style, _activeStyle), label(_label)
{
	SetMinSize(1, label.length());
}

Button::Button(const std::string& _label,
			   const DisplayStyle& _style,
			   const DisplayStyle& _activeStyle)throw() :
	ActiveWidget(_style, _activeStyle), label(_label)
{
	SetMinSize(1, label.length());
}

void Button::OnRedraw(Screen& screen)throw()
{
		
	screen << (GetActive() ? GetActiveStyle() : GetStyle()) << Control::Clear
		   << Control::GotoYX(GetHeight()/2, (GetWidth() - label.length())/2);
	if (label.length()<=GetWidth())
		   screen << GetLabel();
	if (GetActive())
		screen.ForceCursorPosition(Position(0,0));
}

void Button::SetLabel(const std::string& _label)throw()
{
	label = _label;
}

std::string& Button::GetLabel()throw()
{
	return label;
}

Button::~Button()throw()
{
	;
}
