#include "data_transfer.h++"
#include <libxftp/allmessages.h++>
#include <xercesc/util/XMLUniDefs.hpp>

using namespace xercesc;

xftp::client::MsgDataTransfer
::MsgDataTransfer(xercesc::DOMElement *elem,size_t ID,ProcessImplType *_PI) : ClientMessage (ID, _PI) 
{
	streamid=atoi(toNative(
			elem->getAttribute((XMLCh[]){chLatin_s, chLatin_t, chLatin_r, chLatin_e, chLatin_a, chLatin_m, chNull})).c_str())	;
	size=atoi(toNative(
			elem->getAttribute((XMLCh[]){chLatin_s, chLatin_i, chLatin_z, chLatin_e, chNull})).c_str())	;
	;
}

xftp::client::MsgDataTransfer::MsgDataTransfer(ProcessImplType *_PI) : ClientMessage (-1, _PI) {;}

xftp::Message::DOMSerializedMessage
xftp::client::MsgDataTransfer::Serialize(xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetClientNamespaceURI());
		
	Result->setAttribute ((XMLCh[]){chLatin_s, chLatin_t, chLatin_r, chLatin_e, chLatin_a, chLatin_m, chNull},
		fromNative(toString(streamid)).c_str()
		);
	Result->setAttribute ((XMLCh[]){chLatin_s, chLatin_i, chLatin_z, chLatin_e, chNull},
		fromNative(toString(size)).c_str()
		);
	return Result;
}

