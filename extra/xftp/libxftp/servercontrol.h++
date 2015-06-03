#ifndef _SERVERCONTROL_H_
#define _SERVERCONTROL_H_
#include <boost/filesystem.hpp>
#include <libxftp/xftpcommons.h++>
#include <unistd.h>
#include "fsutil.h++"


namespace xftp {
	class ServerControl : public ControlThread
	{
	private:
		boost::filesystem::path dir; 
		FILE* file;
	public:
		typedef enum {SeekEnd = SEEK_END, SeekSet = SEEK_SET,
					  SeekCur = SEEK_CUR} SeekType;
		
		void Open(boost::filesystem::path &path) throw(OpenError, AlreadyOpened);
		size_t Read(char *buffer, size_t size) throw(ReadError, NotReady);
		void Write(char *buffer, size_t size) throw(WriteError, NotReady);
		void Seek(int pos, SeekType type = SeekSet) throw(SeekError, NotReady);
		int GetPos() throw(NotReady);
		void Close() throw(CloseError, NotReady);

		const std::string& GetDir();
		void SetDir(boost::filesystem::path& newdir) throw(OpenDirError);
		void ChDir(boost::filesystem::path& newdir) throw(OpenDirError);
		void ValidateDir(boost::filesystem::path& newdir)
			throw(OpenDirError);

		bool loggedIn;

		ServerControl(ClientBase& client, 
					  streamcon::Stream& _stream) :
			ControlThread(client, _stream),file(NULL) 
			{
				char * d = get_current_dir_name();
				dir=boost::filesystem::path(d);
				free(d);
			};	
		virtual ~ServerControl() {
			if(file) {
				fclose(file);
			}
		}
	};
}

#endif // _SERVERCONTROL_H_
