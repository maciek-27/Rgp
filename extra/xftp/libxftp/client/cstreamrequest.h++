#ifndef _CLIENT_DSTREAMREQUEST_H_
#define _CLIENT_CSTREAMREQUEST_H_

#include "message.h++"

namespace xftp { namespace client {
	
	class MsgCStreamRequest : public ClientMessage {
	private:
		bool startTLS;
		std::vector<std::string> filters;
		int streamid;//  streamid!= ID
	public:
		MsgCStreamRequest(xercesc::DOMElement *elem, size_t ID);	
		MsgCStreamRequest();		
		XMLCh * GetName();
		int GetStreamID() {return streamid;}
		void SetStreamID(int i) { streamid = i ;}
		bool GetStartTLS();
		const std::vector<std::string>& GetFilters();
		Message::DOMSerializedMessage
		Serialize(xercesc::DOMDocument & document);
		void DefaultProcessImpl(ServerControl& t);
	}; 

}}

#endif // _CLIENT_CSTREAMREQUEST_H_
