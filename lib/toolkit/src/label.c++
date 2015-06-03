//#include "toolkit.h++"
#include "label.h++"

using namespace Scr;
using namespace Scr::Tk;

Label::Label(const DisplayStyle& _style)throw()
    :Widget( _style)
{
    label = "";
	SetMaxHeight(1);
	SetMinHeight(1);
}

Label::Label(Uint _width, const std::string& _label,
	     const DisplayStyle& _style)throw()
    :Widget(1, _width, _style)
{
    label = _label;
	SetMaxHeight(1);
	SetMinHeight(1);
}

Label::Label(const std::string& _label,
			 const DisplayStyle& _style)throw()
    :Widget(1, _label.length(), _style)
{
    label = _label;
	SetMaxHeight(1);
	SetMinHeight(1);
}

void Label::OnFocus(FocusPolicy focustype)throw()
{
    GetParent().PassFocusRequest(focustype); // focus another element
}

void Label::OnUnFocus(FocusPolicy focustype)throw()
{
	;
}

void Label::OnRedraw(Screen& screen)throw()
{
    screen <<((style.GetFgColor() == Fg::Transparent)?GetParent().GetStyle():
		style) << Control::Clear << Control::GotoYX(0,0);	
	try
	{
		screen << label;
	}
	catch (Scr::Screen::PrintOutOfRange)
	{
	  try
	    {
	      screen << label.substr(0,screen.GetWidth()-3) << "...";
	    }
	  catch (Scr::Screen::PrintOutOfRange)
	    {
	      for (Uint i = 0 ; i< screen.GetWidth() ; i++ )
		screen << '.';
	    }
	}
}

const std::string& Label::GetText() const throw()
{
	return label;
}

void Label::SetText(std::string _label)throw()
{
	SetMinWidth(label.length());
	label = _label;
}

Screen& Scr::operator<<(Screen & screen,const Tk::Label& whatto)
{
	return (screen << whatto.GetStyle() << whatto.GetText());
}

Label::~Label()throw() {;}
