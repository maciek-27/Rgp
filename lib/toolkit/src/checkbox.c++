#include "checkbox.h++"

using namespace Scr;
using namespace Scr::Tk;

Checkbox::Checkbox(Uint _height, Uint _width, const Label& _label,
				   const DisplayStyle& _style,
				   const DisplayStyle& _activeStyle)throw() :
	ActiveWidget(_height, _width, _style, _activeStyle), label(_label)
{
	SetMinSize(1, label.GetMinWidth() + 4); // +4 because of the actual checkbox
}

Checkbox::Checkbox(const Label& _label,
				   const DisplayStyle& _style,
				   const DisplayStyle& _activeStyle)throw() :
	ActiveWidget(_style, _activeStyle), label(_label)
{
	SetMinSize(1, label.GetMinWidth() + 4);
}

void Checkbox::OnRedraw(Screen& screen)throw()
{
	screen << Control::GotoYX(0,0)
	       << (GetActive() ? GetActiveStyle() : GetStyle()) << '['
	       << (GetState() ? 'X':' ') << ']'
	       << GetParent().GetStyle() << ' ' << label.GetStyle() << label;

	if (GetActive())
		screen.ForceCursorPosition(Position(0,3));
}

void Checkbox::OnAction()throw()
{
	SetState(!GetState());	
}

void Checkbox::SetLabel(const Label& _label)throw()
{
	label = _label;
}

const Label& Checkbox::GetLabel()throw()
{
	return label;
}

void Checkbox::SetState(bool _state)throw()
{
	try {
		if(state != _state)
			GetParent().RedrawRequest(*this);
	} catch(ParentNotDefined) {
		;
	}
	state = _state;
}

bool Checkbox::GetState()throw()
{
	return state;
}

Checkbox::~Checkbox()throw()
{
	;
}
