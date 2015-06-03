#ifndef _CLIENT_DATA_TRANSFER_
#define _CLIENT_DATA_TRANSFER_


#include "message.h++"

namespace xftp { namespace client {
	
	class MsgDataTransfer : public ClientMessage
	{
	protected:
		MsgDataTransfer(xercesc::DOMElement *elem,size_t ID,ProcessImplType *_PI);	
		MsgDataTransfer(ProcessImplType *_PI);
		int size;
		int streamid;
	public:
		inline int GetSize() {return size;}
		inline int GetStreamID(){return streamid;}
		
		inline void SetSize(int i) {size=i;}
		inline void SetStreamID(int i) {streamid=i;}
		Message::DOMSerializedMessage
		Serialize(xercesc::DOMDocument & document);
	};
	
}}


#endif
