#ifndef _XFTP_DATA__TRANSFER__WRITE__
#define _XFTP_DATA__TRANSFER__WRITE__


#include "message.h++"
#include "data_transfer.h++"
namespace xftp { namespace client {
	class MsgWrite : public MsgDataTransfer
	{
	public:
		MsgWrite(xercesc::DOMElement *elem,size_t ID)
		:MsgDataTransfer(elem,ID, ProcessImplDefault(this)) {}
		MsgWrite()
		:MsgDataTransfer(ProcessImplDefault(this)){}
		XMLCh * GetName();
		void DefaultProcessImpl(ServerControl& t);
				
	};
}}


#endif
