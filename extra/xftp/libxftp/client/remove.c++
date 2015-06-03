#include "remove.h++"
 
#include<boost/filesystem/operations.hpp>
#include<libxftp/commondomfeatures.h++>
#include <libxftp/client.h++>
#include <libxftp/servercontrol.h++>
#include <libxftp/server/error.h++>
#include <libxftp/server/acknowledgement.h++>
using namespace xftp;
using namespace client;
using namespace xercesc;

MsgRemoveItem::MsgRemoveItem(DOMElement *elem, size_t ID) : FilenameOperations(elem,ID, ProcessImplDefault(this))
{	   
}

MsgRemoveItem::MsgRemoveItem():FilenameOperations(ProcessImplDefault(this)){;}

XMLCh * MsgRemoveItem::GetName()
{
	static XMLCh name[] = {chLatin_r, chLatin_e, chLatin_m, chLatin_o, chLatin_v,  
							chLatin_e,chUnderscore, chLatin_i, chLatin_t, chLatin_e
			, chLatin_m, chNull};
	return name;
}

void MsgRemoveItem::DefaultProcessImpl(ServerControl& control)
{
	
	try {
		boost::filesystem::path path = control.GetDir();
		path /=GetFilename();
		if (boost::filesystem::exists(path))
			boost::filesystem::remove( path);
		else
		{
			server::MsgError * err = new server::MsgError;
			err-> SetPointer(GetID());
			err-> SetTextMsg(std::string("File does not exist "));
			control.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(err),true);
			return;
		}
	}
	catch (std::exception & e){
		server::MsgError * err = new server::MsgError;
		err-> SetPointer(GetID());
		err-> SetTextMsg(std::string("Unexpected difficulity ")+e.what());
		control.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(err),true);
		return;
	}  
	server::MsgAcknowledgement * ack =
		new server::MsgAcknowledgement;
	ack-> SetPointer(GetID());
	control.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);
}
