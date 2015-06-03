#ifndef _CLIENT_CLOSE_H_
#define _CLIENT_CLOSE_H_

#include "message.h++"

namespace xftp { namespace client {
	
	class MsgClose : public ClientMessage {
	public:
		MsgClose(xercesc::DOMElement *elem, size_t ID);
		XMLCh * GetName();

		MsgClose();
		Message::DOMSerializedMessage Serialize(xercesc::DOMDocument & document);
		void DefaultProcessImpl(ServerControl& control);
	};
}}
	
#endif // _CLIENT_CLOSE_H_
