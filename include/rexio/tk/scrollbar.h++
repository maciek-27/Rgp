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

#ifndef __SCROLLBAR_H__
#define __SCROLLBAR_H__

#include <rexio/tk/widget.h++>

namespace Scr
{
	namespace Tk
	{		
		const DisplayStyle _DEFAULT_SCROLLBAR_BUTTON(
			Fg::Black, Fg::Dark, Bg::White);
		const DisplayStyle _DEFAULT_SCROLLBAR_BUTTONPRESSED(
			Fg::Black, Fg::Bright, Bg::White);

		const wchar_t _DEFAULT_SCROLLBAR_BUTTONUP = 0x2191;
		const wchar_t _DEFAULT_SCROLLBAR_BUTTONDOWN = 0x2193;
		const wchar_t _DEFAULT_SCROLLBAR_BUTTONLEFT = 0x2190;
		const wchar_t _DEFAULT_SCROLLBAR_BUTTONRIGHT = 0x2192;
		const DisplayStyle _DEFAULT_SCROLLBAR_SCROLLBG(
			Fg::White, Fg::Dark, Bg::Black);
		const wchar_t _DEFAULT_SCROLLBAR_SCROLLFIELD = 0x2592;
		const DisplayStyle _DEFAULT_SCROLLBAR_SCROLLFG(
			Fg::Black, Fg::Dark, Bg::White);
		const wchar_t _DEFAULT_SCROLLBAR_SCROLLHANDLEV = 0x2195;
		const wchar_t _DEFAULT_SCROLLBAR_SCROLLHANDLEH = 0x2194;

		//! Scrollbars specific style.
		/*!
		  Describes the way a specific scrollbar is drawn.
		*/
		struct ScrollbarStyle {
			/*!
			  \param _button style for directional buttons
			  \param _buttonPressed style for pressed buttons
			  \param _buttonUp symbol for drawing up button
			  \param _buttonDown symbol for drawing down button
			  \param _buttonLeft symbol for drawing left button
			  \param _buttonRight symbol for drawing right button
			  \param _scrollBg style for drawing scrollbar's 
			  \param _scrollField symbol for drawing scrollbar's area
			  \param _scrollFg style for drawing scrollbar's area
			  \param _scrollHandleV symbol for vertical handle
			  \param _scrollHandleH symbol for horizontal handle
			*/
			ScrollbarStyle(
				const DisplayStyle& _button = 
				_DEFAULT_SCROLLBAR_BUTTON,
				const DisplayStyle& _buttonPressed = 
				_DEFAULT_SCROLLBAR_BUTTONPRESSED,
				wchar_t _buttonUp = _DEFAULT_SCROLLBAR_BUTTONUP,
				wchar_t _buttonDown = _DEFAULT_SCROLLBAR_BUTTONDOWN,
				wchar_t _buttonLeft = _DEFAULT_SCROLLBAR_BUTTONLEFT,
				wchar_t _buttonRight = _DEFAULT_SCROLLBAR_BUTTONRIGHT,
				const DisplayStyle& _scrollBg = _DEFAULT_SCROLLBAR_SCROLLBG,
				wchar_t _scrollField = _DEFAULT_SCROLLBAR_SCROLLFIELD,
				const DisplayStyle& _scrollFg = _DEFAULT_SCROLLBAR_SCROLLFG,
				wchar_t _scrollHandleV = _DEFAULT_SCROLLBAR_SCROLLHANDLEV,
				wchar_t _scrollHandleH = _DEFAULT_SCROLLBAR_SCROLLHANDLEH
				)throw():
				button(_button), buttonPressed(_buttonPressed), 
				buttonUp(_buttonUp), buttonDown(_buttonDown),
				buttonLeft(_buttonLeft), buttonRight(_buttonRight),
				scrollBg(_scrollBg), scrollField(_scrollField),
				scrollFg(_scrollFg),
				scrollHandleV(_scrollHandleV), scrollHandleH(_scrollHandleH)
				{;}						
			//! style for directional buttons
			DisplayStyle button;
			//! style for pressed buttons
			DisplayStyle buttonPressed;
			//! symbol for drawing up button
			wchar_t buttonUp;
			//! symbol for drawing down button
			wchar_t buttonDown;
			//! symbol for drawing left button
			wchar_t buttonLeft;
			//! symbol for drawing right button
			wchar_t buttonRight;
			//! style for drawing scrollbar's 
			DisplayStyle scrollBg;
			//! symbol for drawing scrollbar's area
			wchar_t scrollField;
			//! style for drawing scrollbar's area
			DisplayStyle scrollFg;
			//! symbol for vertical handle
			wchar_t scrollHandleV;
			//! symbol for horizontal handle
			wchar_t scrollHandleH;
		};

