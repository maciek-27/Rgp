#include "supercontrol.h++"
#include <cstdlib>
#include <iostream>
#include <libxftp/client/list.h++>
#include <libxftp/server/list.h++>
#include "supercontrol_internal.h++"

void xftpclient::SuperControl::List (Tokenizer &	 optns)
{
	
	using namespace std;
	string path (".");
	int recurse=0;
	Tokenizer::iterator i =optns.begin ();
	if (i!=optns.end ())
	{
		path.assign (*i);
		if (++i!=optns.end ())
			recurse=atoi (i->c_str ());
	}
	xftp::client::MsgList * list_request = new xftp::client::MsgList;
	list_request->SetRecurse (recurse);
	list_request->SetDir (path);
	xmlStream.PostMessage (
	std::tr1::shared_ptr<xftp::Message>(list_request));
	RemoteList = xmlStream.GetMessage ();
	XFTP_ASSERT (RemoteList, "list failed", return);
	PrintNode (dynamic_cast<xftp::server::MsgList * > 
			( RemoteList.get () ) -> GetNode ());
}
