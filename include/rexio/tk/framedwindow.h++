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

#ifndef __FRAMEDWINDOW_H__
#define __FRAMEDWINDOW_H__

#include <rexio/tk/virtualwindow.h++>

namespace Scr
{
	namespace Tk
	{
		const wchar_t _DEFAULT_FRAME_TOP = 0x2500 ;
		const wchar_t _DEFAULT_FRAME_BOTTOM = 0x2500;
		const wchar_t _DEFAULT_FRAME_LEFT = 0x2502;
		const wchar_t _DEFAULT_FRAME_RIGHT  = 0x2502;
		const wchar_t _DEFAULT_FRAME_TOPLEFT = 0x250C;
		const wchar_t _DEFAULT_FRAME_TOPRIGHT = 0x2510;
		const wchar_t _DEFAULT_FRAME_BOTTOMLEFT = 0x2514;
		const wchar_t _DEFAULT_FRAME_BOTTOMRIGHT = 0x2518;
		/*!
		  Frame specific style.
		*/
		struct FrameStyle {
		public:			
			/*!
			  \param _frameColor frame color
			  \param top
			  \param bottom
			  \param left
			  \param right
			  \param topLeft
			  \param topRight
			  \param bottomLeft
			  \param bottomRight
			*/
			FrameStyle(const DisplayStyle& _frameColor,
					   wchar_t top = _DEFAULT_FRAME_TOP,
					   wchar_t bottom = _DEFAULT_FRAME_BOTTOM,
					   wchar_t left = _DEFAULT_FRAME_LEFT,
					   wchar_t right = _DEFAULT_FRAME_RIGHT,
					   wchar_t topLeft = _DEFAULT_FRAME_TOPLEFT,
					   wchar_t topRight = _DEFAULT_FRAME_TOPRIGHT,
					   wchar_t bottomLeft = _DEFAULT_FRAME_BOTTOMLEFT,
					   wchar_t bottomRight = _DEFAULT_FRAME_BOTTOMRIGHT) :
				frameColor(_frameColor) {
				properties.top = top;
				properties.bottom = bottom;
				properties.left = left;
				properties.right = right;
				properties.topLeft = topLeft;
				properties.topRight = topRight;
				properties.bottomLeft = bottomLeft;
				properties.bottomRight = bottomRight;
			}
			//! color of the frame
			DisplayStyle frameColor;
			//! holds characters used for frame drawing
			union {
				wchar_t frame[8];
				struct {
					wchar_t top;
					wchar_t bottom;
					wchar_t left;
					wchar_t right;
					wchar_t topLeft;
					wchar_t topRight;
					wchar_t bottomLeft;
					wchar_t bottomRight;
				} properties;
			};
		}; // FrameStyle

		const DisplayStyle _DEFAULT_FRAME_COLOR(Fg::White, Fg::Bright, 
												Bg::Black);
		const DisplayStyle FRAMEDWINDOW_DEFAULT_STYLE(Fg::System, Fg::Dark,
													  Bg::System);
		const FrameStyle FRAMEDWINDOW_DEFAULT_FRAMESTYLE(
			_DEFAULT_FRAME_COLOR);
		
		/*!
		  \param W class of inside's window.
		  Template for all framed windows.
		  FramedWindowBase is basically a window having a separate internal
		  window to which most of the calls(like AddWidget) are routed.
		*/
		template <class W>
		class FramedWindowBase : public VirtualWindow<W>
		{
		protected:
			using VirtualWindow<W>::inside;
		public:
			using VirtualWindow<W>::GetHeight;
			using VirtualWindow<W>::GetWidth;
			using VirtualWindow<W>::SetStyle;
			using VirtualWindow<W>::GetStyle;
			
