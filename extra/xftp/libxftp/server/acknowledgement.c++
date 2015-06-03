#include "acknowledgement.h++"
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

MsgAcknowledgement::MsgAcknowledgement (DOMElement *elem, size_t ID) : ServerMessage (ID, ProcessImplDefault(this))
{
	const XMLCh *ptr= elem->getAttribute((XMLCh[]){chLatin_p, chLatin_o, chLatin_i, chLatin_n, chLatin_t, chLatin_e, chLatin_r, chNull});
	if(ptr)
		pointer = atoi(toNative(ptr).c_str());
}

XMLCh * MsgAcknowledgement::GetName ()
{
	static XMLCh name[] = {chLatin_a, chLatin_c, chLatin_k,
						   chLatin_n, chLatin_o, chLatin_w,
						   chLatin_l, chLatin_e, chLatin_d,
						   chLatin_g, chLatin_e, chLatin_m,
						   chLatin_e, chLatin_n, chLatin_t,
						   chNull};
	return name;
}

uint32_t MsgAcknowledgement::GetPointer() const
{
	return pointer;
}

void MsgAcknowledgement::SetPointer(uint32_t _pointer)
{
	pointer = _pointer;
}


Message::DOMSerializedMessage MsgAcknowledgement::Serialize (xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetServerNamespaceURI());

	Result->setAttribute((XMLCh[]){chLatin_p, chLatin_o, chLatin_i, chLatin_n, chLatin_t, chLatin_e, chLatin_r, chNull}, 
						 fromNative(toString(pointer)).c_str());
	
	return Result;
};

void MsgAcknowledgement::DefaultProcessImpl(ClientControl& control)
{

};
