#include "authrefuse.h++"

#include<libxftp/commondomfeatures.h++>
using namespace xftp;
using namespace server;
using namespace xercesc;

MsgAuthRefuse::MsgAuthRefuse(DOMElement *elem, size_t ID) :
	ServerMessage (ID, ProcessImplDefault(this))
{
	DOMNodeList* children = elem->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{		
		DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
		if(!el)
			continue;
		if(XMLString::equals((XMLCh[]){chLatin_r, chLatin_e, chLatin_a, chLatin_s, chLatin_o, chLatin_n, chNull}, 
							 el->getTagName()))
		{
			reason.assign(
				toNative(dynamic_cast<DOMCharacterData*>
									 (el->getFirstChild())->getData()));
			
			const XMLCh* msgid =
				el->getAttribute( (XMLCh[]){chLatin_m, chLatin_s, chLatin_g, chLatin_i, chLatin_d, chNull});
			reasonid = atoi(toNative(msgid).c_str());  
		}						
	}
}

MsgAuthRefuse::MsgAuthRefuse() :
	ServerMessage (-1, ProcessImplDefault(this))
{
	;
}

XMLCh * MsgAuthRefuse::GetName()
{
	static XMLCh name[] = {chLatin_a, chLatin_u, chLatin_t, chLatin_h, 
						   chUnderscore, chLatin_r, chLatin_e, chLatin_f,
						   chLatin_u, chLatin_s, chLatin_e};
	return name;
}

const std::string& MsgAuthRefuse::GetReason()
{
	return reason;
}

void MsgAuthRefuse::SetReason(const std::string& str)
{
	reason = str;
}

int MsgAuthRefuse::GetReasonId()
{
	return reasonid;
}


Message::DOMSerializedMessage MsgAuthRefuse::Serialize(xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetServerNamespaceURI());
	Result->appendChild (
		document.createTextNode(fromNative(reason).c_str())
		);
	
	char number[10];
	snprintf(number, 10, "%i", reasonid);	
	Result->setAttribute(
		(XMLCh[]){chLatin_r, chLatin_e, chLatin_a, chLatin_s, chLatin_o, chLatin_n, chLatin_i, chLatin_d, chNull}, fromNative(std::string(number)).c_str()
		);

	return Result;
}

void MsgAuthRefuse::DefaultProcessImpl(ClientControl& control)
{
	;
}
