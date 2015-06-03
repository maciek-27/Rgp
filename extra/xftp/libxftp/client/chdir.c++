#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "chdir.h++"
#include<libxftp/commondomfeatures.h++>
#include <libxftp/server/error.h++>
#include <libxftp/server/acknowledgement.h++>
#include <libxftp/xmlstream.h++>
#include <libxftp/servercontrol.h++>
using namespace xftp;
using namespace client;
using namespace xercesc;

MsgChDir::MsgChDir(DOMElement *elem, size_t ID) : ClientMessage (ID, ProcessImplDefault(this))
{	
	DOMNodeList* children = elem->getChildNodes();
	
	const  XMLSize_t nodeCount = children->getLength();
	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{		
		
		DOMText * tn = dynamic_cast<DOMText *>(children->item(i));
		if (tn)
		{
			path=toNative(tn->getData());
			break;
		}
	}
	;
}

MsgChDir::MsgChDir(std::string & p) : ClientMessage (-1, ProcessImplDefault(this))
{
	path=p;
}
XMLCh * MsgChDir::GetName()
{
	static XMLCh name[] = {chLatin_c, chLatin_h, chLatin_d, chLatin_i,
						   chLatin_r, chNull};
	return name;
}

const std::string& MsgChDir::GetPath()
{
	return path;
}

Message::DOMSerializedMessage MsgChDir::Serialize(xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetClientNamespaceURI());
		
	Result->appendChild (
		document.createTextNode(fromNative(path).c_str())
		);
	return Result;;
};


void MsgChDir::DefaultProcessImpl(ServerControl& t)
{
	std::cout << "cd" << path << std::endl;

	try {
		boost::filesystem::path newdir(path);
		t.ChDir(newdir);
		server::MsgAcknowledgement * ack =
			new server::MsgAcknowledgement;
		ack-> SetPointer(GetID());
		t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);

	}
	catch (DirAccessDenied){
		server::MsgError * ack = new server::MsgError;
		ack-> SetPointer(GetID());
		ack-> SetTextMsg(stringError(EACCES));
		t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);
	}  
	catch (InvalidDir){
		server::MsgError * ack = new server::MsgError;
		ack-> SetPointer(GetID());
		ack-> SetTextMsg(stringError(ENOTDIR));
		t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);
	}  
	catch (OpenDirError){
		server::MsgError * ack = new server::MsgError;
		ack-> SetPointer(GetID());
		ack-> SetTextMsg("Unknown error");
		t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);
	}  
}



