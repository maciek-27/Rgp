#include "supercontrol.h++"
#include <iostream>
#include <libxftp/client/remove.h++>
#include "supercontrol_internal.h++"
void  xftpclient::SuperControl::Remove (Tokenizer &	optns)
{
	using namespace std;
	using namespace xftp;
	using std::tr1::shared_ptr;
	Tokenizer::iterator i =optns.begin ();
	if (i!=optns.end ())
	{
		string path (*i);
		client::MsgRemoveItem * R = new client::MsgRemoveItem;
		
		R->SetFilename(path);
		xmlStream.PostMessage (shared_ptr<Message> (R));
		
		shared_ptr<Message> Result ( xmlStream.GetMessage ());
		XFTP_ASSERT (Result, "remove_item failed", return);
		boost::filesystem::path newPath (path);
		
	}
	else
		cout << "path?" << endl;
	
}
