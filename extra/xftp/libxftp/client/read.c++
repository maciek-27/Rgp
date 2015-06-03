#include "read.h++"

#include<libxftp/commondomfeatures.h++>
#include <libxftp/server/error.h++>
#include <libxftp/server/acknowledgement.h++>
#include <libxftp/xmlstream.h++>
#include <libxftp/servercontrol.h++>
#include <libxftp/client.h++>
using namespace xercesc;

XMLCh * xftp::client::MsgRead::GetName ()
{
	static XMLCh name[] = {chLatin_r, chLatin_e, chLatin_a, chLatin_d,
	chNull};
	return name;
}

void xftp::client::MsgRead::DefaultProcessImpl(ServerControl& t) 

{
	try {
		int result;
		char buf[FIO_BUFSIZE];
		std::cout << "reading..." <<std::flush;
		
		int rsize;
		for ( rsize=0; (result = t.Read(buf,sizeof(buf))) >0 ; rsize+=result)
			t.GetClient()[streamid].SendData(buf,result);
		std::cout << "OK (sent " << rsize << "b)"<< std::endl;
	}
	catch (std::exception & e){
		server::MsgError * err = new server::MsgError;
		err-> SetPointer(GetID());
		err-> SetTextMsg(std::string("Unexpected difficulity")+e.what());
		t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(err),true);
		return;
	}  
		server::MsgAcknowledgement * ack =
			new server::MsgAcknowledgement;
		ack-> SetPointer(GetID());
		t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);
}
