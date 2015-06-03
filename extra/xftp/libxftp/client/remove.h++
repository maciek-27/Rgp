#ifndef _CLIENT_RM_H_
#define _CLIENT_RM_H_

#include "message.h++"
#include "filename_operations.h++"
namespace xftp { namespace client {
	
	class MsgRemoveItem : public FilenameOperations {
	private:
		std::string filename;

	public:
		MsgRemoveItem(xercesc::DOMElement *elem, size_t ID);
		MsgRemoveItem();
		XMLCh * GetName();
		void DefaultProcessImpl(ServerControl& control);
	};
}}
	
#endif // _CLIENT_OPEN_H_

