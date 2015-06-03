//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2008 Damian Kaczmarek, Maciej Kaminski
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////

#ifndef __WIDGET_H__
#define __WIDGET_H__
#include <rexio/screen.h++>
#include <rexio/tk/stylesheet.h++>

// #ifdef __GNUC__
// #define HASH_MAP_NAMESPACE __gnu_cxx
// #include <ext/hash_map>
// #else
// #include <hash_map>
// #define HASH_MAP_NAMESPACE std
// #endif

#include <rexio/tk/rtti.h++>

#define __FetchProperty(saveto, name) \
try { \
	(saveto) = static_cast<__typeof__(saveto)>( \
_styleSheet->GetProperty(*this, name)); \
} \
catch(Stylesheet::Property::WrongPropertyConversion) { \
; \
} \
catch(Stylesheet::Properties::NoSuchProperty) { \
; \
}

#define __FetchPropertyDefault(saveto, name, default) \
try { \
	(saveto) = static_cast<__typeof__(saveto)>(\
_styleSheet->GetProperty(*this, name)); \
} \
catch(Stylesheet::Property::WrongPropertyConversion) { \
	(saveto) = (default); \
} \
catch(Stylesheet::Properties::NoSuchProperty) { \
	(saveto) = (default); \
}

namespace Scr
{	
	namespace Tk
	{
		const DisplayStyle WIDGET_DEFAULT_STYLE(Fg::System, Fg::Dark,
												Bg::System);

		class Window;
		//! Base UI element.
		/*!
		  Widget - according to the dictionary, a device that is very useful
		  for a particular job. In our case that can be any UI job and thus
		  all the UI elements shall thereby be children of hit.
		  Note that most widgets do not have their own buffer.
		*/
		class Widget
		{
		private:
			friend class Window; //FIXME
			/*!
			  All widgets have a pointer to their parent window.
			  For \a RootWindow, it is a pointer to itself
			  \note For not assigned widgets it is NULL.
			*/
			Window* parentWindow;
			/*!
			  Pointer to the stylesheet. If NULL, the widget's properties
			  should be left default.
			*/
			Stylesheet *styleSheet;
		protected:		   
			/*!
			  This constructor should be used for widgets manually
			  positioned. Widgets managed by \a WidgetGroup should be
			  constructed with a more simple constructor.
			  \param _height desired height
			  \param _width desired width
			  \param _style optional style
			*/
			Widget(Uint _height,
				   Uint _width,
				   const DisplayStyle& _style
				   = WIDGET_DEFAULT_STYLE)throw();
			/*!
			  This constructor should be a preferred one if geometry
			  and position of a Widget are to be managed by some
			  \a WidgetGroup.
			  \param _style optional style
			*/
			Widget(const DisplayStyle& _style
				   = WIDGET_DEFAULT_STYLE)throw();
			
			__DE(ParentNotDefined, Exception);
			__DE(ParentAlreadySet, Exception);

			/*!
			  \param window parent of this widget
			
			  Parent of a widget can be set generally only once.
			  After doing this, widget is ready to face the world
			  so better prepare it properly first.
			  This design decision has been made because of the constructor's
			  primitive nature not being able to sustain all the
			  possibilities.

			  \sa \a ReParent

			  \exception ParentAlreadySet is thrown had the parent already
			  been set.
			*/
			void SetParent(Window& window)throw(ParentAlreadySet);
			/*!
			  \return reference to parent window
			  
			  Get reference to parent window.

			  \exception ParentNotSet is thrown if the parent window has not
			  been yet specified.
			*/
			Window& GetParent()throw(ParentNotDefined);

			/*!
			  \param window pointer to parent of this widget, pass NULL
			  after detaching the widget from window.
			  
			  Provided for convenience. Sets the parent disregarding any
			  conditions.

			  \sa \a SetParent for general use.
			*/
			void ReParent(Window* window)throw();
			

