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

/*! \file throw.h++
  \brief Useful macros for exception handling.
  
  __WHERE_AM_I__ by Curtis Krauskopf; see whole article:
  http://www.decompile.com/cpp/faq/file_and_line_error_string.htm
*/
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

//! file name and line number as plain string
#define __WHERE_AM_I__ "in " __FILE__ ":" TOSTRING(__LINE__) 
//! throw exception x with __WHERE_AM_I__ as constructor argument
#define THROW(x) throw x(__WHERE_AM_I__)

//! throw exception when assertion evaluates false
#define EASSERT(assertion,exception)			\
	if (!(assertion))THROW(exception)

//! throw exception, that has specific parameters
#define THROWP(x,p) throw x(std::string(__WHERE_AM_I__)+'\n'+(p))

//! if assertion false, THROWP
#define EASSERTP(a,e,p) if (!(a))THROWP(e,p)
