#ifndef __TERMINFO_CORE_H__
#define __TERMINFO_CORE_H__
#include"screen.h++"
#include"dictionary.h++"
#include<fstream>
#include<iostream>
#include <boost/thread/mutex.hpp>
namespace Scr
{
	/*!
	  Terminfo database connectivity facilities
	 */
	namespace TI
	{

		class Keymap;
		__DE(DatabaseException,Exception);
		__DE(FailedToLoadDatabaseEntry,DatabaseException);
		__DE(FailedToOpenDatabase,DatabaseException);
		__DE(DatabaseNotOpen,DatabaseException);
		__DE(NotSupportedTerminalType,DatabaseException);

		class TerminfoCore;
		/*!
		  \brief Terminfo entry for single terminal type

		  Terminfo entries will be read from system terminfo database
		  (hashed database or hierarchical directory tree). Only way
		  to obtain this class object is to call apropriate function
		  of TerminfoCore object;
		 */
		class TerminfoEntry
		{
		private:
			struct
			{

				//(2) the size, in bytes, of the names section;
				short namesSize;

				//(3) the number of bytes in the boolean section;
				// (one boolean value in one byte)
				short numBooleans;
				
				//(4) the number of short integers in the numbers section;
				short numIntegers;
				
				//(5) the number of offsets (short integers) in the strings section;
				short numOffsets;
				
				//(6) the size, in bytes, of the string table.
				short stringTableSize;
			}Meta;
			struct
			{// 3 C-style dumb vectors
				char * names;
				char * booleans;
				short * numbers;
				char ** strings;
			}Data;
			char * text; // raw content of file after metadata.
			
			mutable boost::mutex textmod_mtx;
		protected:
			/*!
			  \param ifile - resource reference to compiled terminfo file, that will be used
			  to initialize this entry

			  Default constructor opens the file and reads all
			  information in it. 
			*/
			explicit TerminfoEntry(std::ifstream & ifile)throw();

			/*!
			  \param i cap. ID (enumerated in capabilities.h++)
			  \return i'th boolean value
			 */
			bool GetBoolean (int i) const throw();
			
			/*!
			  \param i cap. ID (enumerated in capabilities.h++)
			  \return positive integer if feature is supported; -1 otherwise.
			 */
			short GetInteger (int i) const throw();

			/*!
			  \param i cap. ID (enumerated in capabilities.h++)
			  \return c-style string if feature is supported. NULL
			  pointer otherwise.
			 */
		    const char * GetString (int i) const throw();
		public:
			__DE(CapabilityNotSupported,Exception);
			__DE(ParseError,Exception);
		protected:
			/*!
			  \param i cap. ID (enumerated in capabilities.h++)
			  \param param parameters
			  (refer to terminfo(5) for parameter descriptions)

			  Parse parametrized string
			  
			  \note implementation currently does not fully conform
			  specification, however it does what is needed for this library.
			 */
			std::string ParseString(int i, Uint * param)
				const throw(CapabilityNotSupported,ParseError);
			
			Keymap * keymap;
		public:
			
			Keymap & GetKeymap() const;
			
			/*!
			  \param newPosition new position (0,0 .. height-1,width-1)

			  \return control string to set cursor position specific
			  to this terminal type
			  
			  Explicitly move cursor to the new position
			 */
			const std::string GotoYX(const Scr::Position & newPosition) const
				throw(CapabilityNotSupported);

			/*!
			  \param newPosition new position of cursor (0,0 .. height-1,width-1)
			  \param oldPosition current position

			  \return optimal control string to set cursor position specific
			  to this terminal type
			  
			  Recommended way of setting cursor position. This
			  function selects way of setting position, that consumes
			  least possible number of bytes.
			  
			  \note dest and then source: the same argument order as
			  for C library functions.
			 */
			const std::string GotoYX(const Scr::Position & newPosition,
									 const Scr::Position & oldPosition) const
				throw(CapabilityNotSupported)  ;

			/*!
			  \param s display style to be set
			  \return control string to set display style for text.
			  
			 */
			const std::string SetDisplayStyle(const Scr::DisplayStyle s) const
				 throw(CapabilityNotSupported)  ;

			/*!
			  \param newStyle display style to be set
			  \param oldStyle current style 
			  \return control string to set display style for text.

			  if current style is known, it is highly recommended to
			  use this function as it will set minimum required subset
			  of style attributes
			 */
			const std::string SetDisplayStyle(const Scr::DisplayStyle newStyle,
										const Scr::DisplayStyle oldStyle)const
				throw(CapabilityNotSupported) ;

			/*!
			  Make cursor visible
			 */
			const std::string ShowCursor() const throw(CapabilityNotSupported);
			
			/*!
			  Make cursor invisible
			 */
			const std::string HideCursor() const throw(CapabilityNotSupported);

			/*!
			  Move cursor to the begining-of-the-screen position
			  ( the same effect as GotoYX(Position(0,0)), but possibly
			  faster )
			 */
			const std::string CursorHome() const throw(CapabilityNotSupported);

			
			~TerminfoEntry()throw();
			friend class TerminfoCore;
			friend class Keymap;
		};

		
		/*!
		  \brief Terminfo subsystem core: manages entries etc.
		  
		  As this class is a singleton class, only one it's instance
		  may exist in the same time. don't bother calling it's
		  constructor manually, as this will result in exiting program
		  at all.
		*/
		class TerminfoCore
		{
		private:
 			mutable Scr::Dictionary<TerminfoEntry* > entries;

			/*!
			  Default constructor; called by static GetTerminfo
			  \exception Scr::TI::FailedToOpenDatabase
			  is thrown when no database found in supported format.
			 */
			TerminfoCore() throw();

			/*!
			  Default destructor
			 */
			~TerminfoCore() throw();

			/*!
			  Function returns reference to TerminfoEntry object. If
			  it was already retrieved, reference to existing one is
			  returned. Otherwise new is created. 
			 *
			 *\param name name of terminal type ($TERM)
			 */
			const TerminfoEntry & __GetTerminfo(const char * name)
				throw(NotSupportedTerminalType);

		public:

			/*!
			  This function forces initialization of terminfo database subsystem
			 */
			static void Initialize()throw(FailedToOpenDatabase);

			/*!
			  \return true if database was successfully opened
			 */
			static bool GetDatabaseStatus()throw(DatabaseNotOpen);
			
			/*!
			  \param name $TERM
			  \return const reference to terminfo entry object
			  \exception
			  Scr::TI::NotSupportedTerminalType is
			  thrown when not supported terminal type is requested

			  \exception Scr::TI::FailedToOpenDatabase
			  is thrown when no database found in supported format.
			 */
			static const TerminfoEntry & GetTerminfo(const char * name)
				throw(NotSupportedTerminalType,FailedToOpenDatabase);

			/*!
			  Force destruction of terminfo subsystem. This may cause
			  numerous problem while any objects are still referencing
			  terminfo entries. This function frees all TI resources
			  if any allocated. Otherwise it won't do anything (so
			  that there is no rish of "double free error").
			 */
			void CleanUp()throw();

			/*!
			  Function conditionally cleans up terminfo connectivity subsystem.
			 */
			static  void FreeTerminfoEntry()throw();

		};
		
	}

}

#endif
