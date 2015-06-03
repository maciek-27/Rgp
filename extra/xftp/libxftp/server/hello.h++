#ifndef _SERVER_HELLO_H_
#define _SERVER_HELLO_H_

#include "../message.h++"

namespace xftp { namespace server {
	
	class MsgHello : public ServerMessage {
	private:
		bool tlsAvailable, tlsRequired;
		std::string serverName;
		std::vector<std::string> authMethods;
	public:

		MsgHello() : ServerMessage (-1, ProcessImplDefault(this)){ ; };
		MsgHello(xercesc::DOMElement *elem, size_t ID);
		XMLCh * GetName();
		
		void SetTLS(bool _tlsAvailable, bool _tlsRequired = false);
		bool GetTLS();
		bool GetTLSRequired();
		const std::string& GetServerName();
		void SetServerName(const std::string &_serverName);
		void AddAuthMethod(const std::string &method);
		const std::vector<std::string>& GetAuthMethods();
			  
		Message::DOMSerializedMessage
		Serialize(xercesc::DOMDocument & document);
		
		void DefaultProcessImpl(ClientControl& control);
	}; 

}}

#endif // _SERVER_HELLO_H_
