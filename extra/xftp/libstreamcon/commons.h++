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
// boost implements new std. library functions
#include <boost/shared_ptr.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <sys/time.h>

namespace std
{
	namespace tr1=boost;// alias to implementations of C++0x Technical Report 1
						// defined interfaces
}

namespace streamcon
{

	/*!
	  \param name label to print
	  Prints a ms timestamp 
	*/
	void Time(const std::string& name);

	/*! Simple profiling class. After going out of scope, prints elapsed time */
	class Profile
	{
	private:
		std::string name;
		struct timeval tv1;
	public:  
		/*!
		  \param name label to print near elapsed time at going out of scope
		*/
		Profile(const std::string& _name)
			{
				name = _name;
				gettimeofday(&tv1, NULL);
			}
		~Profile()
			{
				struct timeval tv2;
				gettimeofday(&tv2, NULL);
				std::cout << name << ": " << (tv2.tv_sec - tv1.tv_sec)*1000 + (tv2.tv_usec - tv1.tv_usec)/1000
						  << "ms" << std::endl;
			}
		
	};
	
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

#endif // __COMMONS_H__
