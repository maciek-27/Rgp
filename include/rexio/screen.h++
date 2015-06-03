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

#ifndef __SCREEN_H__ 
#define __SCREEN_H__

#include <rexio/commons.h++>
#include <rexio/keyboard.h++>

//! Namespace of lower half of the library
/*!
  This namespace contains classes and other utilities connected with
  general purpose text screen manipulation and input processing. It
  implements platform independent Screen class, and Connection class
  representing basic framework for console application development.
  \note Scr::Tk is upper part of the library, and is recommended for
  all higher level UI manipulation, but Scr::Connection can be used
  alone

  \latexonly

  Following figure is
  simplified class relationship diagram for Scr::Screen and
  Scr::Connection connected items, focusing on internal layout of
  implementation of screen.

  \begin{figure}[H]
  \begin{center}
  \leavevmode\includegraphics[width=400pt]{../Basic_class_structure_uml}
  \end{center}
  \end{figure}

  \endlatexonly  
*/
namespace Scr
{   
    //! Foreground colors and styles.
    namespace Fg
    {
		//! Color itself. 8 basic colours + 2 special (Fg::System, Fg::Transparent)
		enum Color
		{
			//!  special colour represents default colour of system
			//!  (for some terminals and terminal emulator this may
			//!  differ from 8 basic colors)
			System      = 0,
			//!  special colour represents colour of
			//! just-replaced character
			Transparent = 1,
			Black       = 30,//!< color 1
			Red         = 31,//!< color 2
			Green       = 32,//!< color 3
			Yellow      = 33,//!<color 4
			Blue        = 34,//!<color 5
			Magenta     = 35,//!<color 6
			Cyan        = 36,//!<color 7
			White       = 37 //!<color  8
		};
		//!  foreground styles
		enum Style
		{
			Dark   = 0,
			Bright = 1
		};// in future maybe also UnderlineOn and UnderlineOff... etc.
    }
    //! Background colors. WITHOUT style.
    namespace Bg
    {
		//! background colours enumeration
		enum Color
		{
			//!  special colour represents default colour of system
			//!  (for some terminals and terminal emulator this may
			//!  differ from 8 basic colors)
			System      = 0,
			//!  Set colour of just-replaced text
			Transparent = 1,
			Black       = 40,//!< color 1
			Red         = 41,//!< color 2
			Green       = 42,//!< color 3
			Yellow      = 43,//!< color 4
			Blue        = 44,//!< color 5
			Magenta     = 45,//!< color 6
			Cyan        = 46,//!< color 7
			White       = 47 //!< color 8
		};
    }

    //! \brief complete set of display properties for
    //! single character
    class DisplayStyle
    {
	private:
		
		/*!
		 * style described as an union
		 */		
		union
		{
			/*! 
			 * As single unsigned integer - for easy copying
			 */
			Uint32 style; 
			
			/*!
			 * And as a set of three separate variables, for easy manipulation
			 */
			struct
			{
				unsigned char fgColor; //!< foreground color
				unsigned char fgStyle; //!< foreground style
				unsigned char bgColor; //!< background color
			}properties;			
		};
    public:
		/*!
		  Set up specified style (parametrized constructor)
		  \param _fgColor
		  \param _fgStyle
		  \param _bgColor
		*/
		DisplayStyle(Fg::Color _fgColor,
					 Fg::Style _fgStyle,
					 Bg::Color _bgColor)throw();

		/*!
		  \param s - source to copy
		  
		  basic copy constructor - binary 1:1 copy.
		 */
		DisplayStyle(const DisplayStyle & s)throw();
		/*!
		  Nonparameter constructor sets colours default. default
		  values are implementation-specific (currently white on
		  green, but this may vary - maybe once upon the time we will
		  implement some special "undefined" values for all three
		  members of this class);
		*/
		DisplayStyle()throw();

		
		/*!
		 * \return foreground color
		 */
		Fg::Color GetFgColor()const throw()
			{
				return static_cast<Fg::Color>(properties.fgColor);
			}

