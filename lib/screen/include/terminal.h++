#ifndef __TERMINAL_H__
#define __TERMINAL_H__
#include "screen.h++"
#include "vt100compatible.h++"

namespace Scr
{

	/*!
	  \brief  base class containing data fields typical to any
	  terminal output type
	 */
	class Terminal
	{
	protected:
		

		/*!
		  Coordinates of cursor onscreen
		*/
		struct
		{
		    Uint x;//!< column
		    Uint y;//!<row
		} termCoords;
		/*!
		  Copy of expected screen contents - used to optimise
		  Refresh() for transfer
		*/
		ScreenBuffer copyBuffer;
	public:
		Terminal()throw();
	};
}

#endif
