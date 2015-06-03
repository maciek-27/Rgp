
#ifndef __LOCALSCREEN_H__
#define __LOCALSCREEN_H__
#include <termios.h>
#include "genericscreen.h++"
#include "screenconnection.h++"
namespace Scr
{
    //! connection on localhost, using cin/cout
    class LocalScreen: public virtual GenericScreen,
		       public __ScreenConnection
    {
    private:
	/*!
	  Store initial terminal settings to restore them after
	  finishing connection (especially settings connected with
	  local echo.
	 */
	struct termios term;
    public:

		/*!
		  \param infd file descriptor
		  \return true if size changed
		  
		  Function checks if size set for object equals size of
		  appropriate screen. If it differs, Resize() is called to
		  match changes
		*/
		void TestForResize();

		/*!
		  \param _connection reference to object representing
		  connection itself
		  \param _input reference to input stream
		  \param _output reference to output stream
		 */
		LocalScreen(Connection & _connection,std::istream & _input,std::ostream & _output)throw();

		/*!
		  \return getenv("TERM");
		 */
		virtual const char * GetType() const throw();

		
		/*!
		  basic main loop of application using local screen
		*/
		int ProcessConnection();
		~LocalScreen()throw();
    };
}

#endif // __LOCALSCREEN_H__