		/*! 
		 * \return foreground style
		 */
		Fg::Style GetFgStyle()const throw()
			{
				return static_cast<Fg::Style>(properties.fgStyle);
			}

		/*! 
		 * \return nackground color
		 */
		Bg::Color GetBgColor()const throw()
			{
				return static_cast<Bg::Color>(properties.bgColor);
			}

		/*! 
		 * Set foreground color
		 *
		 * \param col new color
		 */
		void SetFgColor( const Fg::Color col )throw()
			{
				properties.fgColor=static_cast<unsigned char>(col);
			}
		
		/*!
		 * Set foreground style
		 *
		 * \param s new style
		 */
		void SetFgStyle( const Fg::Style  s  )throw()
			{
				properties.fgStyle=static_cast<unsigned char>(s);
			}
		
		/*! 
		 * Set background color
		 *
		 * \param col new color
		 */
		void SetBgColor( const Bg::Color col )throw()
			{
				properties.bgColor=static_cast<unsigned char>(col);
			}

		

		
		/*!
		  \param other    
		  (fgColor==other.fgColor) && (fgStyle==other.fgStyle) && (bgColor==other.bgColor)
		*/
		bool operator==(const DisplayStyle & other)
			{
				return style==other.style;
			}

		/*!
		  \param other
		  (fgColor!=other.fgColor) || (fgStyle!=other.fgStyle) || (bgColor!=other.bgColor)
		*/
		bool operator!=(const DisplayStyle & other)
			{
				return style!=other.style;
			}

		/*!
		 * \param other style, whose content will be assigned to this
		 *
		 * Copy-assignment operator
		 */
	    Scr::DisplayStyle& operator=(const DisplayStyle & other)
			{
				style=other.style;
				return *this;
			}

	};

    
		
    //! \brief Class representing basic output operation is defined as ABC
    //! (abstract base)
    /*!
      Operations are performed using subroutines apropriate to output
      type. Note, that some implementations of Screen (i.e. remote
      ones) use spcific forms of compression to limit data transfer,
      other rather optimize CPU usage.
    */
    class Screen
    {
    private:
    protected:
		Screen()throw();
    public://exceptions may be thrown
		
		__DE(ConnectionError,Exception); // when failed to update
		// screen due to communication interrupt or missing client terminal 
		// capabilities

		__DE(TerminalTypeUnknown,ConnectionError);// client terminal is too primitive 
		// to support request.
		__DE(CursorVisibilityNotSupported,Exception);
			
		__DE(IllegalOperation,Exception);// each illegal operation
			
		__DE(RangeError,IllegalOperation);// range checking
		// failed
			
		__DE(IllegalCharacter,IllegalOperation);
			
		__DE(InvalidUTF8,IllegalCharacter);//any encoding, that
		//does not pass UTF-8
		//validation. 
		
		__DE(InvalidFirstByte,InvalidUTF8);//doesn't match any of standard
		//patterns (that is it doesn't match any of utf-8 sequences)
			
		__DE(InvalidTrailingByte,InvalidUTF8);
			
		__DE(OverlongUTF8Encoding,InvalidUTF8);// too many bytes
		// used to encode
		// specific unicode 
		// character.
			
		__DE(CharacterExceedsUTF8Range,IllegalCharacter);// char
		//code equal to or greater than 1<<21
			
		__DE(GotoOutOfRange,RangeError);//gotoYX illegal location
			
		__DE(SubscreenOutOfRange,RangeError);// too big subscreen
			
		__DE(PrintOutOfRange,RangeError);//printing where
		//illegal (out of array
		//boundaries)
			
		__DE(PrintOutOfHorizontalRange,PrintOutOfRange);
		__DE(PrintOutOfVerticalRange,PrintOutOfRange);

		__DE(SubscreenResize,IllegalOperation);//subscreen can not
		//be resized
		//__DE()
	
		//(does not move active point, fills screen 
		//withactive bg color)
		/*!
		  Fill whole screen with current background colour.

		  \note function does not operate on physical screen. Use
		  Refresh to see effect. 
		*/
		virtual void Clear()throw()                = 0;
		
