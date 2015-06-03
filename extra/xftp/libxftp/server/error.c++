#include "error.h++"
#include <xercesc/dom/DOMCharacterData.hpp>
#include <boost/thread/detail/singleton.hpp>
#include<libxftp/commondomfeatures.h++>
#include "../commondomfeatures.h++"
#include "../xercesstrings.h++"
#include <libxftp/xftpcommons.h++>

using namespace xftp;
using namespace server;
using namespace xercesc;

#include <iostream>
using namespace std;

MsgError::MsgError (DOMElement *elem, size_t ID) : ServerMessage (ID, ProcessImplDefault(this))
{

	const XMLCh *ptr= elem->getAttribute((XMLCh[]){chLatin_p, chLatin_o, chLatin_i, chLatin_n, chLatin_t, chLatin_e, chLatin_r, chNull});
	if(ptr)
		pointer = atoi(toNative(ptr).c_str());

	DOMNodeList* children = elem->getChildNodes ();
	const  XMLSize_t nodeCount = children->getLength ();
	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{
		if (children->item (i)-> getNodeType ()!=DOMNode:: ELEMENT_NODE )
			continue;
		DOMElement * el = dynamic_cast<DOMElement *>(children->item (i));
		if (XMLString::equals ( (XMLCh[]){chLatin_t, chLatin_e, chLatin_x, chLatin_t, chUnderscore, chLatin_m, chLatin_s, chLatin_g, chNull},
				el->getTagName ()))
			
		{
			DOMCharacterData * __textMsg = 
			dynamic_cast<DOMCharacterData*>(el->getFirstChild ());
			if (__textMsg)
				textMsg = toNative (__textMsg->getData ());
			break;
		}
	}
}

XMLCh * MsgError::GetName ()
{
	static XMLCh name[] = {chLatin_e, chLatin_r, chLatin_r, chLatin_o,
	chLatin_r, chNull};
	return name;
}


uint32_t MsgError::GetPointer() const
{
	return pointer;
}

void MsgError::SetPointer(uint32_t _pointer)
{
	pointer = _pointer;
}


const std::string& MsgError::GetTextMsg ()
{
	return textMsg;
}

void MsgError::SetTextMsg(const std::string& msg)
{
	textMsg = msg;
}


Message::DOMSerializedMessage MsgError::Serialize (xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetServerNamespaceURI());

	Result->setAttribute((XMLCh[]){chLatin_p, chLatin_o, chLatin_i, chLatin_n, chLatin_t, chLatin_e, chLatin_r, chNull}, 
						 fromNative(toString(pointer)).c_str());

	DOMElement * DEtextMsg =
			document.createElement ((XMLCh[]){chLatin_t, chLatin_e, chLatin_x, chLatin_t, chUnderscore, chLatin_m, chLatin_s, chLatin_g, chNull});
	DEtextMsg->appendChild(
			document.createTextNode(fromNative(textMsg).c_str()));
	Result->appendChild (DEtextMsg);
	
	return Result;
};

void MsgError::DefaultProcessImpl(ClientControl& control)
{

};
