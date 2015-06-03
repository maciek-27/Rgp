#ifndef _CLIENT_OPEN_H_
#define _CLIENT_OPEN_H_

#include "message.h++"
#include "filename_operations.h++"
namespace xftp { namespace client {
	
	class MsgOpen : public FilenameOperations {
	private:
		std::string filename;

	public:
		MsgOpen(xercesc::DOMElement *elem, size_t ID);
		MsgOpen();
		XMLCh * GetName();
		void DefaultProcessImpl(ServerControl& control);
	};
}}
	
#endif // _CLIENT_OPEN_H_

