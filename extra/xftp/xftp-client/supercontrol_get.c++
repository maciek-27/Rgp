#include "supercontrol_internal.h++"
#include "supercontrol.h++"
#include <libxftp/allmessages.h++>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <sys/stat.h>
#include <errno.h>
#include "superdata.h++"
//get file
void xftpclient::SuperControl::Get (Tokenizer &	 optns)
{
	
	using std::tr1::shared_ptr;
	using namespace std;
	using namespace xftp;
	using namespace boost::lambda;
	auto_ptr<string> path;

	// Select first nonempty string as name of file to be retrieved
	Tokenizer::iterator i =
			find_if (optns.begin (), optns.end (),
					 !bind (&string::empty, boost::lambda::_1));
	if (i!=optns.end ())
		path.reset (new string (*i));
	else
		// return if no file
		XFTP_DIE ("No filename specified", return);

	// open file for writing (this will be copy of remote file) and create it as
	// empty one
	FILE * f = fopen (path->c_str (), "w");
	if (!f)
		XFTP_DIE ("Could not open file for writing "<< *path
				<< stringError (errno), return);
	fclose (f);

	// create data thread for single data transfer link (multiple links are
	// not yet supported
	DataThread* dt=AssureDT (single_data );
	
	// XML transaction STEP 1:
	// request to open remote file (client:open XML command is analogous to
	// UNIX open routine)
	// 1 a) : setup message
	client::MsgList * list_request = new client::MsgList;
	list_request->SetRecurse (0);
	list_request->SetDir (*path);
	xmlStream.PostMessage (shared_ptr<Message>(list_request), false);
	client::MsgOpen * open = new client::MsgOpen;
	open->SetFilename (*path);
	// 1 b) post message
	xmlStream.PostMessage (shared_ptr<Message>(open), true);
	
	
	// STEP 2: verify, if server allows us to operate specific file
	shared_ptr<Message> Result ( xmlStream.GetMessage ());
	XFTP_ASSERT (Result, "file does not exist", xmlStream.GetMessage ();return);
	int file_size =
	dynamic_cast<server::MsgList * > ( Result.get () ) -> GetNode ()
	.GetMetadata ().filesize;
	
	// If OK, set up data transmission
	SuperData& sd = dynamic_cast<SuperData&>(*dt);
	sd.ExecuteCommand (SuperData::Get, *path, file_size);

	// STEP 3: order server to post whole file (client:read is - as above -
	// simmilar to UNIX read routine)
	client::MsgRead * read = new client::MsgRead;
	read->SetSize (file_size);
	read->SetStreamID (single_data);
	Result= xmlStream.GetMessage ();
	XFTP_ASSERT (Result, "failed to open file", return);
	
	// 3 b) Post READ and CLOSE message
	xmlStream.PostMessage (shared_ptr<Message>(read), false);
	xmlStream.PostMessage (shared_ptr<Message>(
	new client::MsgClose), true);
	 /// server is implemented in  way, that allows it to cope with situations,
	 /// where client doesn't close file, but server is not required to support
	 /// such client any longer.
	
	// Assure, thar both read and  close return success.
	Result=xmlStream.GetMessage ();
	XFTP_ASSERT (Result, "read error", xmlStream.GetMessage ();return);
	Result=xmlStream.GetMessage ();
	XFTP_ASSERT (Result, "close error", return);
	
}
