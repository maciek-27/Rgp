#include "activewidget.h++"

using namespace Scr;
using namespace Scr::Tk;

ActiveWidget::ActiveWidget(Uint _height, Uint _width,
						   const DisplayStyle& _style,
						   const DisplayStyle& _activeStyle)throw() :
	Widget(_height, _width, _style), activeStyle(_activeStyle), active(false)
{;}

ActiveWidget::ActiveWidget(const DisplayStyle& _style,
						   const DisplayStyle& _activeStyle)throw() :
	Widget(_style), activeStyle(_activeStyle), active(false)
{;}

void ActiveWidget::OnFocus(FocusPolicy focustype)throw()
{
	if(GetActive()) {
		GetParent().PassFocusRequest(focustype);
		return;
	}
	SetActive(true);
}

void ActiveWidget::OnUnFocus(FocusPolicy focustype)throw()
{
	SetActive(false);
}

void ActiveWidget::OnKeyDown(Key key)throw()
{
	if(key == Key::Enter)
		OnAction();
}

void ActiveWidget::OnAction()throw()
{
	RexIOLog(LogLevelModerate) << "ACTION?" << std::endl;
}

void ActiveWidget::SetActive(bool _active)throw()
{
	active = _active;
}

bool ActiveWidget::GetActive()throw()
{
	return active;
}

DisplayStyle& ActiveWidget::GetActiveStyle()throw()
{
	return activeStyle;
}

void ActiveWidget::SetActiveStyle(const DisplayStyle& _activeStyle)throw()
{
	activeStyle = _activeStyle;
}

ActiveWidget::~ActiveWidget()throw()
{
	;
}
