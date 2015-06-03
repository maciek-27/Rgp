#include <rexio/tk/selectbox.h++>
#include <rexio/tk/button.h++>

using namespace Scr;
using namespace Scr::Tk;


Selectbox::_SelectList::_SelectList(Uint _width, Uint _height, 
									const DisplayStyle& _style)throw() : 
	FramedWindow(_width, _height, _style),
	scroll(_height - 2), group(_height - 2, _width - 3, _style)
{
	// setup this _SelectList with scroll and group to fit entities (these are
	// basic building blocks of internal _SelectList of Selectbox.
	AddWidget(scroll);
	AddWidget(group);
	// to trigger further setup
	OnResize();
}		
void Selectbox::_SelectList::OnResize()throw()
{
	FramedWindow::OnResize();
	group.SetSize(Size(GetHeight() - 2, GetWidth()-3));

	group.SetPosition(0, 0);

	scroll.SetPosition(0, GetWidth() - 3);
	scroll.SetHeight(GetHeight() - 2);
}
void Selectbox::_SelectList::CloseSelectList()
{
	// _SelectList may be shown or hidden while whole Selectbox is active,
	// however just deleting object without passing focus to the next one
	// may cause SIGSEGV
	try {
		GetParent().PassFocusRequest(AllFocus);
		GetParent().SetActiveWidget(*prevActive);
		GetParent().DelWidget(*this);
	}
	catch(...){}
}
void Selectbox::_SelectList::OnKeyDown(Key key)throw()
{
	if (key==Key::Up)
	{
		if (group.activeWidget!=group.elements.begin())
		{
			(*group.activeWidget)->OnUnFocus(AllFocus);			
			--group.activeWidget;
			(*group.activeWidget)->OnFocus(AllFocus);
		}
	}
	else if (key==Key::Down)
	{
		WidgetList::iterator tmp=group.activeWidget;
		tmp++;
		if (tmp!=group.elements.end())// past-the-end really
		{
			// mofify focus only if "next element" really exists
			(*group.activeWidget)->OnUnFocus(AllFocus);			
			++group.activeWidget;
			(*group.activeWidget)->OnFocus(AllFocus);
		}
	}
	else if (key==Key::Enter)
	{
		CloseSelectList();
		// just close the list - selection is to be saved elsewhere
		// and only if deliberately selected (...::Detail::Selector::OnAction)
	}
	Window::OnKeyDown(key);
}				

void Selectbox::_SelectList::OnFocus(FocusPolicy focustype)throw()
{
	if (group.activeWidget==group.elements.end())
		group.activeWidget=group.elements.begin();
	if (group.activeWidget!=group.elements.end())
		(*group.activeWidget)->OnFocus(AllFocus);
}
void Selectbox::_SelectList::OnUnFocus(FocusPolicy focustype)throw()
{
}

Selectbox::_SelectList::~_SelectList()throw()
{;}


Selectbox::Selectbox(Uint _width,
					 const DisplayStyle& _style,
					 const DisplayStyle& _activeStyle,
					 const SelectboxStyle& _selectBoxStyle)throw()
	: ActiveWidget(1, _width, _style, _activeStyle),
	  selectboxStyle(_selectBoxStyle), selectList(4, _width, _style)
{
	SetMaxHeight(1);
	SetMinHeight(1);
}

Selectbox::Selectbox(const DisplayStyle& _style,
					 const DisplayStyle& _activeStyle,
					 const SelectboxStyle& _selectBoxStyle)throw()
	: ActiveWidget(_style, _activeStyle),
	  selectboxStyle(_selectBoxStyle), selectList(4, 10, _style)
{
	SetMaxHeight(1);
	SetMinHeight(1);
}

namespace Scr{namespace Tk{namespace Detail
{
// Selector is part of _SelectList group , that makes Selectbox running.
class Selector:public Button
{
private:
	Selectbox & SB;
public:
	
	Selector(Selectbox & _SB,
			 const std::string& _label,
			 const DisplayStyle& _style = BUTTON_DEFAULT_STYLE,
			 const DisplayStyle& _activeStyle
			 = BUTTON_DEFAULT_ACTIVESTYLE)
		:Button(_label, _style,
				_activeStyle), SB(_SB) {}
	virtual void OnAction()throw()
		{
			try
			{
				GetParent().GetRootWindow().DelWidget(SB.selectList);
				SB.opened=false;
			}catch(...){}
		}
};}}}

Uint Selectbox::AddOption(const std::string& name)throw()
{
	selectList.group.AddWidget(*(new Detail::Selector(*this,name)));
	return 0;
}

const std::string& Selectbox::GetOption() const throw(NoSuchOption)
{
	try
	{
		const std::string& l =
		dynamic_cast<Button&>
			(selectList.group.GetActiveWidget()).GetLabel();
		return l;
	}
	catch(Window::WidgetNotPresent)
	{
		THROW(NoSuchOption);
	}
}

void Selectbox::OnAction()throw()
{
	if(!opened) {
		opened=true;
		GetParent().GetRootWindow().AddWidget(selectList);
		selectList.SetWidth(GetWidth());
		selectList.SetPosition(
			GetParent().GetAbsoluteRow() + GetRow(), 
			GetParent().GetAbsoluteColumn() + GetCol()
			);
		selectList.prevActive = 
			&(GetParent().GetRootWindow().GetActiveWidget());
		GetParent().GetRootWindow().SetActiveWidget(selectList);
	}
}

void Selectbox::OnRedraw(Screen& screen)throw()
{
	screen << Control::GotoYX(0, 0) <<
		(GetActive()?GetActiveStyle():GetStyle()) <<
		Control::Clear;

	try
	{
		// print out currently active option
		screen << GetOption();
	}
	catch (...) {}
	screen << Control::GotoYX(0, GetWidth() - 2) <<
		selectboxStyle.openStyle << selectboxStyle.openButton;
}

void Selectbox::OnFocus(FocusPolicy focusPolicy)throw()
{
	try
	{
		GetParent().GetRootWindow().DelWidget(selectList);
		opened=false;
	}catch(...){}
	ActiveWidget::OnFocus(focusPolicy);
}

void Selectbox::OnUnFocus(FocusPolicy focusPolicy)throw()
{
	ActiveWidget::OnUnFocus(focusPolicy);
}
