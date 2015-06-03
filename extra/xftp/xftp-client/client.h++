#ifndef ___XFTPCLIENT__REAL_APP__CORE_
#define ___XFTPCLIENT__REAL_APP__CORE_


#include <libxftp/xftpcommons.h++>
#include <boost/functional.hpp>
#include<boost/filesystem.hpp>
#include<boost/tokenizer.hpp>
#include<sstream>
#include <boost/thread/detail/singleton.hpp>
#include "listseparator.h++"

namespace xftpclient
{
	class SuperDuperClient;

	/*!
	 * Used to split commands into tokens
	 */
	typedef boost::tokenizer<ListSeparator<char> > Tokenizer;
	
	/*!
	 * Any function used to execute command
	 */
	typedef boost::function<void (Tokenizer &)> Executor;
	
	/*!
	 * CWD
	 */
	extern boost::filesystem::path CurrentPath;
	
	/*!
	 * Last retrieved list
	 */
	extern std::tr1::shared_ptr<xftp::Message> RemoteList;
	
	
	/*!
	 * For easy access to commands
	 */
#define commands boost::detail::thread::singleton<CommandDictionary>::instance()
	
	/*!
	 * For parsing commands
	 */
	class CommandDictionary: public xftp::Dictionary<
	std::pair<Executor, std::string> >
	{
	protected:
		CommandDictionary ()
		{}
	};
	
	/*!
	 * To iterate through command dictionary.
	 */
	typedef xftp::Dictionary<std::pair<Executor, std::string> >::iterator CommandIterator;
}
extern xftpclient::SuperDuperClient app;


#endif