		// following 3 functions sets active properties (used while
		// inserting new characters or clearing screen)

		/*!
		  \param col new background colour to be set
		  \return  nothing upon successful execution
	  
		  Function sets background colour. Background colour is of
		  type Bg::Color. Typical use example: <code>
		  myscreen.SetBgColor(Bg::Black) </code>.

		  Function is exception safe as it does not throw any exceptions.

		  \note thanks to overloaded operator <<,
		  something like <code> myscreen << Bg::Black </code>will also be valid and
		  will do exactly the same as above.
		*/
		virtual void SetBgColor(Bg::Color col)
			throw()                                = 0;

		/*!
		  \param col new foreground colour to be set
		  \return  nothing upon successful execution
	  
		  Function sets foreground colour. Background colour is of
		  type Bg::Color. Typical use example: <code>myscreen.SetFgColor(Fg::Red)</code>.

		  Function is exception safe as it does not throw any
		  exceptions.

		  \note thanks to overloaded operator <<,
		  something like <code>myscreen << Fg::Red</code> will also be valid and
		  will do exactly the same as above.
	  
		  \note colour is not only foreground property: Fg
		  style sets bright or dark variant of each colour, and it
		  doubles total number of availble colours.
		*/
		virtual void SetFgColor(Fg::Color col)
			throw()                                = 0;

		/*!
		  \param s new foreground text style to be set
		  \return  nothing upon successful execution

	  
		  Set foreground style (i.e. bright (bold) or dim
		  (regular)). Maybe once upon the time more styles will be
		  suppotred to utilise capabilities of more advanced terminal
		  types (such as blink and underline for DEC VT220), but for
		  now we don't specify this, as portability is one of primary goals
		  for our library
	   
		  Function is exception safe as it does not throw any
		  exceptions.	
		*/
		virtual void SetFgStyle(Fg::Style s)throw()= 0;
		
		// move active point
		/*!
		  \param y 
		  \param x  new coordinates of active point (please
		  remember the order of theese attributes)
	  
		  Change active point position (that is point, where writing
		  will start after invocation of AddText or AddCharacter.

		  Function throws exception
		  Scr::Screen::GotoOutOfRange when coordinates
		  exceed size of screen. After exception throw active 
		  position is undefined.

		  \sa SetFgColor # SetBgColor
		  \return  nothing upon successful execution
		*/
		virtual void GotoYX(Uint y,	Uint x)
			throw(GotoOutOfRange)      = 0;
		
		/*!
		  \param c character to be printed
		  \return  nothing upon successful execution

		  Print single low ascii character (for characters out of
		  basic 7-bit ascii set please use integer version of this
		  function and proper UNICODE codes of characters)
	  
		  \exception Scr::Screen::PrintOutOfRange  as for
		  AddText

		  \exception Scr::Screen::IllegalCharacter
		  negative signed (or over-127-unsigned) c supplied.
		*/
		virtual void AddCharacter(char c)// add single LOW ascii character
			throw(PrintOutOfRange,// (128-255 ascii codes
				  IllegalCharacter)     = 0;//are forbidden)
    
		/*!
		  \param c character to be printed
		  \return  nothing upon successful execution

		  Print single unicode character. 

		  \note what programmes supply as parameter is direct number
		  of character, not UTF-8 encoded version of it. UTF-8 may be
		  supplied using  AddText
	  
		  \exception Scr::Screen::PrintOutOfRange  as for
		  AddText

	  
		  \exception Scr::Screen::IllegalCharacter too
		  large value of c.
		*/
		virtual void AddCharacter(wchar_t c) // add single unicode
			throw(PrintOutOfRange,     //character
				  IllegalCharacter)     = 0;		
		
