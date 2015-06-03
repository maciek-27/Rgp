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

#ifndef __TRACE_H__
#define __TRACE_H__
#include <iostream>

//#define DEBUG


#ifdef NDEBUG
# ifdef DEBUG
#  undef DEBUG
# endif
#endif

#ifdef DEBUG
# ifndef LOGGING_LEVEL
#  define LOGGING_LEVEL LogLevelModerate
# endif
#endif		

namespace Scr
{
	/*!
	 * Logging levels acceptable for RexIOLog macro
	 */
	enum{LogLevelQuiet, LogLevelLow, LogLevelModerate,
	LogLevelVerbose};
}

//!
//! RexIOLog macro prints log message if adequate log level is set. it does 
//! not print anything otherwise. it is    primarily used for debugging RexIO
//! library itself. 
//!
#ifdef DEBUG
# define RexIOLog(level) if(level<=LOGGING_LEVEL) std::clog
#else
# define RexIOLog(level) if (0) std::clog
#endif		

#endif

