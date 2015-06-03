#include "dstreamrequest.h++"
#include <libxftp/xftpcommons.h++>
#include <libxftp/client.h++>
#include <libxftp/servercontrol.h++>
#include <libxftp/server/error.h++>
#include <libxftp/server/acknowledgement.h++>
using namespace xftp;
using namespace client;
using namespace xercesc;

MsgDStreamRequest::MsgDStreamRequest(DOMElement *elem, size_t ID)
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
		else if(XMLString::equals(XercesSsie("start_tls").c_str(), el->getTagName())) {
			startTLS = true;
		}
	};
}

MsgDStreamRequest::MsgDStreamRequest()
	: ClientMessage (-1, ProcessImplDefault(this))
{	
	;
}		
	
XMLCh * MsgDStreamRequest::GetName()
{
	static XMLCh name[] = {chLatin_d, chLatin_s, chLatin_t, chLatin_r,
						   chLatin_e, chLatin_a, chLatin_m, chUnderscore,
						   chLatin_r, chLatin_e, chLatin_q, chLatin_u,
						   chLatin_e, chLatin_s, chLatin_t, chNull};
	return name;
}

bool MsgDStreamRequest::GetStartTLS()
{
	return startTLS;
}

void MsgDStreamRequest::SetStartTLS(bool _startTLS)
{
	startTLS = _startTLS;
}

const std::vector<std::string>& MsgDStreamRequest::GetFilters()
{
	return filters;
}


Message::DOMSerializedMessage
MsgDStreamRequest::Serialize(xercesc::DOMDocument & document) {

	DOMSerializedMessage Result (InitializeSerialization(document));
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetClientNamespaceURI());
		
	Result->setAttribute ((XMLCh[]){chLatin_s, chLatin_t, chLatin_r, chLatin_e, chLatin_a, chLatin_m, chLatin_i, chLatin_d, chNull},
		fromNative(toString(streamid)).c_str()
		);
	if (startTLS)
	{
		DOMElement * DEstartTLS
			= document.createElement (XercesSsie("start_tls").c_str());
		Result->appendChild (DEstartTLS);
	}

	return Result;;
};


#define KEYFILE "/home/rush/Programowanie/xftp/trunk/xftp-server/server.pem"
#define PASSWORD "password"
#define DHFILE "/home/rush/Programowanie/xftp/trunk/xftp-server/dh1024.pem"
#define CA_LIST "/home/rush/Programowanie/xftp/trunk/xftp-server/root.pem"

void MsgDStreamRequest::DefaultProcessImpl(ServerControl& t) 
{
	try
	{
/*		if(startTLS) {
			t.GetClient().CreateTLSData(streamid);
			dynamic_cast<TLSStream&>(t.GetClient()[streamid]).InitializeCTX(KEYFILE, DHFILE, PASSWORD, CA_LIST);
		}
		else*/
			t.GetClient().CreateData(streamid);
	}
	catch (xftp::Connection::NoStreamsAvailable)
	{
		server::MsgError * err = new server::MsgError;
		err-> SetPointer(GetID());
		err-> SetTextMsg("No streams available");
		t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(err),true);
	}
	catch (
				streamcon::Connection::StreamIdOccupied)
	{
		;
		server::MsgError * err = new server::MsgError;
		err-> SetPointer(GetID());
		err-> SetTextMsg("stream id already occupied");
		t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(err),true);
	
	}
	catch (
				streamcon::Connection::StreamIdExceeded)
	{
		;
		server::MsgError * err = new server::MsgError;
		err-> SetPointer(GetID());
		err-> SetTextMsg("stream id exceeded");
		t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(err),true);
	}
	server::MsgAcknowledgement * ack =
		new server::MsgAcknowledgement;
	ack-> SetPointer(GetID());
	t.xmlStream.PostMessage( std::tr1::shared_ptr<Message>(ack),true);
/*	try {
		printf("Trying to accept TLS\n");
		dynamic_cast<TLSStream&>(t.GetClient()[streamid]).AcceptTLS();
		printf("TLS accepted\n");
	}
	catch(...)
	{
		;
		}*/
}
