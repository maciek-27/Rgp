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

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
#include "commons.h++"
namespace Scr
{
	/*!
	  \brief Class represents key (or key combination) pressed on client
	  terminal. 
	 */
	class Key
	{
	private:
		//Alt+letter or Esc,than letter
		static const Uint     altMask = 0x00400000;

		//Control+letter
		static const Uint controlMask = 0x00200000;
		
		//!special key pressed
		static const Uint specialMask = 0x56000000;

		//basic key mask
		static const Uint basicKeyMask= 0x0000007f;
		Uint key;
	public:
		
		/*!
		 * Special ascii keys as teletype mnemonics. Please note, that this 
		 * enum is temporary, and will be deprecated in 1.1
		 *
		 */
		enum ASCII
		{
			LF   = 0xa,
			CR   = 0xd,
			EoF  = 0x4			
		};
		
		/*!
		 * Special key names. May be used for comparizons against key object
		 * (please refer to handbook for use example)
		 *
		 */
		enum Special
		{
			Escape  = 0x1b,
			Tab     = 0x561f0000, // on most systems: shift+tab
			BackTab,
			F1,
			F2,
			F3,
			F4,
			
			F5,
			F6,
			F7,
			F8,

			F9,
			F10,
			F11,
			F12,
//shifted
			F13,
			F14,
			F15,
			F16,
			
			F17,
			F18,
			F19,
			F20,
			
			F21,
			F22,
			F23,
			F24,
//??			
			F25,
			F26,			
			F27,
			F28,
			
			F29,
			F30,			
			F31,
			F32,
			
			F33,
			F34,
			F35,
			F36,
			
			Up,
			Down,
			Right,
			Left,

			CtrlUp,
			CtrlDown,
			CtrlRight,
			CtrlLeft,

			Enter,
			Delete,
			Backspace,

			Insert,
			Home,
			PageUp,
			PageDown,
			End
		};

		__DE(NotABasicKey,Exception);
		__DE(NotASpecialKey,Exception);

		Key():key(0){;}
		
		/*!
		 * \param key unsigned integer form
		 *
		 * This constructor allows implicit conversion between two forms of key
		 */
		Key(Uint key)throw()
			{
				this->key = key;
			}

		/*!
		 * implicit or
		 * static cast operator
		 */
		operator Uint()throw()
			{
				return key;
			}

		/*!
		 * If represents plain ascii character, function returns true.
		 * false is returned otherwise
		 */
		bool IsABasicKey()throw()
			{
				return
					( key >= ' ' )
					&& ( (key&0xff) <=127)
					&& ((key & specialMask) != specialMask);
			}
		
		/*
		 * Function returns true if key is a special key (that means return, 
		 * one of arrows, function key, delete etc.
		 */
		bool IsASpecialKey()throw()
			{
				return	(key < ' ') or
						((key & specialMask) == specialMask);
			}
		
		/*! 
		 * as if it was a letter A-Z
		 */
		char GetBasicKey()throw(NotABasicKey);
		
		//! 
		Special GetSpecialKey()throw(NotASpecialKey);

		const char * GetKeyName()throw();		
	};
}

#endif
