#ifndef _CLIENT_CREAT_DIR_H_
#define _CLIENT_CRRAT_DIR_H_

#include "message.h++"
#include "filename_operations.h++"
namespace xftp { namespace client {
	
	class MsgCreateDirectory : public FilenameOperations {
	private:
		std::string filename;

	public:
		MsgCreateDirectory(xercesc::DOMElement *elem, size_t ID);
		MsgCreateDirectory();
		XMLCh * GetName();
		void DefaultProcessImpl(ServerControl& control);
	};
}}
	
#endif // _CLIENT_OPEN_H_

