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

#ifndef __CHECKBOX_H__
#define __CHECKBOX_H__

#include <rexio/tk/activewidget.h++>
#include <rexio/tk/label.h++>

namespace Scr
{
	namespace Tk
	{
		const DisplayStyle CHECKBOX_DEFAULT_STYLE(
			ACTIVEWIDGET_DEFAULT_STYLE);
		const DisplayStyle CHECKBOX_DEFAULT_ACTIVESTYLE(
			ACTIVEWIDGET_DEFAULT_ACTIVESTYLE);
		
		/*!
		  \brief two-state widget
		  
		  A widgets that indicates setting of boolean feature canonly
		  be turned on and off. It has label, that indicates its name
		  and boolean field that indicates its current state
		 */
		class Checkbox:public ActiveWidget
		{
		private:
			//! label near the checkbox
			Label label;

			/*!
			  current state (on/off) displayed to user
			*/
			bool state;
		protected:
			Checkbox(Uint _height,	
					 Uint _width,
					 const Label& _label,
					 const DisplayStyle& _style
					 = CHECKBOX_DEFAULT_STYLE,
					 const DisplayStyle& _activeStyle
					 = CHECKBOX_DEFAULT_ACTIVESTYLE)throw();
			Checkbox(const Label& _label,
					 const DisplayStyle& _style
					 = CHECKBOX_DEFAULT_STYLE,
					 const DisplayStyle& _activeStyle
					 = CHECKBOX_DEFAULT_ACTIVESTYLE)throw();
			~Checkbox()throw();
		public:
			void OnRedraw(Screen& screen)throw();
			void OnAction()throw();
			void SetLabel(const Label& _label)throw();
			const Label& GetLabel()throw();
			void SetState(bool _state)throw();
			bool GetState()throw();
			RTTI_OBJ(Checkbox, ActiveWidget);
		};
	}
}
#endif // __CHECKBOX_H__
