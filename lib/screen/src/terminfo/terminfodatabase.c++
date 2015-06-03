#include "terminfodatabase.h++"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include"throw.h++"
#include"trace.h++"

const char * terminfoPath[]=
{
	// paths, where terminfo database may reside
	
	"/usr/share/terminfo",
	"/lib/terminfo",
	"/etc/terminfo",
	0
}; 

using namespace Scr::TI;

TerminfoDatabase::TerminfoDatabase()throw()
{
	/*look for terminfo directory*/
	struct stat statbuf;
	for (int i = 0 ; terminfoPath[i] ; i++)
	{
		if (stat(terminfoPath[i],&statbuf))
			/* if stat failed*/
			continue;
		
		if (!S_ISDIR(statbuf.st_mode))
			/* file found, but it is not a directory */
			continue;

		if (!(statbuf.st_mode & S_IROTH))
			/* do we have read permission? */
			continue;

		/*proper initialization*/
		{
			status = true;
			path.assign(terminfoPath[i]);
			RexIOLog(LogLevelModerate) << "Created link to terminfo database in path "
					  << path << std::endl;;
			return;
		}
	}
	
	status = false;
}

boost::shared_ptr<std::ifstream>
TerminfoDatabase::OpenFile(const char * name)
	throw(FailedToOpenDatabase,
		  NotSupportedTerminalType,
		  FailedToLoadDatabaseEntry)

{
	// do not do anything if failed to open DB
	EASSERT(status, FailedToOpenDatabase);

	
	std::string fullPath = path;
	fullPath.push_back('/');
	fullPath.push_back(name[0]);
	fullPath.push_back('/');
	fullPath.append(name);
	RexIOLog(LogLevelModerate) << "Looking for terminfo entry for " << name 
				   << " in file " << fullPath << std::endl;
	struct stat statbuf;

	if (stat(fullPath.c_str(),&statbuf)) {
		if (!strcmp("xterm", name)) {
			std::cout << "Fallback for color\n";
		/* if stat failed, then probably file does not exist*/
			return OpenFile("xterm-color");
		} else {
			THROW(NotSupportedTerminalType);
		}
	}
	
	boost::shared_ptr<std::ifstream>
		result ( new
			 std::ifstream( fullPath.c_str(),
					std::ifstream::in |
					std::ifstream::binary ) );
	
	if (result->good()) {
		return result;
	} else {
		THROW(FailedToLoadDatabaseEntry);
	}
			
}// OpenFile

bool TerminfoDatabase::GetDatabaseStatus()throw()
{
	return status;
}

