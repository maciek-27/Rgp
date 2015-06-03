#include "hello.h++"

#include<libxftp/commondomfeatures.h++>
#include <libxftp/server/hello.h++>
#include <libxftp/servercontrol.h++>
using namespace xftp;
using namespace client;
using namespace xercesc;

MsgHello::MsgHello(DOMElement *elem, size_t ID) : ClientMessage (ID, ProcessImplDefault(this))
{	   
	DOMNodeList* children = elem->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{		
		DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
		if (children->item (i)-> getNodeType ()!=DOMNode:: ELEMENT_NODE )
			continue;
		if(XMLString::equals((XMLCh[]){chLatin_u, chLatin_s, chLatin_e, chLatin_r, chUnderscore, chLatin_a, chLatin_g, chLatin_e, chLatin_n, chLatin_t, chNull}, 
							 el->getTagName()))
		{
			if(el->getFirstChild()) {
				userAgent = 
					toNative(dynamic_cast<DOMCharacterData*>
							 (el->getFirstChild())->getData());
			}
			
		}
	}
}

MsgHello::MsgHello() : ClientMessage (-1, ProcessImplDefault(this))
{
	;
}

XMLCh * MsgHello::GetName()
{
	static XMLCh name[] = {chLatin_h, chLatin_e, chLatin_l, chLatin_l,
						   chLatin_o, chNull};
	return name;
}

void MsgHello::SetUserAgent(const std::string& _userAgent)
{
	userAgent = _userAgent;
}

const std::string& MsgHello::GetUserAgent()
{
	return userAgent;
}

Message::DOMSerializedMessage MsgHello::Serialize(xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetClientNamespaceURI());
	DOMElement * DEuserAgent =
		document.createElement ((XMLCh[]){chLatin_u, chLatin_s, chLatin_e, chLatin_r, chUnderscore, chLatin_a, chLatin_g, chLatin_e, chLatin_n, chLatin_t, chNull});
	DEuserAgent->appendChild(
		document.createTextNode(fromNative(userAgent).c_str()));	
	Result->appendChild (DEuserAgent);
	
	return Result;
};

void MsgHello::DefaultProcessImpl(ServerControl& control)
{
	server::MsgHello *hello = new server::MsgHello;
	std::tr1::shared_ptr<Message> l(hello);

	std::cout << "UserAgent: " << GetUserAgent() << std::endl;
	
	hello->SetServerName("XFTP Demo Server - Oprogramowanie Systemowe 2008");
	hello->AddAuthMethod("PLAIN");

	control.xmlStream.PostMessage(l);
}
