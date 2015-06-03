#ifndef __TERMINFO_DATABASE_H__
#define __TERMINFO_DATABASE_H__

#include"terminfo.h++"
#include"screen.h++"
#include"dictionary.h++"
#include<boost/shared_ptr.hpp>
namespace Scr
{

	namespace TI
	{

		/*!
		  \brief terminfo database finds system database and fetches entries
		 */
		class TerminfoDatabase
		{
		private:
			std::string path;

			bool status;
		public:
			/*!
			  Default constructor looks for terminfo resources
			 */
			TerminfoDatabase()throw();

			/*!
			  \param name $TERM
			  \return binary file containing term info.
			 */
			boost::shared_ptr<std::ifstream>
			OpenFile(const char * name)
				throw(FailedToOpenDatabase,
					  NotSupportedTerminalType,
					  FailedToLoadDatabaseEntry);

			/*!
			  \return true if database was successfully opened
			 */
			bool GetDatabaseStatus()throw();
			
		};
		

	}
}

#endif
