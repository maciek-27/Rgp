#ifndef _CLIENT_DSTREAMREQUEST_H_
#define _CLIENT_DSTREAMREQUEST_H_

#include "message.h++"

namespace xftp { namespace client {
	
	class MsgDStreamRequest : public ClientMessage {
	private:
		bool startTLS;
		std::vector<std::string> filters;
		int streamid;//  streamid!= ID
	public:
		MsgDStreamRequest(xercesc::DOMElement *elem, size_t ID);	
		MsgDStreamRequest();		
		XMLCh * GetName();
		int GetStreamID() {return streamid;}
		void SetStreamID(int i) { streamid = i ;}
		bool GetStartTLS();
		void SetStartTLS(bool _startTLS);
		const std::vector<std::string>& GetFilters();
		Message::DOMSerializedMessage
		Serialize(xercesc::DOMDocument & document);
		void DefaultProcessImpl(ServerControl& t);
	}; 

}}

#endif // _CLIENT_DSTREAMREQUEST_H_
