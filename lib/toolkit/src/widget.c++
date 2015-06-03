#include <rexio/screen.h++>

#include <rexio/tk/widget.h++>
#include <rexio/tk/window.h++>
#include <rexio/throw.h++>

using namespace Scr;
using namespace Scr::Tk;

Widget::Widget(Uint _height,	
			   Uint _width,
			   const DisplayStyle & _style)throw()
    :parentWindow(NULL),
	 styleSheet(NULL),
	 position(0, 0),
	 size(_height, _width),
	 sizeMax(UintMax, UintMax),
	 sizeMin(0, 0),
	 style(_style),
	 hidden(false) {;}

Widget::Widget( const DisplayStyle & _style)throw()
    :parentWindow(NULL),
	 styleSheet(NULL),
	 position(0, 0),
	 size(0, 0),
	 sizeMax(UintMax, UintMax),
	 sizeMin(0, 0),
	 style(_style),
	 hidden(false) {;}

void Widget::SetParent(Window& window)throw(ParentAlreadySet)
{
	if(parentWindow)
		THROW(ParentAlreadySet);
	parentWindow = &window;
}

Window& Widget::GetParent()throw(ParentNotDefined)
{
	if(!(this->parentWindow))
		THROW(ParentNotDefined);
	return *parentWindow;
}

void Widget::ReParent(Window *window)throw()
{
	parentWindow = window;
}

void Widget::SetStylesheet(Stylesheet* _styleSheet)throw()
{
	styleSheet = _styleSheet;
	__FetchProperty(style, "style");
//FIXME when changed to Bg::Black, weird things happen
}

void Widget::OnFocus(FocusPolicy focustype)throw()
{
	// default behaviour is allowing focus?
	parentWindow->PassFocusRequest(focustype);
}//element unfocused

void Widget::OnUnFocus(FocusPolicy focustype)throw(){;}//element focused
void Widget::OnStart()throw() {;}
void Widget::OnRedraw(Screen&screen)throw() {;}
void Widget::RedrawRequest()throw()
{
	try 
	{
		GetParent().RedrawRequest(*this);
	} 
	catch(ParentNotDefined) 
	{
		;
	}
}

void Widget::OnResize()throw() {;}
void Widget::OnKeyDown(Key key)throw() {
	if(key.IsASpecialKey()) {
		if(key == Key::Tab) {
			GetParent().PassFocusRequest(TabFocus);
		}

	}
}

void Widget::OnExit()throw() {;}

void Widget::SetPosition(const Position& _pos)throw(ParentNotDefined)
{
	GetParent(); // throws exception
	position = _pos;
}

void Widget::SetPosition(Uint _row, Uint _col)throw(ParentNotDefined)
{
	SetPosition(Position(_row, _col));
}

Position Widget::GetPosition() const throw(ParentNotDefined)
{
	return position;
}

void Widget::SetRow(Uint _row)throw(ParentNotDefined)
{
	SetPosition(_row, position.col);
}

Uint Widget::GetRow() const throw(ParentNotDefined)
{
	return position.row;
}

void Widget::SetCol(Uint _col)throw(ParentNotDefined)
{
	SetPosition(position.row, _col);
}

Uint Widget::GetCol() const throw(ParentNotDefined)
{
	return position.col;
}

void Widget::SetSize(const Size& _size)throw()
{
	size = _size;
	OnResize();
}

void Widget::SetSize(Uint _height, Uint _width)throw()
{
	SetSize(Size(_height, _width));
}

const Size& Widget::GetSize() const throw()
{
	return size;
}

void Widget::SetHeight(Uint _height)throw()
{
	SetSize(_height, size.width);
}

Uint Widget::GetHeight() const throw()
{
	return size.height;
}

void Widget::SetWidth(Uint _width)throw()
{
	SetSize(size.height, _width);
}

Uint Widget::GetWidth() const throw()
{
	return size.width;
}

void Widget::SetGeometry(const Position& _pos, const Size& _size)
	throw(ParentNotDefined)
{
	SetPosition(_pos);
	SetSize(_size);
}

void Widget::SetGeometry(Uint _row, Uint _col,
						 Uint _height, Uint _width)throw(ParentNotDefined)
{
	SetGeometry(Position(_row, _col), Size(_height, _width));
}

void Widget::SetMinSize(const Size& _size)throw()
{
	sizeMin = _size;
}

void Widget::SetMinSize(Uint _height, Uint _width)throw()
{
	SetMinSize(Size(_height, _width));
}

const Size& Widget::GetMinSize() const throw()
{
	return sizeMin;
}

void Widget::SetMinHeight(Uint _height)throw()
{
	SetMinSize(_height, sizeMin.width);
}

Uint Widget::GetMinHeight() const throw()
{
	return sizeMin.height;
}

void Widget::SetMinWidth(Uint _width)throw()
{
	SetMinSize(sizeMin.height, _width);
}

Uint Widget::GetMinWidth() const throw()
{
	return sizeMin.width;
}

void Widget::SetMaxSize(const Size& _size)throw()
{
	sizeMax = _size;
}

void Widget::SetMaxSize(Uint _height, Uint _width)throw()
{
	SetMaxSize(Size(_height, _width));
}

const Size& Widget::GetMaxSize() const throw()
{
	return sizeMax;
}

void Widget::SetMaxHeight(Uint _height)throw()
{
	SetMaxSize(_height, sizeMax.width);
}

Uint Widget::GetMaxHeight() const throw()
{
	return sizeMax.height;
}

void Widget::SetMaxWidth(Uint _width)throw()
{
	SetMaxSize(sizeMax.height, _width);
}

Uint Widget::GetMaxWidth() const throw()
{
	return sizeMax.width;
}

void Widget::SetFocusPolicy(FocusPolicy _policy)throw()
{
	focusPolicy = _policy;
}

Widget::FocusPolicy Widget::GetFocusPolicy() const throw()
{
	return focusPolicy;
}

void Widget::SetStyle(const DisplayStyle& _style)throw()
{
	style = _style;
}

const DisplayStyle& Widget::GetStyle() const throw()
{
	return style;
}

void Widget::SetHidden(bool _hidden)throw()
{
	hidden = _hidden;
	RedrawRequest();
}

bool Widget::IsHidden() const throw()
{
	return hidden;
}

Widget::~Widget()throw() {;}