		//print something (need Refresh to see it), starting from
		//active point. move active point AFTER newly added text.
		/*!
		  \param text traditional null-terminated string in UTF-8 encoding. 
		  \return  nothing upon successful execution
	  
		  Adds specified text in position starting from active point
		  (see GotoYX). Moves active point just after the newly added
		  text irrespectively if this position is valid (so next text
		  will start just after it, always in the same line). Function
		  does not support line breaks.

		  As function supports UTF-8, it also requires string
		  to be valid UTF-8, so each character must be low ascii
		  (1-127) or multibyte.

		  \note function will not emit text to physical screen, unless
		  Refresh called afterwards
	  	  
		  \exception Scr::Screen::PrintOutOfRange is thrown if
		  initial position of active point is invalid, or if text is
		  too long (as function
		  does not support line breaks).<BR>	  
		  If the text ends exactly in last column of screen, active
		  point is set after it, in the same line, so is invalid, and
		  next trial of usage of this function (or any other
		  character-adding one) will fail with
		  Scr::Screen::PrintOutOfRange.

	  
		  \exception  Scr::Screen::IllegalCharacter will be
		  thrown if text supplied is not a valid UTF-8 string (even
		  "overlong sequences" will be considered illegal (according
		  to an apropriate RFC

		  \sa AddCharacter, Refresh, RFC 3629
		  \latexonly \index{RFC, reference to!3629}\endlatexonly	  
		*/
		virtual void AddText(const char * text)         
			throw(PrintOutOfRange,
				  IllegalCharacter)     = 0;

	
		
		/*!
		 * \param text as above but as std::string, not C-style string
		 * 
		 * exceptions: as above.
		 */
		virtual void AddText(const std::string & text)   
			throw(PrintOutOfRange,
				  IllegalCharacter)     = 0;
	
		/*!
		 * \param text wide UNICODE string  to be printed
		 * 
		 * \exception PrintOutOfRange is thrown if
		 * initial position of active point is invalid, or if text is
		 * too long (as function does not support line breaks).
		 *
		 * \exception IllegalCharacter will be
		 * thrown if text supplied is not a valid UTF-8 string (even
		 * "overlong sequences" will be considered illegal (according
		 * to an apropriate RFC
		 *
		 *  \copydoc AddText(const std::wstring & text)
		 */	
		virtual void AddText(const wchar_t * text)   
			throw(PrintOutOfRange,
				  IllegalCharacter)     = 0;

		/*!
		 *	\param text text to be printed
		 *
		 *  \sa Screen::AddText(const char * text) for extensive description
		 */	
		virtual void AddText(const std::wstring & text)   
			throw(PrintOutOfRange,
				  IllegalCharacter)     = 0;		
		
		/*!
		  \param text wide string
		  \param limitcols max width in columns

		  Function prints AT MOST limitcols wide string. Width means
		  number of columns, which is not the same thing as number of
		  characters, as most CJK glyphs are multicolumn.

		  \exception PrintOutOfRange is thrown if
		  initial position of active point is invalid, or if text is
		  too long (as function does not support line breaks).

		  \exception IllegalCharacter will be
		  thrown if text supplied is not a valid UTF-8 string (even
		  "overlong sequences" will be considered illegal (according
		  to an apropriate RFC

		  \sa Screen::AddText(const char * text) for extensive description
		 */	
		virtual Uint AddTextCols(const wchar_t * text, Uint limitcols)   
			throw(PrintOutOfRange,
				  IllegalCharacter)     = 0;

		/*!
		  \param text wide string
		  \param limitcols max width in columns

		  Function prints AT MOST limitcols wide string. Width means
		  number of columns, which is not the same thing as number of
		  characters, as most CJK glyphs are multicolumn.
		 */
		virtual Uint AddTextCols(const std::wstring & text, Uint limitcols)
			throw(PrintOutOfRange,
				  IllegalCharacter)     = 0;

		/*!
		  \param c ASCII character
		  \param n number of repetitions (length of line)

		  Function adds horizontal line of n characters c.
		 */
  		virtual void HorizontalLine(char c, Uint n)
  			throw(PrintOutOfRange, IllegalCharacter) = 0;

