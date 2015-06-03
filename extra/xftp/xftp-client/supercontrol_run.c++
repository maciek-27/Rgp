#include "supercontrol_internal.h++"
#include "supercontrol.h++"
#include <libxftp/allmessages.h++>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <sys/stat.h>
#include "superdata.h++"
#include "completions.h++"
#include <algorithm>
// for input stream filters
#include <libxftp/zipfilters.h++>
#include "printxml.h++"

// for reference bind1st
#include <boost/functional.hpp>

// GNU readline == input autocompletion +  history
#include <readline/readline.h>
#include <readline/history.h>

#if defined _GNU_SOURCE
#define XCRDBUF_AVAIL
#include <sys/ioctl.h>

#include <ext/stdio_filebuf.h>
//typedef  gnustream;
namespace {
int lineno;
int maxlineno=22;

	//! magical streambuf, that pauses output when it exceeds screen height
template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class xcrdbuf: public __gnu_cxx
::stdio_filebuf<char,std::char_traits<char> >
{
protected:
	typedef std::basic_filebuf<_CharT,_Traits> basic;

	//! Check if we have to stop printing
	void lineno_test()
		{
#ifdef    TIOCGWINSZ
			struct winsize ws;
			if (ioctl(0, TIOCGWINSZ, (char *)&ws) >= 0)
				maxlineno = ws.ws_row-2;
#endif
			
			if (lineno>maxlineno)
			{
				const char * s="\nPress [Enter] to scroll";
				printf(s);
				fflush(stdout);
				fflush(stdin);
				getchar();
				lineno=0;
			}
		}
public:
	xcrdbuf():__gnu_cxx::stdio_filebuf<_CharT,_Traits>(1,std::ios::out)
		{
		}
	//! overload default synchronize function: count lines
	int sync() {
		int result = fflush(stdout);
		lineno+=std::count(pbase(),pptr(),'\n');
		lineno_test();
		seekpos(0);
		return result;;
	}
	//! overload default put function: forcing synchronize after each
	//! write
	//! \param s as in C++ standard
	//! \param n as in C++ standard
 	std::streamsize xsputn ( const _CharT * s, std::streamsize n )
 		{
			basic::xsputn(s,n);
			sync();
			return n;
 		}
	//! start counting lines
	static void clear_lineno()
		{
			lineno=0;
		}
};

}
#endif
using namespace xftpclient;



#include "functors.h++"
#include "boost/filesystem/operations.hpp"

