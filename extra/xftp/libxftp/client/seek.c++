#include "seek.h++"

#include<libxftp/commondomfeatures.h++>
#include<libxftp/xftpcommons.h++>
using namespace xftp;
using namespace client;
using namespace xercesc;

MsgSeek::MsgSeek(DOMElement *elem, size_t ID) : ClientMessage (ID, ProcessImplDefault(this))
{	   
	std::string str = toNative(
		elem->getAttribute((XMLCh[]){chLatin_o, chLatin_f, chLatin_f, chLatin_s, chLatin_e, chLatin_t, chNull}));
	if(str == "end") {
		offset.type = End;
	}
	else if(str == "begin") {
		offset.type = Begin;
	}
	else {
		offset.value = atoi(str.c_str());
	}   
}

XMLCh * MsgSeek::GetName()
{
	static XMLCh name[] = {chLatin_s, chLatin_e, chLatin_e, chLatin_k,
						   chNull};
	return name;
}

void MsgSeek::SetOffset(OffsetType type)
{
	offset.type = type;
}

void MsgSeek::SetOffset(int value)
{
	offset.value = value;
}

int MsgSeek::GetOffset()
{
	return offset.value;
}

Message::DOMSerializedMessage MsgSeek::Serialize(xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
	
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetClientNamespaceURI());

	if(offset.type == End) {
		Result->setAttribute ((XMLCh[]){chLatin_o, chLatin_f, chLatin_f, chLatin_s, chLatin_e, chLatin_t, chNull},
							  (XMLCh[]){chLatin_e, chLatin_n, chLatin_d, chNull});
	}
	else if(offset.type == Begin) {
		Result->setAttribute ((XMLCh[]){chLatin_o, chLatin_f, chLatin_f, chLatin_s, chLatin_e, chLatin_t, chNull},
							  (XMLCh[]){chLatin_b, chLatin_e, chLatin_g, chLatin_i, chLatin_n, chNull});
	}
	else {
		Result->setAttribute ((XMLCh[]){chLatin_o, chLatin_f, chLatin_f, chLatin_s, chLatin_e, chLatin_t, chNull},
							  fromNative(
								  toString(offset.value)).c_str()
			);
	}
	return Result;
};

void MsgSeek::DefaultProcessImpl(ServerControl& control)
{
	;
}