		/*!
		  \param c UNICODE character
		  \param n number of repetitions (length of line)

		  Function adds horizontal line of n characters c.
		 */
  		virtual void HorizontalLine(wchar_t c, Uint n)
  			throw(PrintOutOfRange,
  				  IllegalCharacter)	    = 0;

		/*!
		  \param c ASCII character
		  \param n number of repetitions (length of line)

		  Function adds verticel line of n characters c.
		 */
  		virtual void VerticalLine(char c, Uint n)
  			throw(PrintOutOfRange,
  				  IllegalCharacter)	    = 0;

		/*!
		  \param c UNICODE character
		  \param n number of repetitions (length of line)

		  Function adds vertical line of n characters c.
		 */
  		virtual void VerticalLine(wchar_t c, Uint n)
  			throw(PrintOutOfRange,
  				  IllegalCharacter)	    = 0;
				  
		/*!
		 * \param c character used to create rectangle
		 * \param s dimensions of rectangle 
		 *
		 * Function creates rectangle of characters. s specifies 
		 * number of rows and number of repetitions of character
		 * c in each row.
		 */
		virtual void Rectangle(char c, const Size & s)
			throw(PrintOutOfRange,
				  IllegalCharacter)    = 0;
		
		/*!
		 * \param c character used to create rectangle
		 * \param s dimensions of rectangle 
		 *
		 * Function creates rectangle of characters. s specifies 
		 * number of rows and number of repetitions of character
		 * c in each row.
		 */
		virtual void Rectangle(wchar_t c, const Size & s)
			throw(PrintOutOfRange,
				  IllegalCharacter)    = 0;
		
		//change screen size
		/*!
		  \param rows new number of rows (new height) of screen
		  \param cols new number of columns of screen
		  \return  nothing upon successful execution

		  Change the output size.

		  \note this function does not change size of physical screen,
		  it may only be used to resize this object to fit physical
		  screen size. If screen grows, new characters are filled with
		  current background colour.
		  \note Function does not refresh the physical screen after
		  it's resizing, so it's content is undefined after call of
		  this function (however left-top part of it will be restored
		  after Refresh call).

		  \exception Scr::Screen::Exception::IllegalOperation if
		  particular screen may nor be resized for some
		  implementation- or platform- specific reasons. In particular
		  case primitive subscreens may not be resized
		  (SubscreenResize specialization of exception is thrown then).

	  
		*/
		virtual void Resize(Uint rows, Uint cols)
			throw(IllegalOperation)   = 0;

		/*!
		  \param p new cursor position

		  Force cursor position after finishing next refresh. If *this
		  is a subscreen, position (relative to *this) will be mapped
		  to the physical screen.

		  \note effective position after refresh will be position
		  set by last successful call to ForceCursorPosition
		 */
		virtual void ForceCursorPosition(Position p)throw(RangeError) = 0;
			

		/*!
		  make cursor invisible
		 */
		virtual void HideCursor()throw(CursorVisibilityNotSupported) = 0;

		/*!
		  make it visible again
		 */
		virtual void ShowCursor()throw(CursorVisibilityNotSupported) = 0;
		
		//update real screen
		/*!
		  \return  nothing upon successful execution
	  
		  Rewrite internal buffers to physical screen. When writing
		  complex, multi-layer items, it is
		  recommended to call this function after finishing writing
		  everything. When small changes need to be displayed, it may
		  be called every single AddCharacter, as it can't be a very
		  expansive operation in terms of CPU or transfer usage
		  (remote implementations will be optimized for transfer,
		  while local will be writen to achieve best performance for
		  specific terminal).
		*/
		virtual void Refresh()
			throw(ConnectionError)    = 0;

		/*!
		  \param _y_offset vertical offset from top edge of this
		  screen to top edge of new SubScreen.
		  \param _x_offset horizontal offser
		  \param _h height
		  \param _w with
		  \return  pointer to new SubScreen (programmer will have to free it's
		  resources to prevent memory leak and other errors).

		  \exception Scr::Screen::SubscreenOutOfRange
		  is thrown when too big subscreen requested or inapropriate
		  position specified
		*/
		virtual Screen *
		CreateSubScreen(Uint _y_offset,
						Uint _x_offset, Uint _h,
						Uint _w)throw(SubscreenOutOfRange) = 0;

	

