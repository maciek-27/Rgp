#ifndef __XFTP__FSUTIL__H__
#define __XFTP__FSUTIL__H__

#include <boost/filesystem.hpp>
#include <libxftp/xftpcommons.h++>
namespace xftp
{
	const int FIO_BUFSIZE = 1024*16;

	__DE (OpenDirError, streamcon::Exception);
	__DE (InvalidDir, OpenDirError);
	__DE (DirAccessDenied, OpenDirError);
	
	__DE (OpenError, streamcon::Exception);
	__DE (ReadError, streamcon::Exception);
	__DE (WriteError, streamcon::Exception);
	__DE (SeekError, streamcon::Exception);
	__DE (CloseError, streamcon::Exception);
	__DE (NotReady, streamcon::Exception);
	__DE (AlreadyOpened, streamcon::Exception);
	void SwitchDirectory (boost::filesystem::path& newdir, const boost::filesystem::path& olddir);
}

#endif