			/*!
			  Focus policy defines a condition upon a widget
			  can be focused.
			*/
			typedef enum {
				//! Nothing can focus.
				NoFocus = 0x1,
				//! Tabulator(or other switching key) can focus.
				TabFocus = 0x1,
				//! Mouse click can focus.
				ClickFocus = 0x2,

				WheelFocusUp = 0x4,
				WheelFocusDown = 0x8,

				//! Mouse wheel can focus.
				WheelFocus = WheelFocusUp|WheelFocusDown,
				//! TabFocus + Clickfocus.
				StrongFocus = TabFocus|ClickFocus,
				//! Full service focus. :-)
				AllFocus = TabFocus|ClickFocus|WheelFocus
			} FocusPolicy;
			/*
			  Current focus policy.
			*/
			FocusPolicy focusPolicy;

			/*!
			  Position regarding the \a parentWindow.
			  i.e. (position.row == 3) means that row 3 of \a parentWindow
			  is 0th row of this widget.
			*/
			Position position;
			
			/*!
			  Current size.
			*/
			Size size;
			/*!
			  Maximal size that the Widget can be expanded to
			  by for example a \a WidgetGroup.
			*/
			Size sizeMax;
			/*!
			  Minimal size that the Widget can be shrinked to
			  by for example a \a WidgetGroup.
			*/
			Size sizeMin;
			
			/*!
			  Basic style.
			*/
			DisplayStyle style;

			/*!
			  Implies whether the element is hidden.
			  /note When hidden, the element want be a subject
			  into positioning algorithms and its OnRedraw event
			  won't invoked.
			*/
			bool hidden;
			

		public:

			// most of the widgets do not have their own buffers -
			// this is a sane design decision, not a bug ;-)

			/*!
			  \param _styleSheet pointer to style data

			  Apply Stylesheet to this widget. Reinitialize any style
			  properties if their alternatives are supplied.
			*/
			virtual void SetStylesheet(Stylesheet* _styleSheet)throw();

			/*!
			  \param focustype Type of the event, i.e. mouse click.

			  Element focused. Only matters if a proper \a focusPolicy
			  is set.
			*/
			virtual void OnFocus(FocusPolicy focustype)throw();
			/*!
			  \param focustype Type of the event, i.e. mouse click.

			  Element unfocused. Only matters if a proper \a focusPolicy
			  is set.
			*/
			virtual void OnUnFocus(FocusPolicy focustype)throw();
			/*
			  First event after the constructor call.
			*/
			virtual void OnStart()throw();

			/*!
			  \param screen reference to the screen on which to draw
			  
			  This is the main thing, the core of the Widget.
			  Upon this event, the whole content should be redrawn.

			  \note the screen parameter is not a real screen,
			  it is a cutdown to our size screen or even some other
			  overloaded screen flavour.
			*/
			virtual void OnRedraw(Screen& screen)throw();

			/*!
			  If the widget is attached to a window, it invokes
			  parent's RedrawRequest with this widget.
			  If it isn't attached, the function does nothing.

			  \sa Window::RedrawRequest(Widget &w)
			*/
			virtual void RedrawRequest()throw();

			/*!
			  Resize event. Do something i.e. adjust content to the
			  new size.
			*/
			virtual void OnResize()throw();			
			/*!
			  \param key keycode

			  Keyboard button press event.
			*/
			virtual void OnKeyDown(Key key)throw();

			/*!
			  Last event BEFORE the destructor call.
			*/
			virtual void OnExit()throw();