		//!
		//!\return current type of terminal
		//!
		virtual const char * GetType() const throw(TerminalTypeUnknown) = 0;

		
		//retrieve settings
		/*!
		  \return vertical offset of active point
		*/
		virtual Uint GetY() const throw()= 0;
		/*!
		  \return horizontal offset of active point
		*/
		virtual Uint GetX() const throw()= 0;
		/*!
		  \return Current height of screen
		*/
		virtual Uint GetHeight() const throw()= 0;
		/*!
		  \return Current width of screen
		*/
		virtual Uint GetWidth() const throw()= 0;
		
		/*!
		   \return true if cursor is visible, false if it ishidden
		   \sa ShowCursor HideCursor
		 */
		virtual bool GetCursorVisibility() const throw()= 0;
		
		virtual ~Screen()throw();
		
    };
    
    //! Class representing basic input and output operations
    /*!
      Class is implemented and designed as base class for any specific
      application. It controls directly screen size, and specifies
      event interface for reacting keyboard and screen connected
      events. It is designed to be platform-transparent, so programmer
      does not have to bother OS specific method of checking window
      size, key value etc. 

      OnEvent actions are defined as virtual member functions
    */
    class Connection
    {
    private:
    protected:
		std::auto_ptr<Screen> screen;
		Connection(std::istream & _input, std::ostream & _output)throw();
    public:

		__DE(StartFailed,Exception);
		__DE(StopFailed,Exception);
		typedef StopFailed ExitFailed; // type name alias (the
		                              // same exception)
		__DE(AlreadyStopped,StopFailed); 
		__DE(Broken,StartFailed);

		__DE(IllegalClientAction,StartFailed);
		
		__DE(IllegalTelnetAction,IllegalClientAction);
		
		__DE(IncorrectSubnegotiation,IllegalTelnetAction); 
		__DE(IncorrectWindowSizeSubnegotiation,IncorrectSubnegotiation); 
		__DE(IncorrectTerminalTypeSubnegotiation,IncorrectSubnegotiation); 
		
		__DE(UnsupportedClientFeature,IllegalClientAction); 
		__DE(UnsupportedKey,UnsupportedClientFeature);
		__DE(UnsupportedTelnetFeature,UnsupportedClientFeature); 

		__DE(AlreadyRunning,StartFailed); 
		__DE(NotYetStarted,StopFailed); 
		/*!
		  \return result of whole connection. If broken, the result is
		  1. Else the result is argument passed to Exit(int)

	  
		  Start connection (with no arguments - they must be set with
		  application specific methods defined by
		  programmer). Function blocks execution of thread up to
		  finish of connection.

		  \exception Scr::Connection::AlreadyRunning when connection has
		  already been started (one execution thread per class
		  instance allowed) and hasn't yet been stopped.
		  \exception Scr::Connection::Broken is thrown
		  when connection is broken (i.e. input/output error occured)
	  
		  \exception Scr::Connection::FailedToStart when
		  connecction can not be estabilished for some reason. 
		  \note as Start() is defined in way, that allows it to throw
		  only one exception class and all OnEvent functions do not
		  allow any exceptions, all of them must be handled within
		  exception handling function. Unexpected exception handler
		  will be used otherwise.
		*/
		virtual int Start()
			throw(StartFailed,Screen::IllegalCharacter); // nonvirtual;

		/*!
		  \param argc number of arguments
		  \param argv C-style array of arguments

		  Start connection. \a argv can be parsed in inheritting classes.

		  \sa \a Start() for detailed info
		*/
		virtual int Start(int argc, char **argv)
			throw(StartFailed,Screen::IllegalCharacter); // nonvirtual;

