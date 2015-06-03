#ifndef __TERMINFO_ENABLED_H__
#define __TERMINFO_ENABLED_H__
#include "screen.h++"
#include "terminal.h++"
#include "terminfo.h++"

namespace Scr
{
    //! class representing terminal controlled according to terminfo database
    /*!
      This class provides full implementation of Scr::Screen abstract
      interface in terms of capabilities of any terminal described in
      terminfo database.

      \latexonly
      Algorithm for Refresh()

      \begin{figure}[H]
      \begin{center}
      \leavevmode
      \includegraphics[width=260pt]{../Optimised_Refresh_algorithm_general_flowchart}
      \end{center}
      \end{figure}

      Aux procedure used there
      \begin{figure}[H]
      \begin{center}
      \leavevmode
      \includegraphics[width=100pt]{../next_coords_algorithm}
      \end{center}
      \end{figure}

      \endlatexonly
     */
    class TerminfoEnabledScreen:public virtual GenericScreen, public Terminal
    {
    private:
		TI::TerminfoEntry * ti;

		
		DisplayStyle p;
		
	protected:
		
		/*!
			Minimum implementation supportingonly 12 basic functionkeys, arrows 
		    and few special, in several formats of VT100-like terminal emulators. 		 
		 */
		virtual Key DecodeKeyPressed()
		throw(Connection::UnsupportedKey,Screen::InvalidUTF8);
	public:
		explicit TerminfoEnabledScreen(std::istream & _input,
				std::ostream & _output)throw();

		/*!
		  Full support for colour and refreshing algorithm optimized
		  for transfer
		*/
		virtual void Refresh()throw(ConnectionError);

		/*!
		  \param rows
		  \param cols
	  
		  differs from Scr::GenericScreen::Resize only by the fact,
		  that it supports copyBuffer
		*/
		virtual void Resize(Uint rows, Uint cols)
			throw();
		virtual void CleanUp()  throw(ConnectionError);
		~TerminfoEnabledScreen()throw();
    };
}

#endif
