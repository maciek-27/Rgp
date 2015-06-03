#ifndef __SCREENBASE_H__
#define __SCREENBASE_H__

#include "screen.h++"

namespace Scr
{
    //! \brief Implements features common to subscreen and
    //! generic screen
    class ScreenBase: public virtual Screen
    {
    protected:
		/*!
		  vertical and horizontal offset from the left edge of the screen
		*/
		Position  aPoint;
    public:
		ScreenBase();
	
		/*!
		  \return horizontal offset from the left edge of the screen
		*/
		Uint GetX()const throw();
		/*!
		  \return vertical offset from top of the screen
		*/
		Uint GetY()const throw();
		virtual void AddText(const std::string & text, Uint cols)         
			throw(PrintOutOfRange,
				  IllegalCharacter);
    }; // ScreenBase
} // Scr

#endif // __SCREENBASE_H__
