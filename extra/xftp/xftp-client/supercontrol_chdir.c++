#include "supercontrol.h++"
#include <cstdlib>
#include <iostream>
#include <libxftp/client/chdir.h++>
#include "supercontrol_internal.h++"
#include <libxftp/fsutil.h++>

void  xftpclient::SuperControl::ChDir (Tokenizer &	 optns)
{
	
	using namespace std;
	using namespace xftp;
	using std::tr1::shared_ptr;
	Tokenizer::iterator i =optns.begin ();
	// if argument is supplied, it must be (relative or absolute) path.
	// this command won't work w/o argument
	if (i!=optns.end ())
	{
		string path(*i);
		// Post XML message containing apropriate path (that is client:chdir)
		xmlStream.PostMessage (
		shared_ptr<Message>(new client::MsgChDir (path)));
		// Read message
		shared_ptr<Message> Result ( xmlStream.GetMessage ());

		// in case of failure just inform client and exit
		XFTP_ASSERT (Result, "chdir failed", return);

		// save current path (just for display prompt purposes)
		boost::filesystem::path newPath (path);
		SwitchDirectory (newPath, CurrentPath);
		CurrentPath.swap (newPath);
	}
	else
		cout << "path?" << endl;
}
