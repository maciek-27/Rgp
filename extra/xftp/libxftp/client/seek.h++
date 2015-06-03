#ifndef _CLIENT_SEEK_H_
#define _CLIENT_SEEK_H_

#include "message.h++"

namespace xftp { namespace client {
	
	class MsgSeek : public ClientMessage {
	private:		
		typedef enum {End = -1, Begin = -2} OffsetType;		
		union {
			int value; // -1 for END, -2 for BEGIN
			OffsetType type;
		} offset;
		std::string userAgent;
	public:
		MsgSeek(xercesc::DOMElement *elem, size_t ID);
		MsgSeek():ClientMessage (-1, ProcessImplDefault(this)){}
		XMLCh * GetName();

		void SetOffset(OffsetType type);
		void SetOffset(int value);		
		int GetOffset();

		Message::DOMSerializedMessage Serialize(xercesc::DOMDocument & document);
		void DefaultProcessImpl(ServerControl& control);
	};
}}
	
#endif // _CLIENT_SEEK_H_
