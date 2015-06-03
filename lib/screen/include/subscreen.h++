#ifndef __SUB_SCREEN_H__
#define __SUB_SCREEN_H__
#include"screenbase.h++"
#include"genericscreen.h++"

namespace Scr
{
	class GenericScreen;
	
    /*!
      Subscreen may be considered a specified region of screen limited
      to one rectangle. Subscreen does not provide it's own buffer, so
      it can be used as range for specific procedure rather than a
      layer. It allows all actions, but limited to it's width and
      height. It is useful for implementing procedures drawing
      specific features, i.e. widgets in UI toolkit.

      Strict range limitation is achieved by disabling of Scr::SubScreen::Resize
      member function
	*/
    class SubScreen:public ScreenBase
    {
    protected:
		/*!
		  reference to parent screen
		*/
		GenericScreen & parent;
		/*!
		  vertical distance from top of containing
		  (parent) screen to top of this
		   and horizontal  distance from its left edge.
		*/
		Position offset;

		/*!
		  Width and height of screen
		 */
		Size s;
		
		/*!
		 * Call GotoYX for parent. Rethrow possible exception as
		 * Printing exception.
		 */		
		inline void ParentGotoYXForPrinting()throw(PrintOutOfRange);
	public:
		/*!
		  \param _parent reference to parent screen
		  \param _y_offset vertical distance from top of containing
		  (parent) screen to top of this
		  \param _x_offset horizontal  distance from left edge of containing
		  (parent) screen to left edge of this
		  \param _h height
		  \param _w width
		*/
		SubScreen(GenericScreen & _parent,
				  Uint _y_offset,
				  Uint _x_offset,
				  Uint _h,
				  Uint _w)throw();

		/*!
		  Fills rectangle defined by this subscreen with current
		  background color, directly on containing buffer (so it may
		  be later hidden by containing buffer)
		*/
		virtual void Clear()throw();

		/*!
		  \param col color
		  
		  Subscreen does not have it's own DisplayProperties, so it
		  calls SetBgColor for parent screen
		*/
		virtual void SetBgColor(Bg::Color col)throw();
		/*!
		  \param col color
		  
		  Subscreen does not have it's own DisplayProperties, so it
		  calls SetFgColor for parent screen
		*/
		virtual void SetFgColor(Fg::Color col)throw();
		/*!
		  \param s style
		  
		  Subscreen does not have it's own DisplayProperties, so it
		  calls SetFgStyle for parent screen
		*/
		virtual void SetFgStyle(Fg::Style s)throw();

		/*!
		  \param x
		  \param y
		  
		  this does not access directly to parent window, as SubScreen
		  has it's own YX coordinates
		*/
		virtual void GotoYX(Uint y, Uint x)
			throw(GotoOutOfRange);

		/*!
		  \param text
		  
		  Print text directly on parent buffer
		  \note it means, that first appropriate GotoYX must be called
		  for parent, so it modifies not only contents of buffer, but
		  also coordinates of its active point.
		*/
		virtual void AddText(const char * text)throw(PrintOutOfRange,
											 IllegalCharacter);

		/*!
		  \param text
		  
		  Same as above. 
		 */
		virtual void AddText(const std::string & text)
			throw(PrintOutOfRange,
				  IllegalCharacter);

		/*!
		  \param text 

		  Same as above
		*/
		virtual void AddText(const wchar_t * text)   
			throw(PrintOutOfRange, IllegalCharacter);	

		/*!
		  \param text 

		  Same as above, but UNICODE
		*/
		virtual void AddText(const std::wstring & text)   
			throw(PrintOutOfRange, IllegalCharacter);
		
		
		virtual Uint AddTextCols(const wchar_t * text, Uint limitcols)   
			throw(PrintOutOfRange,
				  IllegalCharacter);

		virtual Uint AddTextCols(const std::wstring & text, Uint limitcols)   
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
		  \param c
		  
		  Print character directly on parent buffer
		  \note as for AddText, it modifies not only contents of buffer, but
		  also coordinates of its active point.
		*/
		virtual void AddCharacter(char c)throw(PrintOutOfRange);
		
		/*!
		  \param c
		  
		  Print UNICODE character directly on parent buffer
		  \note as for AddText, it modifies not only contents of buffer, but
		  also coordinates of its active point.
		*/
		virtual void AddCharacter(wchar_t c)throw(PrintOutOfRange,
									   IllegalCharacter);

		/*!
		  \param p position

		  mapped to parent
		 */
		virtual void ForceCursorPosition(Position p )throw(RangeError);			

		/*!
		  make cursor invisible
		 */
		virtual void HideCursor()throw(CursorVisibilityNotSupported);

		/*!
		  make it visible again
		 */
		virtual void ShowCursor()throw(CursorVisibilityNotSupported);

		/*!
		  force refresh of parent buffer
		*/
		virtual void Refresh()throw(ConnectionError);

		/*!
		  \param rows
		  \param cols
		  \exception Scr::Screen::SubscreenResize is
		  thrown always, as subscreen can not be resized
		*/
		virtual void Resize(Uint rows, Uint cols)
			throw(SubscreenResize);

		/*!
		  Return type of parent screen type (effectively it is the
		  type of underlying real screen)
		*/
		virtual const char * GetType() const throw(TerminalTypeUnknown);
		
		virtual Uint GetHeight() const throw();
		
		virtual Uint GetWidth() const throw();
		
		virtual bool GetCursorVisibility() const throw();
		
		virtual Screen * 
		CreateSubScreen(Uint _y_offset, Uint _x_offset, Uint _h,
						Uint _w)throw(SubscreenOutOfRange);
		
		~SubScreen()throw();
    };
}

/*!
  This particular macro is useful while implementing CreateSubScreen
  function. it does full range checking according to prototype
  declared in screen.h++
 */
#define SubScreenRangeCheck()										\
    /*throw exception on inproper position of subscreen	*/			\
    if (_x_offset>=GetWidth())										\
		THROW(SubscreenOutOfRange);									\
    if (_y_offset>=GetHeight())										\
		THROW(SubscreenOutOfRange); /* as theese are */				\
	/* unsigned integers, an  */									\
    /* exception is thrown also when _x_offset<0 or _y_offset<0  */	\
																	\
    /* throw exception on inproper size of subscreen	         */	\
    if (_x_offset+_w>GetWidth())									\
		THROW(SubscreenOutOfRange);									\
    if (_y_offset+_h>GetHeight())									\
		THROW(SubscreenOutOfRange);

#endif // __SUBSCREEN_H__
