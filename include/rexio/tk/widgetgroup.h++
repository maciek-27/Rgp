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

#ifndef __WIDGETGROUP_H__
#define __WIDGETGROUP_H__

#include "window.h++"

namespace Scr
{
	namespace Tk
	{ 
		//! General class for grouping widgets and managing them.
		/*!
		  This class is a base class for all sorts of of grouping widgets.
		  Widgets inside of
		*/
		class WidgetGroup:public Window
		{
		protected:
			WidgetGroup(Uint _height,	
						Uint _width,
						const DisplayStyle & _style
						= WINDOW_DEFAULT_STYLE)throw();
			WidgetGroup(const WidgetGroup & base)throw();
			/*!
			  where all magic is done :)
			 */
			virtual void ArrangeContents()throw();
		public:
			/*!
			  \param widget1 First widget
			  \param widget2 Second widget

			  Swap two widgets with together, provided that they are being
			  contained by the WidgetGroup.
			*/
			virtual void SwapWidgets(Widget& widget1, Widget &widget2)throw();
			/*!
			  \param widget Targetted widget
			  
			  Move the widget further away on the containing widget list.
			  Upon end of the list, move to the beginning.
			*/
			virtual void ShiftFWidget(Widget &widget)throw();
			/*!
			  \param widget Targetted widget
			  
			  Move the widget closer on the containing widget list.
			  Upon beginning of the list, move to the end.
			*/
			virtual void ShiftBWidget(Widget &widget)throw();

			virtual ~WidgetGroup()throw();

			RTTI_OBJ(WidgetGroup, Window);
		};
	}
}

#endif // __WIDGETGROUP_H__
