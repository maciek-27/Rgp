
#ifndef __GENERIC_SCREEN_H__
#define __GENERIC_SCREEN_H__
#include "screenbase.h++"
#include "utf8.h++"
#include "bufferedinput.h++"
#include "screenbuffer.h++"
#include "subscreen.h++"
#include <boost/thread/mutex.hpp>
#include <queue>
#include <vector>
namespace Scr
{
	
    //! Most basic implementation of whole Scr::Screen
    /*!
      This class provides generic implementation of large part of Scr::Screen
      interface, including basic output subroutines, but some of them
      lacks important platform-specific features
	*/
    class GenericScreen: public virtual ScreenBase
    {
    private:
		/*!
		 * Function used to compute width of text as well as width of each
		 * character. The function is designed to be called from within all 
		 * types of AddText
		 *
		 * \return width of string (correct value <= maxwidth)
		 *
		 * \param text is text, whose element widths need to be computed
		 * \param widths is C-type array of character widths, that need to be 
		 *   computed
		 * \param maxwidth is max width of whole text (if width of whole text 
		 *   exceeds allowed width, stop computation and throw exception)
		 *
		 * \exception Scr::Screen::RangeError exception is thrown when text is 
		 *   too wide.
		 *
		 * \exception Scr::Screen::IllegalCharacter exception is thrown when UNICODE 
		 *   encoding is incorrect (validation occurs only for _char_type=char)
		 */		
		template<typename _char_type>
		Uint PrecomputeTextCharsWidth(_char_type * text, std::vector<char>& widths, 
				Uint maxwidth)
			throw(RangeError, IllegalCharacter);
									  //please note, that this template is 
									  //implemented in genericscreen.c++
									  //and only there may be called with
									  //any _char_type
		
    protected:
		/*!
		  mut-ex for synchronous refresh operations
		 */
		boost::mutex refreshMutex;
		/*!
		  buffer used to implement all textual operations. All Add*
		  functions operate on it directly.
		*/
		ScreenBuffer controlBuffer;
		/*!
		  current properties (set w/ SetBg/FgColor/Style)
		*/
		DisplayStyle properties;

		/*!
		  cursorPosition
		 */
		Position cursorPosition;

		static const Uint cursorForced  = 1;
		static const Uint cursorVisible = 2;
		mutable Uint cursorFlags;

		BufferedInput input;
		
		/*!
		  Output file stream for writing
		*/
		std::ostream & output;
		
		/*!
		  get key esc-code from std input stream. decode it into form
		  from keyboard.h++
		*/
		virtual Key DecodeKeyPressed()
		throw(Connection::UnsupportedKey,Screen::InvalidUTF8);

		virtual Key DecodeBasicKeyPressed()throw(Screen::InvalidUTF8);
	
	public:

		/*!
		 * \param _input 
		 * \param _output
		 *
		 * GenericCcreen operates on C++ standard iostream.
		 */
		GenericScreen(std::istream & _input,std::ostream & _output)throw();
		/*!
		  empty controlBuffer
		*/
		virtual void Clear()throw();

		/*!
		  \param col new background colour to be set
		  \return  nothing upon successful execution

		  
		  Function operates on properties member object.
	  
		  Refer to manual for base class for action description.
		*/
		virtual void SetBgColor(Bg::Color col)throw();

		/*!
		  \param col new foreground colour to be set
		  \return  nothing upon successful execution
	  
		  Function operates on properties member object.
	  
		  Refer to manual for base class for action description.
		*/
		virtual void SetFgColor(Fg::Color col)throw();

		/*!
		  \param s new foreground text style to be set
		  \return  nothing upon successful execution

		  Function operates on properties member object.
	  
		  Refer to manual for base class for action description.
		*/
		virtual void SetFgStyle(Fg::Style s)throw();

		/*!
		  \param y 
		  \param x  new coordinates of active point (please
		  remember the order of theese attributes)
	  
		  Operates on coordinate values inherited from ScreenBase
		*/
		virtual void GotoYX(Uint y, Uint x)     
			throw(GotoOutOfRange);

		/*!
		  \param c 

		  Operates on controlBuffer and  coordinate values inherited from ScreenBase
		*/
		virtual void AddCharacter(char c)throw(PrintOutOfRange);
		
		/*!
		  \param c 
		  
		  Operates on controlBuffer and  coordinate values inherited from ScreenBase
		*/
		virtual void AddCharacter(wchar_t c)
			throw(PrintOutOfRange, IllegalCharacter);

		/*!
		  \param p position
		  
		  visible after refresh
		 */
		virtual void ForceCursorPosition(Position p )throw(RangeError);
				
		/*!
		 * \param text text to be printed (as C string)
		 * \copydoc Screen::AddText(const char *)
		 *
		 * \note Operates on controlBuffer and  coordinate values inherited
		 * from ScreenBase
		 */
		virtual void AddText(const char * text)throw(PrintOutOfRange,
											 IllegalCharacter);

		/*!
		 * \param text what to be printed (as C++ string)
		 * \copydoc Screen::AddText(const std::string &)
		 * \note Operates on controlBuffer and  coordinate values inherited
		 * from ScreenBase
		 */
		virtual void AddText(const std::string & text)   
			throw(PrintOutOfRange,
				  IllegalCharacter);

