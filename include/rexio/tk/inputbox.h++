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

#include "activewidget.h++"

/*
  TODO:
  -Undo stack.
  -Text marking.
  -Possible clipboard interaction.
*/

namespace Scr
{
	namespace Tk
	{
		struct InputboxStyle {
			InputboxStyle(const DisplayStyle& _cursorStyle)throw() : 
				cursorStyle(_cursorStyle) {;}
			DisplayStyle cursorStyle;
		};

		const DisplayStyle INPUTBOX_DEFAULT_STYLE(
			ACTIVEWIDGET_DEFAULT_STYLE);
		const DisplayStyle INPUTBOX_DEFAULT_ACTIVESTYLE(
			ACTIVEWIDGET_DEFAULT_ACTIVESTYLE);
		const InputboxStyle INPUTBOX_DEFAULT_IBOXSTYLE(
			DisplayStyle(Fg::System, Fg::Bright, Bg::System));
			
		//! Simple text input field.
		/* Text input field with wide char support and text winding. */
		class Inputbox : public ActiveWidget
		{
		private:
			//! Column position of the cursor.
			//! 0 is considered beginning of Inputbox
			Uint cursorPos;
			//! After which character in the current input
			//! the cursor is located
			Uint charPos;

			//! Currently shown number of columns
			Uint curCols;
			//! Currently shown number of characters
			Uint curChars;
		protected:
			//! Index of first character currently visible in the input.
			Uint textOffset;

			//! Text content..
			std::wstring text;

			//! Inputbox specific style
			InputboxStyle inputboxStyle;

			//! Maximum length of input
			Uint maxLength;
			
		public:
			__DE(OffsetOutOfRange, Exception); // Wrong offset supplied

			Inputbox(Uint _width,
					 const std::wstring&  _text,
					 const DisplayStyle& _style
					 = INPUTBOX_DEFAULT_STYLE,
					 const DisplayStyle& _activeStyle
					 = INPUTBOX_DEFAULT_ACTIVESTYLE,
					 const InputboxStyle& _inputboxStyle
					 = INPUTBOX_DEFAULT_IBOXSTYLE)throw();
			Inputbox(const std::wstring&  _text,
					 const DisplayStyle& _style
					 = INPUTBOX_DEFAULT_STYLE,
					 const DisplayStyle& _activeStyle
					 = INPUTBOX_DEFAULT_ACTIVESTYLE,
					 const InputboxStyle& _inputboxStyle
					 = INPUTBOX_DEFAULT_IBOXSTYLE)throw();

			/*!
			  \param _text Extended string to replace current content
			  of inputbox
			  
			  Set the actual content text.
			*/
			virtual void SetText(const std::wstring& _text)throw();

			/*!
			  \return const reference to the containing text

			  Get the content text.
			*/
			virtual const std::wstring& GetText()throw();
			
			/*!
			  \param _maxLength new value

			  Set max length of possible input
			*/
			virtual void SetMaxLength(Uint _maxLength)throw();

			/*!
			  \return \a maxLength

			  Get max length of possible input
			*/
			virtual Uint GetMaxLength()throw();
			
			/*!
			  \param _textOffset new value

			  Set new text offset.

			  \exception OffsetOutOfRange is thrown had the offset been 
			  wrongly provided.
			*/
			virtual void SetOffset(Uint _textOffset)throw(OffsetOutOfRange);

			/*!
			  \return \a textOffset

			  Return current text offset.
			*/
			virtual Uint GetOffset()throw();
			

			virtual void SetStylesheet(Stylesheet* _styleSheet)throw() {
				ActiveWidget::SetStylesheet(_styleSheet);
				__FetchProperty(inputboxStyle.cursorStyle, "cursorStyle");
			}
			
			virtual void OnKeyDown(Key key)throw();
			virtual void OnRedraw(Screen& screen)throw();

			~Inputbox()throw();
			RTTI_OBJ(Inputbox, ActiveWidget);
		};
	}
}
