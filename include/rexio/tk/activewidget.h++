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

#ifndef __ACTIVEWIDGET_H__
#define __ACTIVEWIDGET_H__

#include <rexio/tk/widget.h++>
#include <rexio/tk/window.h++>

namespace Scr
{
	namespace Tk
	{
		const DisplayStyle ACTIVEWIDGET_DEFAULT_STYLE(WIDGET_DEFAULT_STYLE);
		const DisplayStyle ACTIVEWIDGET_DEFAULT_ACTIVESTYLE( 
			DisplayStyle(Fg::Black, Fg::Dark, Bg::White));

		//! Focus capable widget
		/*!
		  Focusable widget, useful for input fields and other form elements.		  
		*/
		class ActiveWidget:public Widget
		{
		protected:
			ActiveWidget(Uint _height,	
						 Uint _width,
						 const DisplayStyle& _style
						 = ACTIVEWIDGET_DEFAULT_STYLE,
						 const DisplayStyle& _activeStyle
						 = ACTIVEWIDGET_DEFAULT_ACTIVESTYLE)
				throw();
			ActiveWidget(const DisplayStyle& _style
						 = ACTIVEWIDGET_DEFAULT_STYLE,
						 const DisplayStyle& _activeStyle
						 = ACTIVEWIDGET_DEFAULT_ACTIVESTYLE)
				throw();

			virtual void SetStylesheet(Stylesheet* _styleSheet)throw() {
				Widget::SetStylesheet(_styleSheet);
				__FetchProperty(style, "style");
				__FetchProperty(activeStyle, "activeStyle");
			}

			~ActiveWidget()throw();

			DisplayStyle activeStyle;
			bool active;
		public:
			void OnFocus(FocusPolicy focustype)throw();
			void OnUnFocus(FocusPolicy focustype)throw();
			void OnKeyDown(Key key)throw();
			virtual void OnAction()throw();
			
			void SetActive(bool _active)throw();
			bool GetActive()throw();			

			DisplayStyle& GetActiveStyle()throw();
			void SetActiveStyle(const DisplayStyle& _activeStyle
								= DisplayStyle(Fg::White, Fg::Dark,
											   Bg::Black))throw();
			RTTI_OBJ(ActiveWidget, Widget);
		};
	}
}

#endif // __ACTIVEWIDGET_H__