			/*!
			  \param _pos position new position

			  Set position of the Widget regarding to the
			  \a parentWindow.

			  \exception ParentNotDefined is thrown had the widget not been
			  assigned to any window. Use \a AddWidget.
			*/
			virtual void SetPosition(const Position& _pos)
				throw(ParentNotDefined);
			/*!
			  \param _row new row position
			  \param _col new column position

			  Set position of the Widget regarding to the
			  \a parentWindow.

			  \exception ParentNotDefined is thrown had the widget not been
			  assigned to any window. Use \a AddWidget.
			*/
			virtual void SetPosition(Uint _row, Uint _col)
				throw(ParentNotDefined);
			/*!
			  \return position
			
			  Get position of the Widget regarding to the
			  \a parentWindow.

			  \exception ParentNotDefined is thrown had the widget not been
			  assigned to any window. Use \a AddWidget.
			*/
			virtual Position GetPosition() const throw(ParentNotDefined);
			/*!
			  \param _row new row position

			  Set position of the Widget regarding to the
			  \a parentWindow .

			  \exception ParentNotDefined is thrown had the widget not been
			  assigned to any window. Use \a AddWidget.
			*/
			virtual void SetRow(Uint _row)throw(ParentNotDefined);
			/*!
			  \return row position
			
			  Get position of the Widget regarding to the
			  \a parentWindow.

			  \exception ParentNotDefined is thrown had the widget not been
			  assigned to any window. Use \a AddWidget.
			*/
			virtual Uint GetRow() const throw(ParentNotDefined);
			/*!
			  \param _col new column position

			  Set position of the Widget regarding to the
			  \a parentWindow.

			  \exception ParentNotDefined is thrown had the widget not been
			  assigned to any window. Use \a AddWidget.
			*/
			virtual void SetCol(Uint _col)throw(ParentNotDefined);
			/*!
			  \return col position
			
			  Get position of the Widget regarding to the
			  \a parentWindow.

			  \exception ParentNotDefined is thrown had the widget not been
			  assigned to any window. Use \a AddWidget.
			*/
			virtual Uint GetCol() const throw(ParentNotDefined);

			/*!
			  \param _size new size

			  Set size of the Widget.
			  \note If entered size is bigger than \a GetMaxSize()
			  or smaller than \a GetMinSize(), it will crop the entered
			  value to the boundaries.
			*/
			virtual void SetSize(const Size& _size)throw();
			/*!
			  \param _height new height
			  \param _width new width

			  Set size of the Widget.
			  \note If entered size is bigger than \a GetMaxSize()
			  or smaller than \a GetMinSize(), it will crop the entered
			  value to the boundaries.
			*/
			virtual void SetSize(Uint _height, Uint _width)throw();
			/*!
			  \return size

			  Get size of the Widget.
			*/
			virtual const Size& GetSize() const throw();
			/*!
			  \param _height new height

			  Set height of the Widget.
			  \note If entered size is bigger than \a GetMaxSize()
			  or smaller than \a GetMinSize(), it will crop the entered
			  value to the boundaries.
			*/
			virtual void SetHeight(Uint _height)throw();
			/*!
			  \return height

			  Get height of the Widget.
			*/
			virtual Uint GetHeight() const throw();
			/*!
			  \param _width new width

			  Set width of the Widget.
			  \note If entered size is bigger than \a GetMaxSize()
			  or smaller than \a GetMinSize(), it will crop the entered
			  value to the boundaries.
			*/
			virtual void SetWidth(Uint _width)throw();
			/*!
			  \return width

			  Get width of the Widget.
			*/
			virtual Uint GetWidth() const throw();

			/*!
			  \param _pos position new position
			  \param _size new size

			  Set both position and size of the Widget regarding to the
			  \a parentWindow.
			  \note If entered size is bigger than \a GetMaxSize()
			  or smaller than \a GetMinSize(), it will crop the entered
			  value to the boundaries.

			  \exception ParentNotDefined is thrown had the widget not been
			  assigned to any window. Use \a AddWidget.
			*/
			virtual void SetGeometry(const Position& _pos, const Size& _size)
				throw(ParentNotDefined);
			/*!
			  \param _row new row position
			  \param _col new column position
			  \param _height new height
			  \param _width new width

			  Set both position and size of the Widget regarding to the
			  \a parentWindow.
			  \note If entered size is bigger than \a GetMaxSize()
			  or smaller than \a GetMinSize(), it will crop the entered
			  value to the boundaries.

			  \exception ParentNotDefined is thrown had the widget not been
			  assigned to any window. Use \a AddWidget.
			*/
			virtual void SetGeometry(Uint _row, Uint _col,
									 Uint _height, Uint _width)
				throw(ParentNotDefined);

