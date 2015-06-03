#include "open.h++"

#include<libxftp/commondomfeatures.h++>
#include <libxftp/client.h++>
#include <libxftp/servercontrol.h++>
#include <libxftp/server/error.h++>
#include <libxftp/server/acknowledgement.h++>
using namespace xftp;
using namespace client;
using namespace xercesc;

MsgOpen::MsgOpen(DOMElement *elem, size_t ID) : FilenameOperations(elem,ID, ProcessImplDefault(this))
{	   
}

MsgOpen::MsgOpen():FilenameOperations(ProcessImplDefault(this)){;}

XMLCh * MsgOpen::GetName()
{
	static XMLCh name[] = {chLatin_o, chLatin_p, chLatin_e, chLatin_n
						   , chNull};
	return name;
}

void MsgOpen::DefaultProcessImpl(ServerControl& control)
{
	
	try {
		boost::filesystem::path path(control.GetDir());
		path /=GetFilename();
		control.Open( path);
	}
	catch (std::exception & e){
		server::MsgError * err = new server::MsgError;
		err-> SetPointer(GetID());
		err-> SetTextMsg(std::string("Unexpected difficulity")+e.what());
		control.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(err),true);
		return;
	}  
		server::MsgAcknowledgement * ack =
			new server::MsgAcknowledgement;
		ack-> SetPointer(GetID());
		control.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);
}