void SuperControl::Run ()
{	
	using std::tr1::shared_ptr;
	using namespace std;
	using namespace xftp;
	using namespace boost::lambda;
	using namespace boost::filesystem;

#ifdef XCRDBUF_AVAIL	
	// add support for scrolling
	cout.rdbuf(new xcrdbuf<char>);
#endif
	
	// setup i/o processing
	stdfilter::FilterChain InputFilters;
	InputFilters.AddFilter (stdfilter::UnZCompress);
	InputFilters.AddFilter (PrintXML (32));
	xmlStream.SetGetFilter ( InputFilters);
	
	stdfilter::FilterChain OutputFilters;
	OutputFilters.AddFilter (PrintXML (33));
	OutputFilters.AddFilter (stdfilter::ZCompress);
	xmlStream.SetPostFilter ( OutputFilters);

	// set default command prompt
	const char * EnvPromptTemplate = "XFTPPROMPT";
	char * p = getenv(EnvPromptTemplate);
	if (p)
		PromptTemplate.assign(p);
	else
		PromptTemplate.assign("$r> ");
	
#define assignCommandNonMem(c,F,d)							\
	commands.insert(c,pair<Executor,string>(F,string(d)));
#define assignCommandMem(c,F,d)											\
	assignCommandNonMem(c,boost::bind1st(								\
							std::mem_fun(&SuperControl::F),this),d)
	
#define assignAlias(a,c)\
	assignCommandNonMem(a,commands.find(c)->first,"- alias for -> "  c)
	
	assignCommandNonMem ("lpwd", Print((path(*)())current_path<path>)
						 ," - Print local working directory.");

	assignCommandNonMem ("pwd",
						 Print(bind(std::mem_fun(&path::string),&CurrentPath))
						 ," - Print local working directory.");
	
	assignCommandNonMem ("xml",Toggle(PrintXML::Enable,PrintXML::Disable)
				   ,"<yes|no> - Enable/disable debug xml listing. ");

	assignCommandNonMem ("prompt",
						 Print(bind(std::mem_fun(&string::c_str),
									&PromptTemplate))
						 ," - print prompt template.");

	assignCommandNonMem ("set_prompt",Set(
							 bind(//string::assign must be explicitly "casted"
								  //as there are many functions with
								  //the same name.
								 mem_fun((  string &(string::*)(const string &))
										 & string::assign), &PromptTemplate,
								 boost::lambda::_1))
						 ,
						 " - set prompt template. Special keywords may be "
						 "used while defining prompt:\n"
						 "$L - local path (full)\n"
						 "$l - local path (simplified: usually last component)"
						 "\n$R - remote path (full)\n"
						 "$r - remote path (simplified: as above)\n"
						 "If you like to use any of theese sequences "
						 "as themselves (not replaced w/ paths) "
						 "preceed them with \\\\.\n"
						 "Default xftp client prompt may be set with "
						 "XFTPPROMPT environment variable."
		);


	
	assignCommandMem ("lcd",LChDir,"<path> - Change local working directory.");

	assignCommandMem ("lls", LList, "[path] [recursive] - List local directory."
			"\n[path] may be relative or absolute. You must have r-x permission "
			"to list contents of directory"
			"\n[recursive] Any positive integer to trigger recursive directory "
			"listing. 1 means list contents of all subdirectories of path, and "
			"nothing more. -1 means no limits (i.e. full directory tree w/o "
			"links.)"
			"\nHINT: If you want to create recursive listing of current directory, "
			"type lls . -1");

	assignCommandMem ("ls", List, "[path] [recursive] - list remote directory."
			"\n[path] may be relative or absolute. You must have r-x permission "
			"to list contents of directory"
			"\n[recursive] Any positive integer to trigger recursive directory "
			"listing. 1 means list contents of all subdirectories of path, and "
			"nothing more. -1 means no limits (i.e. full directory tree w/o "
			"links."
			"\nHINT: If you want to create recursive listing of current directory, "
			"type ls . -1 (server does not have to support this feature)"
			"\nNOTE: If recursive listing is not working properly, probably "
			"server administrator has disabled or limited this feature.");

	assignCommandMem ("cd", ChDir, "<path> - Change remote directory.");

	assignCommandMem ("help", Help, "- This screen.");

	assignCommandMem ("put", Put, "<file> - Upload local file. File must be "
			"regular (i.e. it can't be directory or block device). "
			"File will be saved in current remote directory.");

	assignCommandMem ("get", Get, "<file> - Download remote file. File must be "
			"regular (i.e. it can't be directory or block device). "
			"File will be saved in current local directory.\n\n");

	assignCommandMem ("exit", Exit, "- end using XFTP. ");
	
	assignCommandMem ("mkdir", MkDir, "<path> - Create directory on remote server. "
			"\nPath may be relative to remote working directory, or absolute on "
			"remote machine. You must have apropriate writing permissions to "
			"create directory.");

	assignCommandMem ("lmkdir", LMkDir, "<path> - Create directory on local computer. "
			"\nPath may be relative to local working directory, or absolute on "
			"local machine. You must have apropriate writing permissions to "
			"create directory.");

	assignCommandMem ("rm", Remove, 
			"<path> [-1] - Remove file or empty directory on remote server. "
			"\n\n   \n\n  If -1 option is enabled, nonempty directories may "
			"be removed too (this option enables recursive removal)");

	assignCommandMem ("lrm", LRemove, 
			"<path> [-1] - Remove file or empty directory "
			"on local computer. "
			"\nIf -1 option is enabled, nonempty directories may be removed too"
			" (this option enables recursive removal)");

		

	//default command aliases - as for FTP
	assignAlias("dir","ls"); 
	assignAlias("recv","get"); 
	assignAlias("send","put"); 
	assignAlias("quit","exit");
	assignAlias("?","help");

	
	client::MsgHello *hello = new client::MsgHello;
	
	hello->SetUserAgent (
	"XFTP Demo Client - Oprogramowanie Systemowe 2008");
	xmlStream.PostMessage (
	shared_ptr<Message>(hello));
	
	std::tr1::shared_ptr<Message> ptr = xmlStream.GetMessage ();
	std::tr1::shared_ptr<Message> authResponse;
	do
	{
		ptr->Process (*this);
		authResponse = xmlStream.GetMessage ();
	}
	while(toNative (authResponse->GetName ())
	== "auth_refuse");
	//authResponse->Process(*this);
	server::MsgServerInfo * server_info =
			dynamic_cast<server::MsgServerInfo*>(authResponse.get ());
	assert (server_info);
	CurrentPath=boost::filesystem::path (server_info->GetStartingDir ());
	
	// GNU Readline internal variable;
	rl_attempted_completion_function = completions;
	
	
	while (1)
	{
		
		char * cmd = readline (GeneratePrompt().c_str());
		string command (cmd);
		xcrdbuf<char>::clear_lineno();
		add_history (cmd);
		stringstream tmp;
		tmp << command;
		tmp >> command;
		if (command.empty())
			continue;
		xftp::Dictionary<pair <Executor, string> >::iterator i =
				commands.find ( command.c_str () );
		if (i.valid () )
		{
			string attributes (tmp.str (), tmp.tellg ());
			Tokenizer tok (attributes,
					ListSeparator<char>('\\', ' ', '\"'));
			try
			{
				i->first (tok);
			}
			catch (std::exception & e)
			{
				cout << "Unexpected problem happened while processing command:\n"
					 << e.what() << endl;
			}
			catch (...)
			{
				cout << "Unknown problem happened while processing command"
					 << endl;
			}
		}
		else
			cout << "Incorrect command. see help for command summary." << endl;
		free (cmd);
	}
	
	std::tr1::shared_ptr<Message> ptr2 = xmlStream.GetMessage ();
	ptr2->Process (*this);
}
