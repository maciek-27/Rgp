#ifndef __CORE_H__
#define __CORE_H__

#include <iostream>

namespace Scr
{
	//! \brief template class representing full implementation of
	//! Scr::Screen and Scr::__ScreenConnection
	/*!
	  \param LOCATION local, telnet etc ..
	  \param TYPE frameless VT100-like, UTF8, Windows....

	  See figure attached to Scr:: namespace description for more details
	*/
    template<class LOCATION,class TYPE>
    class RScreen : public LOCATION, public TYPE
    {
    public:
	RScreen(Connection & _connection,std::istream& _input, std::ostream& _output)throw()
			: GenericScreen(_input,_output),
			  LOCATION(_connection,_input,_output),
			  TYPE(_input,_output)
			{;}
		~RScreen()throw(){;}
    };

}
#endif /* __CORE_H__ */
