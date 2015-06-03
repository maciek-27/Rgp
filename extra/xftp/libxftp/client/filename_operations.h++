#ifndef _CLIENT_FILENAME_OPS_H_
#define _CLIENT_FILENAME_OPS_H_

#include "message.h++"

namespace xftp { namespace client {
	
	class FilenameOperations : public ClientMessage {
	private:
		std::string filename;

	public:
		FilenameOperations(xercesc::DOMElement *elem, size_t ID,ProcessImplType *_PI);
		FilenameOperations(ProcessImplType *_PI);
		XMLCh * GetName();

		void SetFilename(const std::string& _filename);
		const std::string& GetFilename();

		Message::DOMSerializedMessage Serialize(xercesc::DOMDocument & document);
	};
}}
	
#endif 