		protected:
			//! how to draw a frame around \a inside.
			FrameStyle frameStyle;
		public:
			/*!
			  \param _height desired height
			  \param _width desired width
			  \param _style optional style
			  \param _frameStyle optional frame style
			*/
			FramedWindowBase(Uint _height,  Uint _width,
							 const DisplayStyle& _style
							 = FRAMEDWINDOW_DEFAULT_STYLE,
							 const FrameStyle& _frameStyle
							 = FRAMEDWINDOW_DEFAULT_FRAMESTYLE) throw()
				: VirtualWindow<W>(_height, _width, _frameStyle.frameColor), 
				  frameStyle(_frameStyle) {

				inside.SetSize(Size(_height - 1, _width - 1));
				inside.SetPosition(1, 1);
			}
			virtual void OnResize()throw() {
				Widget::OnResize();
				inside.SetSize(Size((GetHeight() >= 2)?GetHeight() - 2:0,
					(GetWidth() >= 2)?GetWidth() - 2:0));
				inside.SetPosition(1, 1);
			}

			virtual void SetStylesheet(Stylesheet* _styleSheet)throw() {
				Window::SetStylesheet(_styleSheet);
				std::cerr << this->TypeName() << " stylesheet."<<std::endl;
				__FetchProperty(frameStyle.frameColor, "frameColor");
				SetStyle(frameStyle.frameColor);
				DisplayStyle style;
				__FetchProperty(style, "style");
				inside.SetStyle(style);

				__FetchProperty(frameStyle.properties.top,
								"top");
				__FetchProperty(frameStyle.properties.bottom,
								"bottom");
				__FetchProperty(frameStyle.properties.left,
								"left");
				__FetchProperty(frameStyle.properties.right,
								"right");
				__FetchProperty(frameStyle.properties.topLeft,
								"topLeft");
				__FetchProperty(frameStyle.properties.topRight,
								"topRight");
				__FetchProperty(frameStyle.properties.bottomLeft,
								"bottomLeft");
				__FetchProperty(frameStyle.properties.bottomRight,
								"bottomRight");
			}

			virtual void OnRedraw(Screen& screen)throw() {
				screen << GetStyle();
				VirtualWindow<W>::OnRedraw(screen);
				screen << frameStyle.frameColor;
				screen.GotoYX(0, 0);
				screen.AddCharacter(frameStyle.properties.topLeft);
				screen.HorizontalLine(frameStyle.properties.top, GetWidth() - 2);
				screen.AddCharacter(frameStyle.properties.topRight);
				screen.GotoYX(GetHeight() - 1, GetWidth() - 1);
				
				screen.GotoYX(1, 0);
				screen.VerticalLine(frameStyle.properties.left, GetHeight() - 2);
				screen.GotoYX(1, GetWidth() - 1);
				screen.VerticalLine(frameStyle.properties.right, GetHeight() - 2);
				
				screen.GotoYX(GetHeight() - 1, 0);
				screen.AddCharacter(frameStyle.properties.bottomLeft);
				screen.HorizontalLine(frameStyle.properties.bottom, GetWidth() - 2);
				screen.AddCharacter(frameStyle.properties.bottomRight);
			}
			
			virtual void SetFrameStyle(const FrameStyle &_frameStyle) {
				frameStyle = _frameStyle;
			}

//			RTTI_OBJ(FramedWindow, Window);
		}; // FramedWindowBase

		/*!
		  Basic FramedWindow with basic Window as its internal area.
		*/
		class FramedWindow : public FramedWindowBase<Window>
		{
		public:
			/*!
			  \param _height desired height
			  \param _width desired width
			  \param _style optional style
			  \param _frameStyle optional frame style
			*/
			FramedWindow(Uint _height, Uint _width,
						 const DisplayStyle& _style
						 = FRAMEDWINDOW_DEFAULT_STYLE,
						 const FrameStyle& _frameStyle
						 = FRAMEDWINDOW_DEFAULT_FRAMESTYLE
				)throw();
			RTTI_OBJ(FramedWindow, Window);
		}; // FramedWindow
	} // Tk
} // Scr
#endif // __FRAMEDWINDOW_H__