			/*!
			  \param _size new minimal size

			  Set minimal size of the Widget, \a minSize property.
			  \note If size is bigger than \a GetMaxSize(), it will
			  crop the entered value to the boundary.
			*/
			virtual void SetMinSize(const Size& _size)throw();
			/*!
			  \param _height new minimal height
			  \param _width new minimal width

			  Set minimal size of the Widget, \a minSize property.
			  \note If size is bigger than \a GetMaxSize(), it will
			  crop the entered value to the boundary.
			*/
			virtual void SetMinSize(Uint _height, Uint _width)throw();
			/*!
			  \return minimal size

			  Get minimal size of the Widget.
			*/
			virtual const Size& GetMinSize() const throw();
			/*!
			  \param _height new minimal height

			  Set minimal height of the Widget, \a minSize property.
			  \note If size is bigger than \a GetMaxSize(), it will
			  crop the entered value to the boundary.
			*/
			virtual void SetMinHeight(Uint _height)throw();
			/*!
			  \return minimal height

			  Get minimal height of the Widget.
			*/
			virtual Uint GetMinHeight() const throw();
			/*!
			  \param _width new minimal width

			  Set minimal width of the Widget, \a minSize property.
			  \note If size is bigger than \a GetMaxSize(), it will
			  crop the entered value to the boundary.
			*/
			virtual void SetMinWidth(Uint _width)throw();
			/*!
			  \return minimal width

			  Get minimal width of the Widget.
			*/
			virtual Uint GetMinWidth() const throw();

			/*!
			  \param _size new maximal size

			  Set maximal size of the Widget, \a minSize property.
			  \note If size is smaller than \a GetMinSize(), it will
			  crop the entered value to the boundary.
			*/
			virtual void SetMaxSize(const Size& _size)throw();
			/*!
			  \param _height new maximal height
			  \param _width new maximal width

			  Set maximal size of the Widget, \a minSize property.
			  \note If size is smaller than \a GetMinSize(), it will
			  crop the entered value to the boundary.
			*/
			virtual void SetMaxSize(Uint _height, Uint _width)throw();
			/*!
			  \return maximal size

			  Get maximal size of the Widget.
			*/
			virtual const Size& GetMaxSize() const throw();
			/*!
			  \param _height new maximal height

			  Set maximal height of the Widget, \a minSize property.
			  \note If size is smaller than \a GetMinSize(), it will
			  crop the entered value to the boundary.
			*/
			virtual void SetMaxHeight(Uint _height)throw();
			/*!
			  \return maximal height

			  Get maximal height of the Widget.
			*/
			virtual Uint GetMaxHeight() const throw();
			/*!
			  \param _width new maximal width

			  Set maximal width of the Widget, \a minSize property.
			  \note If size is smaller than \a GetMinSize(), it will
			  crop the entered value to the boundary.
			*/
			virtual void SetMaxWidth(Uint _width)throw();
			/*!
			  \return maximal width

			  Get maximal width of the Widget.
			*/
			virtual Uint GetMaxWidth() const throw();

			
			/*!
			  \param _policy new focus policy

			  Set focus policy.
			*/
			virtual void SetFocusPolicy(FocusPolicy _policy)throw();

			/*!
			  \return current focus policy

			  Get current focus policy.
			*/
			virtual FocusPolicy GetFocusPolicy() const throw();

			/*!
			  \param style

			  Set style.
			*/
			virtual void SetStyle(const DisplayStyle& style
								  = DisplayStyle(Fg::System,Fg::Dark,
												 Bg::System))throw();

			/*!
			  \return current style

			  Get style.
			*/
			virtual const DisplayStyle& GetStyle() const throw();

			/*!
			  \param _hidden new state value

			  Set the hidden state.
			*/
			void SetHidden(bool _hidden)throw();
			/*!
			  \return current hidden state			  
			*/
			bool IsHidden() const throw();

			virtual ~Widget()throw();	
			
			//! Object name. Used for style targetting.
			std::string objectName;

			RTTI_BASE(Widget);
			/*! \typedef std::vector<std::string> ClassHierarchy;
			  Container holding the list of class names.
			*/
			/*! \fn bool Widget::IsTypeOf(std::string _className) const
			  \param _className name of a class
			  \return whether the _className is in class hierarchy of this' class.
			*/
			/*! \fn const char * Widget::TypeName() const
			  \return class name of this widget.
			*/
			/*! \fn const char * Widget::ParentName() const
			  \return parent class of this widget.
			*/			
			/*! \fn const Widget::ClassHierarchy& Hierarchy()
			  \return class hierarchy of this widget.
			*/
		}; // Widget				
	} // namespace Tk
} // namespace Scr

#include "stylesheet.h++"

#endif // __WIDGET_H__
