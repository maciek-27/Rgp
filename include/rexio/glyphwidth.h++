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

#ifndef __GLYPHWIDTH_H__
#define __GLYPHWIDTH_H__

#include <wchar.h>
#include <rexio/commons.h++>
#include <bitset>

namespace Scr {
	
	
	/*! Singleton class. */
	class GlyphWidth {
/*!
  Bitset for caching the width results. 2 bits per glyph.
  Note: the bitset gets reasonably fast only in the Release build
*/
		static std::bitset<(1<<17)*2> glyphWidth;
		GlyphWidth();
	public:
		__DE(NotPrintable, Exception);

		/*!
		  \arg ch

		  \return width of unicode character (0 or 1 or 2), that means number of 
		  cells in console, it needs to fit. 
		*/
		static Uint Get(wchar_t ch) {
			static GlyphWidth g;

			if((Uint)ch >= glyphWidth.size()/2) {		
				// covers the > 17 bits 2 column with.
				if((ch >= 0x20000 && ch <= 0x2fffd) ||
				   (ch >= 0x30000 && ch <= 0x3fffd))
					return 2;
				/* the following condition is a condition for 0 width
				   characters, taken from the wcwidth.c and containing only ranges
				   above 17 bits. */
				else if( ( ch >= 0xE0001 && ch <= 0xE0001 ) || ( ch >= 0xE0020 && ch <= 0xE007F ) ||
						 ( ch >= 0xE0100 && ch <= 0xE01EF ))
					return 0;
				else
					return 1;
			}
			
			// one lookup for 1 width characters, 2 for the others
			return glyphWidth[(ch<<1)] ? 1:2*glyphWidth[(ch<<1) + 1];
		}
	};
	
	/*!
	 * Computes width of unicode character (0 or 1 or 2), that means number of 
	 * cells in console, it needs to fit. Furthermore, it returns -1 if a character
	 * is a non-printable one.
	 */
	inline unsigned long width(wchar_t c)
	{
		return GlyphWidth::Get(c);
	}
	//__attribute__ ((deprecated));
}

#endif // __GLYPHWIDTH_H__
