#include "supercontrol_internal.h++"
#include "supercontrol.h++"
#include <libxftp/allmessages.h++>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <sys/stat.h>
#include "superdata.h++"
void xftpclient::SuperControl::Put (Tokenizer &	 optns)
{		
	using std::tr1::shared_ptr;
	using namespace std;
	using namespace xftp;
	using namespace boost::lambda;
	auto_ptr<string> path;
	Tokenizer::iterator i = optns.begin ();
	if (i!=optns.end ())
		path.reset (new string (*i));
	else
		XFTP_DIE ("No filename specified", return);
	
	DataThread* dt=AssureDT (single_data );
	struct stat st;
	if(stat (path->c_str (), &st))
	{
		std::cout << "stat failed for" << *path << std::endl;
		return;
	}
	int file_size = st.st_size;
	
	
	client::MsgOpen * open = new client::MsgOpen;
	open->SetFilename (*path);
	
	xmlStream.PostMessage (shared_ptr<Message>(open), true);
	
	shared_ptr<Message> Result (  xmlStream.GetMessage ());
	
	XFTP_ASSERT (Result, "failed to open file", return);
	
	// if open successful, write
	SuperData& sd = dynamic_cast<SuperData&>(*dt);
	sd.ExecuteCommand (SuperData::Put, *path, file_size);
	
	
	
	
	client::MsgWrite * w = new client::MsgWrite;
	w->SetSize (file_size);
	w->SetStreamID (single_data);
	
	
	xmlStream.PostMessage (shared_ptr<Message>(w), false);
	xmlStream.PostMessage (shared_ptr<Message>(
	new client::MsgClose), true);
	
	
	Result=xmlStream.GetMessage ();
	XFTP_ASSERT (Result, "read error", xmlStream.GetMessage ();return);
	Result=xmlStream.GetMessage ();
	XFTP_ASSERT (Result, "close error", return);
	
}
