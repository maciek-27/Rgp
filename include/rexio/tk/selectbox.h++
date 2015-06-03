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

#ifndef __SELECTBOX_H__
#define __SELECTBOX_H__

#include <rexio/tk/activewidget.h++>
#include <rexio/tk/label.h++>
#include <rexio/tk/scrollbar.h++>
#include <rexio/tk/framedwindow.h++>
#include <rexio/tk/verticalgroup.h++>
#include <rexio/tk/rootwindow.h++>

namespace Scr
{
	namespace Tk
	{
		const DisplayStyle SELECTBOX_DEFAULT_STYLE(
			ACTIVEWIDGET_DEFAULT_STYLE);
		const DisplayStyle SELECTBOX_DEFAULT_ACTIVESTYLE(
			ACTIVEWIDGET_DEFAULT_ACTIVESTYLE);

		const wchar_t _DEFAULT_SELECTBOX_OPENBUTTON = 0x2193;
		const DisplayStyle _DEFAULT_SELECTBOX_OPENSTYLE(
			ACTIVEWIDGET_DEFAULT_ACTIVESTYLE);

		//! Selectbox specific style
		/*!
		  Describes the way a specific selectbox is drawn.
		*/
		struct SelectboxStyle {
			/*!
			  \param _openButton symbol for drawing opening symbol
			  \param _openStyle color for drawing the opening symbol
			*/
			SelectboxStyle(
				const wchar_t _openButton = _DEFAULT_SELECTBOX_OPENBUTTON,
				const DisplayStyle& _openStyle = _DEFAULT_SELECTBOX_OPENSTYLE
				)throw() :openButton(_openButton), openStyle(_openStyle) {;};			
			wchar_t openButton;
			DisplayStyle openStyle;
		};
		//! Selection form widget.
		namespace Detail{class Selector;}
		/*!
		  Widget allowing to select one of available options.
		*/
		class Selectbox : public ActiveWidget
		{
			friend class Detail::Selector;
		private:
			//! Actual list of available options at Selectbox.
			class _SelectList : public FramedWindow
			{
			private:
				class SelectGroup:public VerticalGroup
				{
				public:
					SelectGroup(Uint _height,	
								Uint _width,
								const DisplayStyle & _style
								= DisplayStyle(Fg::White,Fg::Dark,Bg::Black))
					throw()
						:
						BoxGroup(_height,_width,_style),
						VerticalGroup(_height,_width,_style){;}
					using Window::elements;
					using Window::activeWidget;
				};
			public:
				//! Scrollbar.
				VerticalScrollbar scroll;
				SelectGroup group;
				/*!
				  previous active widget at RootWindow to which the
				  focus will have to be returned.
				*/
				Widget *prevActive;
				
				_SelectList(Uint _width, Uint _height, 
							const DisplayStyle& _style)throw();
				void OnResize()throw();
				
				void CloseSelectList();
				void OnKeyDown(Key key)throw();
				void OnFocus(FocusPolicy focustype)throw();
				void OnUnFocus(FocusPolicy focustype)throw();
				
				virtual ~_SelectList()throw();
			};
		protected:
			//! internal style
			SelectboxStyle selectboxStyle;
			//! list of options
			_SelectList selectList;
			//! indicated whether the list of options is open
			bool opened;
		public:
			__DE(NoSuchOption, Exception);

			/*!
			  \param width
			  \param _style
			  \param _activeStyle
			  \param _selectboxStyle
			*/
			Selectbox(Uint width,
					  const DisplayStyle& _style =
					  SELECTBOX_DEFAULT_STYLE,
					  const DisplayStyle& _activeStyle = 
					  SELECTBOX_DEFAULT_ACTIVESTYLE,
					  const SelectboxStyle& _selectboxStyle = 
					  SelectboxStyle())throw();
			Selectbox(const DisplayStyle& _style =
					  SELECTBOX_DEFAULT_STYLE,
					  const DisplayStyle& _activeStyle = 
					  SELECTBOX_DEFAULT_ACTIVESTYLE,
					  const SelectboxStyle& _selectboxStyle = 
					  SelectboxStyle())throw();
			
			/*!
			  \param name
			  \return unique identifier

			  Adds new option to the list.
			*/
			Uint AddOption(const std::string& name)throw();
			/*!
			  \param id
			  \return Selected option
			  \exception NoSuchOption if no option is selected
			*/
			const std::string& GetOption() const throw(NoSuchOption);
			/*!
			  \param id identifier of option to delete
			  Deletes option from the list.
			*/
			void DelOption(Uint id)throw(NoSuchOption) {;};
			void OnAction()throw();
			void OnRedraw(Screen& screen)throw();
			void OnFocus(FocusPolicy focusPolicy)throw();
			void OnUnFocus(FocusPolicy focusPolicy)throw();
			RTTI_OBJ(Selectbox, ActiveWidget);
		};
	}
}

#endif // __SELECTBOX_H__
