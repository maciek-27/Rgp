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

#ifndef __COMMONS_H__
#define __COMMONS_H__

#include <tr1/memory>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <rexio/trace.h++>

// boost implements new std. library functions
// - it may be useful if tr1/memory is not availble on specific platform
//
// #include<boost/shared_ptr.hpp>
// 
// namespace std
// {
// 	namespace tr1=boost;// alias to implementations of C++0x Technical Report 1
// 						// defined interfaces
// }

namespace Scr {
    //! Machine specific unsigned integer. Type of at least 32 bits.
    typedef unsigned long Uint; 
	//! Maximal value of Uint type
	const Uint UintMax = -1;
    //! Machine specific signed integer. Type of at least 32 bits.
    typedef long Sint; 
	//! Maximal value of Sint type
	const Uint SintMax = UintMax/2;
	//! Minimal value of Sint type
	const Uint SintMin = -SintMax-1;

	typedef int               Sint32;
	typedef unsigned int      Uint32;

/* ISO C++ does not define long long, however it is practical*/
# if defined __x86_64__
	typedef unsigned long int Uint64;
	typedef long int          Sint64;
# else
	typedef
	unsigned long long int    Uint64;
	typedef long long int     Sint64;
# endif

	//! vector container
	struct Vector
	{
		/*!
		  \param _rows rows offset
		  \param _cols cols offset

		  Simple constructor for convenient initialization and
		  creation.
		*/
		Vector(Sint _rows, Sint _cols);
		
		//! offset in rows
		Sint rows;
		//! offset in columns
		Sint cols;
	};

	//! size container
	struct Size
	{
		/*!
		  \param _height height
		  \param _width width
		  
		  Simple constructor for convenient initialization and
		  creation.
		*/
		Size(Uint _height, Uint _width);

		/*!
		  height property
		*/
		Uint height;
		/*!
		  width property
		*/
		Uint width;
	};

	//! position container.
	struct Position
	{
		/*!
		  \param _row row position
		  \param _col col position

		  Simple constructor for convenient initialization and
		  creation.
		*/
		Position(Uint _row,Uint _col);
		/*!
		  \param pos addition target 

		  Simple addition.
		*/
		Position operator+(const Position& pos);
		/*!
		  \param size size to increment by

		 Result of incrementing position by a size of some object.
		*/
		Position operator+(const Size& size);
		/*!
		  \param vec vector to add

		  Result of modificating position by a vector.
		*/
		Position operator+(const Vector& vec);
		/*!
		  \param pos addition target

		  Simple assignment by addition.
		*/
		Position& operator+=(const Position& pos);
		/*!
		  \param size size to increment by

		  Incrementation of position by a size of some object.
		*/
		Position& operator+=(const Size& size);
		/*!
		  \param vec vector to add

		  Modification of position by a vector.
		*/
		Position& operator+=(const Vector& vec);
		/*!
		  \param pos substraction target

		  Simple substraction.
		*/
		Position operator-(const Position& pos);
		/*!
		  \param size size to decrement by

		 Result of decrementing position by a size of some object.
		*/
		Position operator-(const Size& size);
		/*!
		  \param vec vector to substract

		  Result of modificating position by a vector.
		*/
		Position operator-(const Vector& vec);
		/*!
		  \param pos substraction target

		  Simple assignment by substraction.
		*/
		Position& operator-=(const Position& pos);	   
		/*!
		  \param size size to decrement by

		  Decrementation of position by a size of some object.
		*/
		Position& operator-=(const Size& size);
		/*!
		  \param vec vector to substract

		  Modification of position by a vector.
		*/
		Position& operator-=(const Vector& vec);

		
		/*!
		  row number
		*/
		Uint row;
		/*!
		  column number
		*/
		Uint col;				
	};

	//! Standard comparison operator
	inline bool operator!=(const Scr::Position & p1,const Scr::Position & p2)
	{
		return p1.col!=p2.col or p1.row!=p2.row;
	}
	
	//! Standard comparison operator
	inline bool operator==(const Scr::Position & p1,const Scr::Position & p2)
	{
		return p1.col==p2.col and p1.row==p2.row;
	}


	
    //! \brief base class for exceptions thrown by library
    //! objects.
	/*!
	  exception holds message about conditions etc, where it was thrown
	 */
    class Exception:  public std::exception
    {
    private:
		/*!
		  message passed as reference counting pointer to prevent
		  resource waste during throw-catch sequence.
		*/
		std::tr1::shared_ptr<std::string> message;
    public:
		/*!
		  \param _m message associated w/ exception. i.e. brief
		  description of situation. Will be displayed after program
		  failure.
		  
		  Only argument is exception reason.
		*/
		Exception(std::string _m)throw() ;
		/*!
		  \param _base exception to copy (copy constructor is used
		  widely during throw-catch sequence.
		  
		*/
		Exception(const Exception& _base)throw();//copy ctor.

		/*!
		  what() derrivated from std::exception: informs on reason of exception
		*/
		virtual const char* what() const throw();
		/*!
		  destructor conditionally frees resources (thanks to smart
		  pointer used). 
		*/
		virtual ~Exception()throw() ;
    };
    

/*!
  Macro to simplify defining of exception hierarchy. To define insitu
  new exception just add __DE(MoreSpecificException,MoreGeneralOne)
 */
#define __DE(CHILD,PARENT) class CHILD: public PARENT	\
    {public:											\
		CHILD(const CHILD & _b)throw():PARENT(_b){;}	\
		CHILD(std::string _m)throw():PARENT(_m){;}		\
    }

	// always outside of throw specification, always causes abort,
	// and therefore may be useful while you want to explicitly stop
	// application on certain condition
	__DE(FatalException,Exception);

	// also outside of throw spec., Used in assertion: exception may be 
	// thrown only if programmer is mistaken.
	__DE(LogicError,Exception);
}

#include <rexio/glyphwidth.h++>

#endif // __COMMONS_H__
