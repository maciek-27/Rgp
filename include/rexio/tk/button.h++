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

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <rexio/tk/activewidget.h++>


namespace Scr
{
	namespace Tk
	{ 		
		const DisplayStyle BUTTON_DEFAULT_STYLE(WIDGET_DEFAULT_STYLE);
		//! style for button, when it is focused 
		const DisplayStyle BUTTON_DEFAULT_ACTIVESTYLE(
			DisplayStyle(Scr::Fg::Black, Scr::Fg::Dark, Scr::Bg::White));
		class Button:public ActiveWidget
		{
		private:
			std::string label;
		public:
			Button(Uint _height,	
				   Uint _width,
				   const std::string& _label,
				   const DisplayStyle& _style = BUTTON_DEFAULT_STYLE,
				   const DisplayStyle& _activeStyle 
				   = BUTTON_DEFAULT_ACTIVESTYLE)
				throw();
			Button(const std::string& _label,
				   const DisplayStyle& _style = BUTTON_DEFAULT_STYLE,
				   const DisplayStyle& _activeStyle
				   = BUTTON_DEFAULT_ACTIVESTYLE)
				throw();
			~Button()throw();
			void OnRedraw(Screen& screen)throw();
			void SetLabel(const std::string& _label)throw();
			std::string& GetLabel()throw();

			RTTI_OBJ(Button, ActiveWidget);
		};
	}
}
#endif // __BUTTON_H__