		/*!
		  \return nothing
		  \param code this will be the result of ongoing Start()

		  If connection is currently running (that means, Start()
		  member function of specific object is running) Exit tells it
		  to break as soon as possible, call OnExit() and return code
		  given.

		  \exception Scr::Connection::AlreadyStopped
		  exception is thrown when  Exit was already called, but
		  connection wasn't stopped yet.
		  \exception Scr::Connection::NotYetStarted is
		  thrown when connection was already stopped or hasn't yet
		  been started.
		*/
		void Exit(int code)throw(StopFailed); // finishes connection
		// Start() returns code passed to Exit() or 1 if other cause
		// of finishing connection
		

		
		//event "callback" functions
		virtual void OnStart()throw();
		virtual void OnResize(Uint rows, Uint cols)
			throw();//              new height           new width
		virtual void OnKeyDown(Key key) // ascii code or
			throw();                              // special code.
		//!
		//! \param code exit code. Will be returned by Start just
		//! after finish of app.
		//!
		virtual void OnExit(int code)throw(); // last actions of program
		//(called by destructor; while OnExit called, screen may not
		//be used (it is already destroyed)
		virtual ~Connection()throw();
		
		friend class Screen;
		//friend class LocalScreen;
    };//class Connection

    //! namespace containing iomanipulator-like items
    namespace Control
    {
		
		/*!
		  This is ,,private'' class of system. It is only designed as
		  a return type of Scr::Control::GotoYX(Uint , Uint) - 
		  simmilar idea to std::_Setw (as return type of
		  std::setw(int)).
		*/
		class _PositionYX:public Position
		{
		public:
			_PositionYX(Uint _row,Uint _col):
				Position(_row,_col){;}
		};

		/*!
		  \param _y row on screen
		  \param _x column on screen
		  
		  Controlling screen active point position (the point, where
		  text starts).
		  FooScreen << Scr::Control::GotoYX(3,4) is an
		  direct equivalent of FooScreen.GotoYX(3,4).
		*/
		_PositionYX GotoYX(Uint _y, Uint _x);
		

		/*!
		  Special one-element type introduced only for Refresh manipulator
		*/
		enum _Refresh {
			/*!
			  This manipulator forces refreshing of screen.
			  FooScreen << Scr::Control::Refresh is an
			  direct equivalent of FooScreen.Refresh().
			*/
			Refresh
		};

		/*!
		  Special one-element type introduced only for Clear manipulator
		*/
		enum _Clear {        
			/*!
			  This manipulator clears whole screen. 
			  FooScreen << Scr::Control::Clear is an
			  direct equivalent of FooScreen.Clear().
			*/
			Clear
		};

    } // namespace Control
    

	
//	Screen& operator<<(Screen & screen,const EString & whatto);
	Screen& operator<<(Screen & screen,const std::wstring & whatto);
	Screen& operator<<(Screen & screen,wchar_t const * const & whatto);
	Screen& operator<<(Screen & screen,wchar_t * const & whatto);

	Screen& operator<<(Screen & screen,const std::string & whatto);
	Screen& operator<<(Screen & screen,char const * const & whatto);
	Screen& operator<<(Screen & screen,char * const & whatto);

	Screen& operator<<(Screen & screen,const Fg::Color & whatto);
	Screen& operator<<(Screen & screen,const Fg::Style & whatto);
	Screen& operator<<(Screen & screen,const Bg::Color & whatto);
	Screen& operator<<(Screen & screen,const Control::_PositionYX & whatto);
	Screen& operator<<(Screen & screen,const Control::_Refresh & whatto);
	Screen& operator<<(Screen & screen,const Control::_Clear & whatto);
	Screen& operator<<(Screen & screen,const DisplayStyle & whatto);
	
	Screen& operator<<(Screen & screen,unsigned int  whatto);

	
	Screen& operator<<(Screen & screen,int  whatto);
	Screen& operator<<(Screen & screen,std::_Setw  whatto);

	Screen& operator<<(Screen & screen,unsigned long  whatto);
	Screen& operator<<(Screen & screen,long  whatto);
	Screen& operator<<(Screen & screen, char  whatto);
	Screen& operator<<(Screen & screen, wchar_t  whatto);

}

#endif
