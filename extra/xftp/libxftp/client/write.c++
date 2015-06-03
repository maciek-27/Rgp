#include "write.h++"

#include<libxftp/commondomfeatures.h++>
#include <libxftp/server/error.h++>
#include <libxftp/server/acknowledgement.h++>
#include <libxftp/xmlstream.h++>
#include <libxftp/servercontrol.h++>
#include <libxftp/client.h++>
#include <algorithm>
using namespace xercesc;

XMLCh * xftp::client::MsgWrite::GetName ()
{
	static XMLCh name[] = {chLatin_w, chLatin_r, chLatin_i, chLatin_t,
	chLatin_e, chNull};
	return name;
}
void xftp::client::MsgWrite::DefaultProcessImpl(ServerControl& t) 
{
	try {
		int result;
		char buf[FIO_BUFSIZE];
		std::cout << "reading..." <<std::flush;
		
			unsigned int file_size=size;
			while ( (result=
			 t.GetClient()[streamid]
			  .GetData(buf,std::min<unsigned int>(sizeof(buf),file_size) ) ))
			{
				file_size-=result;
				//fwrite(buf, 1, result,f);
				t.Write(buf,result);
				std::cout << file_size  << "b left." <<std::endl;
				if (!file_size)
					break;
			}
		//while ( (result = t.Read(buf,sizeof(buf))) >0 )
		//	t.GetClient()[streamid].SendData(buf,result);
		std::cout << "OK" << std::endl;
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
		t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);	;
}

