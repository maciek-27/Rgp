#include "servercontrol.h++"
#include "fsutil.h++"
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <iostream>
#include "libstreamcon/throw.h++"
using namespace streamcon;
using namespace xftp;

void ServerControl::Open(boost::filesystem::path& path)
	throw(OpenError, AlreadyOpened)
{
	using namespace std;
	const char * cpath = path.string().c_str();
	errno=0;
	if(file)
		THROW(AlreadyOpened);
	file = fopen(cpath, "r+");
	if(!file)
	{
		file = fopen(cpath, "w");
		if(!file)
		{
			file = fopen(cpath, "r");
			if(!file)
				THROWP(OpenError,stringError(errno));
			else 
				cout << "opening file " << path << " r" << endl;
		}
		else 
			cout << "opening file " << path << " w" << endl;
	}
	else 
		cout << "opening file " << path << " r+" << endl;
}

size_t ServerControl::Read(char *buffer, size_t size)
	throw(ReadError, NotReady)
{
	errno=0;
	if(!file)
		THROW(NotReady);
	int status = fread(buffer, 1, size, file);
	if(feof(file)) {
		if (status>0)
			return status;
		return 0;
	}
	if(status == -1) {
		THROWP(ReadError,stringError(errno));
	}
	return status;
}

void ServerControl::Write(char *buffer, size_t size)
	throw(WriteError, NotReady)
{
	errno=0;
	if(!file)
		THROW(NotReady);
	// Partial write, what to do afterwards? Erase?
	// Anyway, not handled at the moment
	if(fwrite(buffer, 1, size,file) < size) {
		THROWP(WriteError,stringError(errno));
	}
}

void ServerControl::Seek(int pos, SeekType type)
	throw(SeekError, NotReady)
{
	if(!file)
		THROW(NotReady);
	if(fseek(file, pos, static_cast<int>(type)))
		THROW(SeekError);
}

void ServerControl::Close() throw(CloseError, NotReady)
{
	if(!file)
		THROW(NotReady);
	if(fclose(file))
		THROW(CloseError);
	file = NULL;
}


int ServerControl::GetPos() throw(NotReady)
{
	if(!file)
		THROW(NotReady);
	if(feof(file))
		return EOF;
	return ftell(file);
}

void ServerControl::ValidateDir(boost::filesystem::path& newdir)
	throw(OpenDirError)
{
	ScopedLock lock(opendir_Mutex);	
	DIR *d = opendir(newdir.string().c_str());
	if(!d) {
		if(errno == EACCES)
			THROW(DirAccessDenied);
		else if(errno == ENOTDIR || errno == ENOENT)
			THROW(InvalidDir);
		else
			THROW(OpenDirError);			
	}
	closedir(d);
}

const std::string& ServerControl::GetDir()
{
	return dir.string();
}

void ServerControl::SetDir(boost::filesystem::path& newdir) throw(OpenDirError)
{
	ValidateDir(newdir);
	dir = newdir;
}

void ServerControl::ChDir(boost::filesystem::path& newdir) throw(OpenDirError)
{
	
	SwitchDirectory(newdir,dir);

	std::cout << "switching directory " << newdir << std::endl;
	ValidateDir(newdir);
	dir = newdir;
}


