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

#ifndef __BOXGROUP_H__
#define __BOXGROUP_H__

//#include <ext/hash_map>
#include <rexio/tk/widgetgroup.h++>
#include <tr1/unordered_map>
//namespace HASH_MAP_NAMESPACE {
//	template <>
//}

namespace Scr
{
	namespace Tk
	{
		//! Provides horizontal and vertical widget grouping capabilities.
		/*!
		  Intelligently places the containing widgets among allocated space.
		  Widgets can be placed vertically or horizontally.
		  \latexonly
		  \begin{figure}
		  \begin{center}
		  \leavevmode
		  \includegraphics[width=200pt]{../boxgroup1} \\
		  BoxGroup in Horizontal Mode. Widget1's stretchFactor == 1 and the others' is 2. \\
		  \includegraphics[width=120pt]{../boxgroup2}
		  \includegraphics[width=120pt]{../boxgroup3}
		  \includegraphics[width=120pt]{../boxgroup4} \\
		  All of the widgets here have their maxSize set so that there is still free space. \\
		  It shows different types of AlignPolicy. Respectively: Center, Start, Distributed.
		  \end{center}
		  \end{figure}
		  \endlatexonly

		  \sa \a VerticalGroup and \a HorizontalGroup provided for convenience.		 
		*/		
		class BoxGroup:public WidgetGroup
		{
		public:
			/*!
			  Widget aligning policy in case of not all space being used.
			*/
			typedef enum {
				/*!
				  Align everything to the left/top depending on \a groupType.
				*/
				Begin,
				/*!
				  Align everything to the center.
				*/
				Center,
				/*!
				  Align everything to the right/bottom depending on
				  \a groupType.
				*/
				End,
				/*!
				  Try to evenly distribute free space between widgets,
				  adding a margin between each of them.
				*/
				Distribute
			} AlignPolicy;
		protected:
			BoxGroup(Uint _height,	
					 Uint _width,
					 const DisplayStyle & _style
					 = WINDOW_DEFAULT_STYLE)throw();
			
			BoxGroup(const WidgetGroup & base)throw();

			virtual void ArrangeContents()throw() = 0;
			
			//! Additional data used for positioning.
			/*!
			  Widget layouting information inside BoxGroup.
			*/
			struct LayoutData {
				LayoutData(Uint _stretchFactor) :
					stretchFactor(_stretchFactor) {RexIOLog(LogLevelModerate) << "Layout data created" << std::endl;}
				LayoutData() : stretchFactor(1) {RexIOLog(LogLevelModerate) << "Layout data created" << std::endl;}
				/*!
				  Defines a factor of dividing free space between widgets.
				  i.e.
				  space = (this_factor/sum_of_factors) * freespace.
				*/
				Uint stretchFactor;
			};
			
			struct _hash {
				size_t operator() (const Scr::Tk::Widget *x) const {
					return reinterpret_cast<size_t>(x);
				}
			};
			
//			HASH_MAP_NAMESPACE::
			/*!
			  Associates \a LayoutData to each attached widget.
			*/
			std::tr1::unordered_map<const Widget*, LayoutData,_hash> elementsLayout;

			/*!
			  Current aligning policy.
			*/
			AlignPolicy alignPolicy;
		public:		  
			/*!
			  \param widget1 First widget
			  \param widget2 Second widget

			  Swap two widgets with together, provided that they are being
			  contained by the WidgetGroup. rearrange contents afterwards
			*/
			virtual void SwapWidgets(Widget& widget1, Widget &widget2)throw();
			virtual void AddWidget(Widget& widget)throw();
			/*!
			  \param widget widget to attach to this window
			  \param stretchFactor part of the added widget's \a LayoutData

			  Attach a widget to this window.
			  Specifically, add it to the \a elements.
			*/
			virtual void AddWidget(Widget& widget, Uint stretchFactor)throw();
			virtual void DelWidget(Widget& widget)throw();

			virtual void OnStart()throw();
			virtual void OnResize()throw();

			/*!
			  \param _alignPolicy enumerative type parameter
			  specifying aling policy (refer to documentation for this
			  class for information on it)
			  
			  Set new BoxGroupType. Can be invoked anytime and it will
			  initiate a redraw.
			*/
			virtual void SetAlignPolicy(AlignPolicy _alignPolicy)throw();
			/*!
			  Get current AlignPolicy.
			*/
			virtual AlignPolicy GetAlignPolicy()throw();
			virtual ~BoxGroup()throw();
			RTTI_OBJ(BoxGroup, WidgetGroup);
		};
	}
}

#endif // __BOXGROUP_H__
