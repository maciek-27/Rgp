#ifndef _SERVER_ERROR_H_
#define _SERVER_ERROR_H_

#include "../message.h++"

namespace xftp { namespace server {
	
	class MsgError : public ServerMessage {
	private:
		std::string textMsg;
		uint32_t pointer;
		
	public:

		MsgError() : ServerMessage (-1, ProcessImplDefault(this)){ ; };
		MsgError(xercesc::DOMElement *elem, size_t ID);
		XMLCh * GetName();
		
		uint32_t GetPointer() const;
		void SetPointer(uint32_t _pointer);
		const std::string& GetTextMsg();
		void SetTextMsg(const std::string& msg);

		bool GetTLS();
		bool GetTLSRequired();
		const std::string& GetServerName();
		const std::vector<std::string>& GetAuthMethods();
			  
		Message::DOMSerializedMessage
		Serialize(xercesc::DOMDocument & document);
		
		void DefaultProcessImpl(ClientControl& control);
	}; 

}}

#endif // _SERVER_ERROR_H_
