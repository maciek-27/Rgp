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

#ifndef __HORIZONTALGROUP_H__
#define __HORIZONTALGROUP_H__

#include <rexio/tk/boxgroup.h++>

namespace Scr
{
	namespace Tk
	{
		//! Horizontal widget grouping capabilities.
		/*!
		  Intelligently places the containing widgets among allocated space.
		  Widgets are placed horizontally.
		*/
		class HorizontalGroup:virtual public BoxGroup
		{
		protected:
			virtual void ArrangeContents()throw();
		public:
			HorizontalGroup(const WidgetGroup & base)throw();
			HorizontalGroup(Uint _height,	
							Uint _width,
							const DisplayStyle & _style
							= DisplayStyle(Fg::White,Fg::Dark,Bg::Black))
				throw();
			virtual ~HorizontalGroup()throw();
			RTTI_OBJ(HorizontalGroup, BoxGroup);
		};
	}
}
#endif // __HORIZONTALGROUP_H__
