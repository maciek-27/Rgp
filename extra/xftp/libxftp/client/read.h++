#ifndef _XFTP_DATA__TRANSFER__READ__
#define _XFTP_DATA__TRANSFER__READ__


#include "message.h++"
#include "data_transfer.h++"
namespace xftp { namespace client {
	class MsgRead : public MsgDataTransfer
	{
	public:
		MsgRead(xercesc::DOMElement *elem,size_t ID)
		:MsgDataTransfer(elem,ID, ProcessImplDefault(this)) {}
		MsgRead()
		:MsgDataTransfer(ProcessImplDefault(this)){}
		XMLCh * GetName();
		void DefaultProcessImpl(ServerControl& t);
				
	};
}}


#endif