		//! Base for implementing scrollbars.
		/*!
		  This class implements interface for HorizontalScrollbar 
		  and VerticalScrollbar. Allows setting progress, offsets,
		  size, style.
		*/
		class ScrollbarBase : public Widget
		{
		protected:			
			ScrollbarBase(Uint _width, Uint _height,
					  const ScrollbarStyle& _scrollbarStyle
					  = ScrollbarStyle()
				)throw();
			ScrollbarStyle scrollbarStyle;
			Uint scrollSize;
			Uint scrollOffset;
		public:
			virtual void OnRedraw(Screen& screen)throw() = 0;
			/*!
			  \param _scrollSize
			  Set virtual area that the scrollbar should cover.
			*/
			virtual void SetScrollSize(Uint _scrollSize)throw();
			/*!
			  \return virtual size the scrollbar covers.
			*/
			virtual Uint GetScrollSize() const throw();
			/*!
			  \param _scrollOffset
			  Set number of virtual offset.
			*/
			virtual void SetScrollOffset(Uint _scrollOffset)throw();
			/*!
			  Return virtual offset.
			*/
			virtual Uint GetScrollOffset() const throw();
			/*!
			  \param progress
			  Provided for convenience. Sets the scrollOffset
			  in respect to scrollSize accordingly to given
			  progress.
			*/
			virtual void SetScrollProgress(float progress)throw();
			/*!
			  \return Current scrolling progress.
			*/
			virtual float GetScrollProgress() const throw();

			/*! \param _scrollStyle new style 
			  Set scrollbar specific style.
			*/
			virtual void SetScrollbarStyle(
				const ScrollbarStyle& _scrollStyle)throw();
			/*!
			  \return current scrollbar specific style
			*/
			virtual const ScrollbarStyle& GetScrollbarStyle() const throw();
			
			virtual void SetStylesheet(Stylesheet* _styleSheet)throw() {
				Widget::SetStylesheet(_styleSheet);
				__FetchProperty(scrollbarStyle.button, "buttonStyle");
				__FetchProperty(scrollbarStyle.buttonPressed,
								"buttonPressed");
				__FetchProperty(scrollbarStyle.buttonUp, "buttonUp");
				__FetchProperty(scrollbarStyle.buttonDown, "buttonDown");
				__FetchProperty(scrollbarStyle.buttonLeft, "buttonLeft");
				__FetchProperty(scrollbarStyle.buttonRight, "buttonRight");
				__FetchProperty(scrollbarStyle.scrollBg, "scrollBg");
				__FetchProperty(scrollbarStyle.scrollField, "scrollField");
				__FetchProperty(scrollbarStyle.scrollFg, "scrollFg");
				__FetchProperty(scrollbarStyle.scrollHandleV,
								"scrollHandleV");
				__FetchProperty(scrollbarStyle.scrollHandleH,
								"scrollHandleH");
			}

			RTTI_OBJ(ScrollbarBase, Widget);
		};
		
		//! Horizontal scrollbar
		class HorizontalScrollbar : public ScrollbarBase {
		public:
			/*!
			  \param _width
			  \param _scrollbarStyle
			*/
			HorizontalScrollbar(Uint _width,
								const ScrollbarStyle& _scrollbarStyle
								= ScrollbarStyle())throw();
			virtual void OnRedraw(Screen& screen)throw();

			RTTI_OBJ(HorizontalScrollbar, ScrollbarBase);
		};
		//! Vertical scrollbar
		class VerticalScrollbar : public ScrollbarBase {
		public:
			/*!
			  \param _height
			  \param _scrollbarStyle
			*/
			VerticalScrollbar(Uint _height,
							  const ScrollbarStyle& _scrollbarStyle
							  = ScrollbarStyle())throw();
			virtual void OnRedraw(Screen& screen)throw();
			RTTI_OBJ(VerticalScrollbar, ScrollbarBase);
		};
	}
}

#endif // __SCROLLBAR_H__
