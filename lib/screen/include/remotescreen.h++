
#ifndef __REMOTE_SCREEN_H__
#define __REMOTE_SCREEN_H__
#include "screenconnection.h++"
#include "keyboard.h++"
//not include termios.h as it declares macro ECHO conflicting with
//ECHO in TELNET namespace 

namespace Scr
{
    //! TELNET connection
    class RemoteScreen: public virtual GenericScreen,
						public __ScreenConnection
    {
    protected:
		std::string clientname;
	
		/*!
		  general subnegotiation switch.
		*/
		void AnswerCommand();
		/*!
		  Read window size and possibly call OnResize; Handle
		  subnegotiation end (SE) correctly.
		  ASSUME, that IAC SB NAWS was just recv, so process w h IAC
		  SE (check for correctnes after each).
		  \latexonly \index{RFC, reference to!1073}\endlatexonly
		*/
		void SubnegotiateWindowSize();
		/*!
		  read information on terminal type. 
		  \latexonly \index{RFC, reference to!1091}\endlatexonly
		*/
		void SubnegotiateTerminalType();
		/*!
		  Process characters according to telnet protocol. Handle
		  variants of Enter key.
		  
		  \latexonly \index{RFC, reference to!854}\endlatexonly
		*/
		virtual Key DecodeKeyPressedHandleTelnet();

		/*!
		  When resize request is pending, store requested dimensions
		  here.
		 */
		Size requestedSize;
		/*!
		  Client has requested resize. Let him wait until counter == 0.
		 */
		bool resizeRequestPending;

		/*!
		  1-2-3-...-254-255-0
		 */
		char counter;

		static const Uint windowSize = 1;
		static const Uint terminalType = 2;
		Uint telnetSettings;
	public:
		RemoteScreen(Connection & _connection,
					 std::istream & _input,
					 std::ostream & _output)throw();

		/*!
		  \return returns information retrieved by
		  SubnegotiateTerminalType() if telnet client supports
		  TELNET::TTYPE extension (RFC 1091). If client does not
		  support this feature, dumb terminal type will be assumed and
		  NULL will be returned. "unknown" special value will be
		  returned 
		  \latexonly \index{RFC, reference to!1091}\endlatexonly
		 */
		virtual const char * GetType() const throw(TerminalTypeUnknown);

		int ProcessConnection();
		~RemoteScreen()throw();
		friend class TelnetCommandProcessor;
    };
}
#endif
