#ifndef _SERVER_AUTHREFUSE_H_
#define _SERVER_AUTHREFUSE_H_

#include "message.h++"

namespace xftp { namespace server {
	class MsgAuthRefuse : public ServerMessage {
	private:
		std::string reason;
		int reasonid;
	public:
		MsgAuthRefuse(xercesc::DOMElement *elem, size_t ID);
		MsgAuthRefuse();

		XMLCh * GetName();

		const std::string& GetReason();
		void SetReason(const std::string& str);

		int GetReasonId();

		DOMSerializedMessage
		Serialize(xercesc::DOMDocument & document);
		virtual void DefaultProcessImpl(ClientControl& control);
	};
		
}}

#endif // _SERVER_AUTHREFUSE_H_
