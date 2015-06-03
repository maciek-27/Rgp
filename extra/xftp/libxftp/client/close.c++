#include "close.h++"

#include <libxftp/server/error.h++>
#include <libxftp/server/acknowledgement.h++>
#include <libxftp/xmlstream.h++>
#include <libxftp/servercontrol.h++>
#include<libxftp/commondomfeatures.h++>
using namespace xftp;
using namespace client;
using namespace xercesc;

MsgClose::MsgClose(DOMElement *elem, size_t ID) : ClientMessage (ID, ProcessImplDefault(this))
{	   
	;
}

MsgClose::MsgClose(): ClientMessage (-1, ProcessImplDefault(this)) {}
XMLCh * MsgClose::GetName()
{
	static XMLCh name[] = {chLatin_c, chLatin_l, chLatin_o, chLatin_s
						   ,chLatin_e, chNull};
	return name;
}

Message::DOMSerializedMessage MsgClose::Serialize(xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetClientNamespaceURI());
	return Result;
};

void MsgClose::DefaultProcessImpl(ServerControl& control)
{
	try {
		
		control.Close();//control.Read();
		
	}
	catch (...){
		server::MsgError * err = new server::MsgError;
		err-> SetPointer(GetID());
		err-> SetTextMsg("Unexpected difficulity");
		control.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(err),true);
		return;
	}  
		server::MsgAcknowledgement * ack =
			new server::MsgAcknowledgement;
		ack-> SetPointer(GetID());
		control.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);
	;
}
