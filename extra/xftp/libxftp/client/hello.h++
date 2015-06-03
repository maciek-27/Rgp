#ifndef _CLIENT_HELLO_H_
#define _CLIENT_HELLO_H_

#include "message.h++"

namespace xftp { namespace client {
	
	class MsgHello : public ClientMessage {
	private:
		std::string userAgent;

	public:
		MsgHello(xercesc::DOMElement *elem, size_t ID);
		MsgHello();
		XMLCh * GetName();

		void SetUserAgent(const std::string& _userAgent);
		const std::string& GetUserAgent();

		Message::DOMSerializedMessage Serialize(xercesc::DOMDocument & document);
		void DefaultProcessImpl(ServerControl& control);
	};
}}
	
#endif // _CLIENT_HELLO_H_
