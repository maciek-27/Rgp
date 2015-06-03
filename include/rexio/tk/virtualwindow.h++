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

#ifndef __VIRTUALWINDOW_H__
#define __VIRTUALWINDOW_H__

#include <rexio/tk/window.h++>

namespace Scr
{
	namespace Tk
	{
		
		/*!
		  \param W class of inside's window.
		  Template for all framed windows.
		  FramedWindowBase is basically a window having a separate internal
		  window to which most of the calls(like AddWidget) are routed.
		*/
		template <class W>
		class VirtualWindow : public Window
		{
		protected:
			//! internal area, should have Window compatible interface.
			W inside;
		public:
			VirtualWindow(Uint _height,  Uint _width,
						  const DisplayStyle& _style = WINDOW_DEFAULT_STYLE,
						  const DisplayStyle& _inStyle =
						  WINDOW_DEFAULT_STYLE
						  )throw()
				: Window(_height, _width, _style), inside(_height, _width,
														  _inStyle)
				{
					Window::AddWidget(inside);
				}
			/*!
			  \param screen cut-down to actual content area

			  Similiar to OnRedraw with an exception of providing cut-down
			  screen.
			*/
			virtual void OnRedrawInside(Screen& screen)throw() {
				;
			}
			virtual void OnRedraw(Screen& screen)throw() {
				Window::OnRedraw(screen);
				OnRedrawInside(
					*screen.CreateSubScreen(inside.GetRow(), inside.GetCol(),
											inside.GetHeight(),
											inside.GetWidth()));
			}

			/*! \copydoc Window::AddWidget(Widget&)
			  \a VirtualWindow specific:
			  Passes the call to its internal window.
			*/
			virtual void AddWidget(Widget& widget)throw() {
				inside.AddWidget(widget);
				inside.SetPosition(0, 0);
			}
			/*! \copydoc Window::DelWidget(Widget&)
			  \a VirtualWindow specific:
			  Passes the call to its internal window.
			*/							
			virtual void DelWidget(Widget& widget)throw() {
				inside.DelWidget(widget);
			}
			/*! \copydoc Widget::OnResize()
			  \a VirtualWindow specific:
			  Has to be overloaded in deriving classes to handle
			  proper resizing of containing window.
			*/
			virtual void OnResize()throw() = 0;

			RTTI_OBJ(VirtualWindow, Window);
		}; // VirtualWindow
	} // Tk
} // Scr
#endif // __VIRTUALWINDOW_H__
