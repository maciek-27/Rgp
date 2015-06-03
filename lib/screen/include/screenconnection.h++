#ifndef __SCREEN_CONNECTION_H__
#define __SCREEN_CONNECTION_H__
#include "keyboard.h++"
#include"fileno_hack.h++"
#include"throw.h++"

namespace Scr
{
	
    //! \brief internal class which is base for all connection-specific
    //! implementations of screen (multiple-inheritance case)
    /*!
      It represents internal interface between Scr::Connection and
      Scr::Screen classes. 
	*/
    class __ScreenConnection
    {
    protected:
		/*!
		  ProcessConnection will return this value upon successful finish
		*/
		int exitcode;
		/*!
		  is application running? does it have to stop?
		  (ExitConnection() is called by Connection::Exit(int), sets
		  exit code and breaks main loop performed in ProcessConnection)
		*/
		Connection & connection;

		/*!
		  break main loop if set to false
		 */
		bool active;
	public:
		/*!

		  \param _input input stream (used to capture some of events,
		  in particular keyboard events).	  
		  \param _connection newely estabilished connection to serve
		*/
		__ScreenConnection(Connection & _connection,std::istream & _input)throw();
		/*!
	  
		  \return value of exitcode, as it was in the moment of
		  connection termination if successful.
	  	  
		  Initialize, conduct and end connection in way apropriate to
		  connection type, operating system etc. Inform
		  Scr::Connection object supplied about all captured events

		  \note as function (for design reasons) lacks exception-set
		  specification, it may throw any exceptions, but it is
		  recommended, that only exceptions typical to
		  Scr::Connection::Start() will be thrown.
	  
		*/
		virtual int  ProcessConnection() =0;

		/*!
		  \param _code exit code return from ProcessConnection after
		  successfully finished connection
	  
		  Force stopping connection as soon as possible
		  \note as function (for design reasons) lacks exception-set
		  specification, it may throw any exceptions, but it is
		  recommended, that only exceptions typical to
		  Scr::Connection::Exit() will be thrown.
		*/
		virtual void ExitConnection(int _code);
		virtual ~__ScreenConnection()throw();
    };
}
#endif
