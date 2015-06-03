#include "cstreamrequest.h++"
#include <libxftp/xftpcommons.h++>
using namespace xftp;
using namespace client;
using namespace xercesc;

MsgCStreamRequest::MsgCStreamRequest(DOMElement *elem, size_t ID)
	: ClientMessage (ID, ProcessImplDefault(this))
{	
		
	streamid=atoi(toNative(
			elem->getAttribute((XMLCh[]){chLatin_s, chLatin_t, chLatin_r, chLatin_e, chLatin_a, chLatin_m, chLatin_i, chLatin_d, chNull})).c_str())	;
	DOMNodeList* children = elem->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{
		DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
		if (not el)
			continue;
		
		if(XMLString::equals((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_t, chLatin_e, chLatin_r, chLatin_s, chNull}, 
							 el->getTagName())) {
			DOMNodeList* children = el->getChildNodes();
			const  XMLSize_t nodeCount = children->getLength();
			for( XMLSize_t i = 0; i < nodeCount; ++i )
			{
				if (children->item(i)-> getNodeType() != DOMNode:: ELEMENT_NODE)
					continue;
				filters.push_back(
					toNative(dynamic_cast<DOMCharacterData*>
							 (children->item(i)->getFirstChild())->getData()));
			}
		}
	};
}

MsgCStreamRequest::MsgCStreamRequest()
	: ClientMessage (-1, ProcessImplDefault(this))
{	
	;
}		
	
XMLCh * MsgCStreamRequest::GetName()
{
	static XMLCh name[] = {chLatin_d, chLatin_s, chLatin_t, chLatin_r,
						   chLatin_e, chLatin_a, chLatin_m, chUnderscore,
						   chLatin_r, chLatin_e, chLatin_q, chLatin_u,
						   chLatin_e, chLatin_s, chLatin_t, chNull};
	return name;
}

bool MsgCStreamRequest::GetStartTLS()
{
	return startTLS;
}

const std::vector<std::string>& MsgCStreamRequest::GetFilters()
{
	return filters;
}


Message::DOMSerializedMessage
MsgCStreamRequest::Serialize(xercesc::DOMDocument & document) {

	DOMSerializedMessage Result (InitializeSerialization(document));
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetClientNamespaceURI());
		
	Result->setAttribute ((XMLCh[]){chLatin_s, chLatin_t, chLatin_r, chLatin_e, chLatin_a, chLatin_m, chLatin_i, chLatin_d, chNull},
		fromNative(toString(streamid)).c_str()
		);
	return Result;;
};


void MsgCStreamRequest::DefaultProcessImpl(ServerControl& t)
{

}
