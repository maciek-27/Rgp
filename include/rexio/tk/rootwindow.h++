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

#ifndef __ROOTWINDOW_H__
#define __ROOTWINDOW_H__

#include <rexio/screen.h++>
#include <rexio/tk/window.h++>
#include <fstream>

namespace Scr
{
	namespace Tk
	{
		const DisplayStyle ROOTWINDOW_DEFAULT_STYLE(WINDOW_DEFAULT_STYLE);

		//! Main application window.
		/*!
		  Lord of the widgets and the main connection point between
		  the Toolkit and lower level library.
		*/
		class RootWindow:public Connection,public Window
		{
		protected:			
			/*!
			  \param _input input stream handler
			  \param _output output stream handler
			  \param _style default style

			  The input handlers make it possible to attach to any character
			  device. Specifically it can be an ordinary terminal
			  or a tcp connection to a remote telnet application.
			*/
			RootWindow(std::istream& _input, std::ostream& _output,
					   const DisplayStyle & _style
					   = ROOTWINDOW_DEFAULT_STYLE)throw();
			/*!
			  \return Screen handler reference.
			*/
			virtual Screen& GetScreen()throw();
			/*!
			  \return 0
			*/
			virtual Uint GetAbsoluteColumn()throw();
			/*!
			  \return 0
			*/
			virtual Uint GetAbsoluteRow()throw();
		public:
			
			// Process events recieved as Connection
			// using procedures typical to Window
			using Connection::OnStart;
			using Connection::OnKeyDown;
			using Window::OnResize;

			/*!
			  \copydoc Connection::Start(int, char **)
			  
			  \a RootWindow specific:
			  
			  Arguments:
			  -style=FILE     - Use this FILE as a stylesheet.
			*/
			virtual int Start(int argc, char **argv)throw(StartFailed,Screen::IllegalCharacter);
			virtual int Start()throw(StartFailed,Screen::IllegalCharacter);

			/*!
			  \return referene to self			  
			*/
			virtual RootWindow& GetRootWindow()throw();

			virtual void OnStart()throw();
			virtual void OnKeyDown(Key key)throw();
			virtual void OnRedraw(Screen& screen)throw();
			virtual void OnResize(Uint rows, Uint cols)throw();

			__DE(FileNotOpened, Exception);
			/*!
			  \param filename location of the stylesheet

			  Loads stylesheet from the given location.

			  \exception FileNotOpened is thrown if the file couldn't
			  be opened.

			  \exception ParsingError is thrown if the input file contained
			  inappropriate input.
			*/
			void LoadStylesheet(const char* filename)
				throw(FileNotOpened, Stylesheet::ParsingError);

			/*!
			  Repaints whole screen (useful after invoking background
			  programs, that modify its content)
			 */
			void ForceRepaint()throw();

			/*!
			  Trigger OnRedraw event
			 */
			void ForceOnRedraw()throw()
				{OnRedraw(*screen);}
			
			virtual ~RootWindow()throw();

			RTTI_OBJ(RootWindow, Window);
		}; // RootWindow
	} // Tk
} // Scr
#endif // __ROOT_WINDOW_H__
