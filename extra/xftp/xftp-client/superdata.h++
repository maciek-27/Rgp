
#ifndef ___XFTPCLIENT__REAL_APP__SUPERDATA_
#define ___XFTPCLIENT__REAL_APP__SUPERDATA_
#include <libxftp/xftpcommons.h++>
namespace xftpclient
{
	//! real data thread implementation
	class SuperData : public xftp::DataThread
	{
	public:
		//!used internally and as interface to ExecuteCommand
		enum Command
		{Get, Put};
	private:

		//! command scheduled to execute
		Command C;
		
		//! filename associated w/ current command 
		std::string path;
		//! remaining filesize
		unsigned int file_size;
		
		//! state mutex
		streamcon::Mutex LocalMTX;
	public:
		SuperData (xftp::ClientBase& client, streamcon::Stream& stream);
		
		//! order running thread to read incoming file and save it (command Get)
		//! or send local file (command Put
		void ExecuteCommand	(Command c, std::string &p, unsigned int s) ;
	protected:
		
		void Run ();
	};
}
#endif
