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

#ifndef __LABEL_H__
#define __LABEL_H__

#include <rexio/screen.h++>
#include <rexio/tk/window.h++>

namespace Scr
{
	namespace Tk
	{
		const DisplayStyle LABEL_DEFAULT_STYLE(
			DisplayStyle(Fg::Transparent, Fg::Dark, Bg::Transparent));
		/*!
		  Simple text data holder.
		*/
		class Label : public Widget
		{
		protected:
			/*!
			  Actual label holder.
			*/
			std::string label;
		public:
			Label(const DisplayStyle& _style = LABEL_DEFAULT_STYLE)throw();
			explicit Label(Uint _width,
						   const std::string&  _label,
						   const DisplayStyle& _style = LABEL_DEFAULT_STYLE)
				throw();
			explicit Label(const std::string&  _label,
						   const DisplayStyle& _style = LABEL_DEFAULT_STYLE)
				throw();

			virtual void SetStylesheet(Stylesheet* _styleSheet)throw() {
				Widget::SetStylesheet(_styleSheet);
				__FetchProperty(style, "style");
				__FetchProperty(label, "content");
			}

			/*!
			  \return containing text
			  
			  Return the actual label text.
			*/
			virtual const std::string& GetText() const throw();
			/*!
			  \param _label string to replace current content
			  of label

			  Set the actual label text.
			*/
			virtual void SetText(const std::string _label)throw();
			virtual void OnFocus(FocusPolicy focustype)throw();
			virtual void OnUnFocus(FocusPolicy focustype)throw();
			virtual void OnRedraw(Screen& screen)throw();

			virtual ~Label()throw();
			RTTI_OBJ(Label, Widget);
		};

	}
	Screen& operator<<(Screen & screen,const Tk::Label& whatto);

}

#endif // __LABEL_H__
