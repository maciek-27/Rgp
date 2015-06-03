#ifndef _SERVER_ACKNOWLEDGEMENT_H_
#define _SERVER_ACKNOWLEDGEMENT_H_

#include "../message.h++"


namespace xftp { namespace server {
	
	class MsgAcknowledgement : public ServerMessage {
	private:
		uint32_t pointer;

	public:
		explicit
		MsgAcknowledgement() : ServerMessage (-1, ProcessImplDefault(this)){ ; };
		MsgAcknowledgement(xercesc::DOMElement *elem, size_t ID);
		XMLCh * GetName();
		
		uint32_t GetPointer() const;
		void SetPointer(uint32_t _pointer);
			  
		Message::DOMSerializedMessage
		Serialize(xercesc::DOMDocument & document);
		
		void DefaultProcessImpl(ClientControl& control);
	}; 

}}

#endif // _SERVER_ACKNOWLEDGEMENT_H_