    	/*!
		 * \param text UTF-8 encoded character string
		 * \param cols length of string
		 * \param widths widths of subsequent characters
		 *
		 * Function prints specified text assuming, that its width is EXACTLY 
		 * specified by cols parameter
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
		 * \note function is NOT a part of Scr::Screen interface, and is not 
		 * accessible outside of screen module
		 *
		 * \sa Screen::AddText(const char * text) for extensive description
		 */
		virtual void AddText(const char * text, Uint cols, 
				const std::vector<char> & widths)         
			throw(PrintOutOfRange, IllegalCharacter);
		
		/*!
		 * \param text 
		 *
		 * Operates on controlBuffer and  coordinate values inherited from ScreenBase
		*/
		virtual void AddText(const std::wstring & text)   
			throw(PrintOutOfRange, IllegalCharacter);
		
		/*!
		 * \param text 
		 *
		 * Operates on controlBuffer and  coordinate values inherited from ScreenBase
		 */
		virtual void AddText(const wchar_t * text)   
			throw(PrintOutOfRange, IllegalCharacter);

		/*!
		 * \copydoc Screen::AddTextCols(const wchar_t *, Uint)
		 * \note Operates on controlBuffer and  coordinate values inherited
		 * from ScreenBase.
		 */
		virtual Uint AddTextCols(const wchar_t * text, Uint limitcols)
			throw(PrintOutOfRange, IllegalCharacter);
		// Doxygen complains about 'not defined parameters'. they are defined, 
		// but in Scr::Screen (and \copydoc copies 'em)		
		
		/*!	  
		 * \copydoc Screen::AddTextCols(const wchar_t *, Uint)
		 * Operates on controlBuffer and  coordinate values inherited from ScreenBase
		 */
		virtual Uint AddTextCols(const std::wstring & text, Uint limitcols)
			throw(PrintOutOfRange, IllegalCharacter);

		/*!
		 * Function adds "text in subscreen", that is text, which was to be 
		 * be inserted in subscreen. This function is called by apropriate
		 * Scr::Subscreen::AddText . 
		 *
		 * \param text UTF-8 encoded text to be printed
		 * \param width maximum number of columns to be printed
		 *
		 * \exception Scr::Screen::IllegalCharacter may be thrown if any 
		 *   character of text is incorrectly encoded
		 *
		 * \exception Scr::Screen::PrintOutOfRange is thrown when text runs 
		 *   out of root screen range or when it's width (as number of 
		 *   columns, not characters) exceeds widthlimit.
		 */
		void AddSubscreenText(const char * text, Uint widthlimit)
			throw(PrintOutOfRange, IllegalCharacter);

		/*!
		 * Purpose of this function is as above, but one of parameters 
		 * slightly differs.
		 *
		 * \param text UNICODE text
		 * \param width maximum number of columns to be printed
		 *
		 * \exception Scr::Screen::PrintOutOfRange is thrown when text runs 
		 *   out of root screen range or when it's width (as number of 
		 *   columns, not characters) exceeds widthlimit.		 *
		 */
		void AddSubscreenText(const wchar_t * text, Uint widthlimit)
			throw(PrintOutOfRange, IllegalCharacter);
				
		// for following functions please refer to docmentation-comment 
		// in include/rexio/screen.h++
		virtual void HorizontalLine(char c, Uint n)
  			throw(PrintOutOfRange, IllegalCharacter);
		
  		virtual void HorizontalLine(wchar_t c, Uint n)
  			throw(PrintOutOfRange, IllegalCharacter);

		virtual void VerticalLine(char c, Uint n)
  			throw(PrintOutOfRange, IllegalCharacter);
		
  		virtual void VerticalLine(wchar_t c, Uint n)
  			throw(PrintOutOfRange, IllegalCharacter);

		virtual void Rectangle(char c, const Size & s)
			throw(PrintOutOfRange, IllegalCharacter);
		
		virtual void Rectangle(wchar_t c, const Size & s)
			throw(PrintOutOfRange, IllegalCharacter);
				
		/*!
		  make cursor invisible
		 */
		virtual void HideCursor()throw(CursorVisibilityNotSupported);

		/*!
		  make it visible again
		 */
		virtual void ShowCursor()throw(CursorVisibilityNotSupported);
		
		/*!
		  Most basic implementation suitable really only for dumb
		  terminals or line printers: prints each line of buffer to
		  stdout. Created only for debugging reasons.
		*/	
		void Refresh() 
		// just a dumb proc to produce
		// basic debug printout
			throw(ConnectionError);

		virtual Screen *
		CreateSubScreen(Uint _y_offset,
						Uint _x_offset, Uint _h,
						Uint _w)throw(SubscreenOutOfRange);

		/*!
		  \return always throw exceptn
		*/
		virtual const char * GetType() const throw(TerminalTypeUnknown);
		
		/*!
		  \return height of controlBuffer
		*/	
		virtual Uint GetHeight() const throw();
		
		/*!
		  \return width of controlBuffer
		*/
		virtual Uint GetWidth() const throw();
		
		virtual bool GetCursorVisibility() const throw();
		
		/*!
		  Cleans screen up: restore default colours and clear (it is
		  good to use this function while finishing application etc.)
		*/
		virtual void CleanUp()  throw(ConnectionError);
		
		virtual void Resize(Uint rows, Uint cols)throw();
		
		~GenericScreen()throw();			
	
    };

}

#endif // __GENERICSCREEN_H__
