#ifndef __TERMINFO_KEYMAP_H__
#define __TERMINFO_KEYMAP_H__
#include "dictionary.h++"
#include "terminfo.h++"
#include "keyboard.h++"

namespace Scr
{
	namespace TI
	{
		class TerminfoEntry;
		
		/*!
		 * \brief Class responsible for mapping control sequences to unique key codes
		 *
		 * 
		 */
		class Keymap
		{
		private:
			typedef Dictionary<Scr::Key> key_dictionary;
			
			//!real engine of this module is Dictionary Tree.
			key_dictionary keyboard;	
		protected:
			
			/*!
			 * \param te Terminfo entry for which keymap will be generated
			 */
			explicit Keymap(const TerminfoEntry & te)
				throw();

			/*!
			 * Do real work of constructor. Way of doing this work may slightly
			 * differ for specific terminal types, so we have to move this action
			 * from the constructor to enable virtualization
			 * \param te Terminfo entry for which keymap will be generated
			 */
			virtual void InitializeKeymap(const TerminfoEntry & te)
				throw();
			
		
		public:
			
			typedef key_dictionary::iterator::validity validity;
			
			/*!
			 * \param code keycode provided by client. i.e. "\x1b[24~" means 
			 * function key F12 for DEC VT220 Terminal.
			 *
			 * Test if string supplied matches any key code stored in tree.
			 *
			 */
			validity TestCode(const char * code)
				throw();
			
			/*				
*			 * \return valid key code if matched,
			 * \exception Connection::UnsupportedKey is thrown when no such key
			 * is stored in tree. 
			 */
			Scr::Key GetCode(const char * code)
				throw(Connection::UnsupportedKey);

			virtual ~Keymap() throw() {;}
			friend class TerminfoEntry;
		};
	}	
}

#endif
