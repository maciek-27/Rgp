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

#ifndef __WINDOW_H__
#define __WINDOW_H__

// TODO: Honor IsHidden() for widgets focus passing.

#include <list>
#include <rexio/tk/widget.h++>
#include <rexio/tk/autolist.h++>

#ifdef __GNUC__
#define HASH_MAP_NAMESPACE __gnu_cxx
#else
#define HASH_MAP_NAMESPACE std
#endif

namespace Scr
{
	namespace Tk
	{		
		class RootWindow;

		const DisplayStyle WINDOW_DEFAULT_STYLE(WIDGET_DEFAULT_STYLE);

		/*!
		  Window, a buffered ancestor of \a Widget. It can also group
		  other widgets and pass all the events down the path.
		  \sa \a WidgetGroup for an automated Widget grouping solution.
		*/
		class Window : public Widget
		{
		protected:
			/*!
			  Focuses on a next contained element that has a proper
			  \a focusPolicy.
			  Specifically, \a activeWidget iterator is incremented.
			*/
			void NextWidget(); // activeWidget ++

			/*!
			  Widget dedicated container.
			*/
			typedef AutoList<Widget*> WidgetList;

			/*!
			  Represensts all contained widgets, including subwindows.
			*/
			WidgetList elements;
			/*!
			  Currently active widget.	
			*/
			WidgetList::iterator activeWidget;

			/*!
			  \return Screen handler reference.
			  
			  Returns the top-level Screen handler.

			  \exception ParentNotDefined is thrown had the window
			  not been attached to any other.
			*/
			virtual Screen& GetScreen()throw(ParentNotDefined);

		public:
		  int GetNumElements() { return elements.size();}
			__DE(WidgetAlreadyAdded, Exception);
			__DE(WidgetNotPresent, Exception);
		  Widget * GetFirstElement() {
		    if (elements.empty())
		      THROW(WidgetNotPresent);
		    return *elements.begin();}

			/*!
			  Returns an absolute column the window is positioned
			  on a RootWindow

			  \exception ParentNotDefined is thrown had the window
			  not been attached to any other.
			*/
			virtual Uint GetAbsoluteColumn()throw(ParentNotDefined);
			/*!
			  Returns an absolute row the window is positioned
			  on a RootWindow
			  
			  \exception ParentNotDefined is thrown had the window
			  not been attached to any other.
			*/
			virtual Uint GetAbsoluteRow()throw(ParentNotDefined);

			/*!
			  \param _height desired height
			  \param _width desired width
			  \param _style optional style
			*/
			Window(Uint _height,	
				   Uint _width,
				   const DisplayStyle& _style
				   = DisplayStyle(Fg::White,Fg::Dark,Bg::Black))throw();
			/*!
			  \copydoc Widget::SetStylesheet(Stylesheet*)
			  \a Window specific:
			  Recursively passes this call to all its children.
			*/
			virtual void SetStylesheet(Stylesheet* _styleSheet)throw();

			/*!
			  \param widget widget to attach to this window

			  Attach a widget to this window.
			  Specifically, add it to the \a elements.
			  
			  \exception ParentAlreadySet is thrown if the widget
			  has already been attached to some other window.
			  \exception WidgetAlreadyAdded if the widget
			  is already attached to THIS window.
			*/
			virtual void AddWidget(Widget& widget)
				throw(ParentAlreadySet, WidgetAlreadyAdded);
			/*!
			  \param widget widget to detach from this window

			  Detach a widget from this window.
			  Specifically, del it from the \a elements.

			  \exception WidgetNotPresent is thrown if the widget
			  is not attached to this window.
			*/
			virtual void DelWidget(Widget& widget)throw(WidgetNotPresent);
			
			/*!
			  \return RootWindow
			  
			  \exception ParentNotDefined is thrown if the window hasn't
			  been attached to any other and thus is not in relation
			  with the root one.
			*/
			virtual RootWindow& GetRootWindow()
				throw(ParentNotDefined);
			

			/*!
			  Need to redraw, pass the \a OnRedraw() event to all
			  contained widgets.
			*/
			virtual void RedrawRequest()throw();
			/*!
			  \param widget reference to widget which needs redrawing

			  Redraw one specific widget. Pass the \a OnRedraw() event
			  to it.
			*/
			virtual void RedrawRequest(Widget& widget)throw();

			
			virtual void OnFocus(FocusPolicy focustype)throw();
			virtual void OnUnFocus(FocusPolicy focustype)throw();
			/*!
			  \param focustype focus policy of this event
			  
			  This event is triggered when containing event does want
			  to revoke its focus.
			*/
			virtual void PassFocusRequest(FocusPolicy focustype)throw();

			/*!
			  \param w widget to activate
			  
			  Activates a given widget. Widget has to be directly
			  contained by this window.

			  \note Widget might directly revoke its activity.

			  \exception WidgetNotPresent is thrown if the widget
			  is not attached to this window.
			*/
			virtual void SetActiveWidget(Widget &w)
				throw(WidgetNotPresent);

			
			/*!
			  \return reference to current active widget

			  \exception WidgetNotPresent is thrown if no widget is currently
			  active.
			*/
			virtual Widget& GetActiveWidget()const throw(WidgetNotPresent);
			
			virtual void OnStart()throw();
			virtual void OnResize()throw();
			virtual void OnRedraw(Screen& screen)throw();
			virtual void OnKeyDown(Key key)throw();

			/*!
			  \param _size new size

			  Set size of the Window. Invoke \a OnResize() event afterwards.
			  \note If entered size is bigger than \a GetMaxSize()
			  or smaller than \a GetMinSize(), it will crop the entered
			  value to the boundaries.
			  \note Since all the other size functions depend on this one,
			  all of them get the \a OnResize() event for free.
			*/
			virtual void SetSize(const Size& _size)throw();
		  ~Window() throw() {}
			RTTI_OBJ(Window, Widget);
		}; // Window
	} // Tk
} // Scr
#endif // __WINDOW_H__

