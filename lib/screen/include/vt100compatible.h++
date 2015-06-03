#ifndef __VT100COMPATIBLE_H__
#define __VT100COMPATIBLE_H__
#include "screen.h++"

#include "terminal.h++"

namespace Scr
{
    //! terminal compatible w/ DEC VT-100 
    /*!
      This class provides full implementation of Scr::Screen abstract
      interface in terms of capabilities of DEC VT100 compatible
      terminals.
	  It will be used as fallback implementation when terminfo
      database is not availble
     */
    class VT100Compatible:public virtual GenericScreen, public Terminal
    {
    private:

		void RealGotoYX(const Position & p)throw(ConnectionError);
		
	protected:
		/*!
			Minimum implementation supportingonly 12 basic functionkeys, arrows 
		    and few special, in several formats of VT100-like terminal emulators. 		 
		 */
		virtual Key DecodeKeyPressed()
		throw(Connection::UnsupportedKey,Screen::InvalidUTF8);
	public:
		explicit VT100Compatible(std::istream & _input,std::ostream & _output)throw();
		
		/*!
		  Full support for colour and refreshing algorithm optimized
		  for transfer
		*/
		virtual void Refresh()throw(ConnectionError);

		/*!
		  \param rows
		  \param cols
	  
		  differs from Scr::GenericScreen::Resize only by the fact,
		  that it supports copyBuffer
		*/
		virtual void Resize(Uint rows, Uint cols)
			throw();
		virtual void CleanUp()  throw(ConnectionError);
		virtual ~VT100Compatible()throw();
    };
}

#endif
