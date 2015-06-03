#ifndef _CLIENT_CHDIR_H_
#define _CLIENT_CHDIR_H_

#include "message.h++"

namespace xftp { namespace client {
	
	class MsgChDir : public ClientMessage {
	private:
		std::string path;

	public:
		MsgChDir(xercesc::DOMElement *elem, size_t ID);

		MsgChDir(std::string & p);
		
		MsgChDir():ClientMessage (-1, ProcessImplDefault(this)){}
		
		XMLCh * GetName();

		const std::string& GetPath();
		
		Message::DOMSerializedMessage Serialize(xercesc::DOMDocument & document);
		
		void DefaultProcessImpl(ServerControl& t);
	};
}}
	
#endif // _CLIENT_CHDIR_H_
