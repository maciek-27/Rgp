#include "supercontrol_internal.h++"
#include "supercontrol.h++"
#include <libxftp/allmessages.h++>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <libxftp/client.h++>

xftp::DataThread* xftpclient::SuperControl::AssureDT (int n)
{	
	using std::tr1::shared_ptr;
	using namespace std;
	using namespace xftp;
	using namespace boost::lambda;
	
	DataThread* dt;
	
	if (not has_dstream)
	{
		// Create datathread for data stream n. rerutn NULL if failed;
		client::MsgDStreamRequest * dsr = new client::MsgDStreamRequest;
		dsr->SetStreamID (single_data);
		xmlStream.PostMessage (
		shared_ptr<Message>(dsr));
		shared_ptr<Message> Result ( xmlStream.GetMessage ());
		XFTP_ASSERT (Result, "failed to create stream", return NULL);
		
		dt=&GetClient ().CreateData (n);
		cout << "stream created" << endl;
		has_dstream=true;
		dt->Start ();
	}
	dt=&GetClient ().GetExistingData (n);
	return dt;
}
