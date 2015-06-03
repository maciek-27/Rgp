#ifndef _SERVER_SERVERINFO_H_
#define _SERVER_SERVERINFO_H_

#include "message.h++"

namespace xftp {
	struct MetadataInfo {
		struct  {
			bool branch;
			bool data;
			bool link;
		} flags;
		bool filename;
		bool filesize;
	};
}

namespace xftp {  namespace server {
	

	class MsgServerInfo : public ServerMessage {
	private:
		std::string rootDir;
		std::string startingDir;
		MetadataInfo metaData;		

	public:
		MsgServerInfo(xercesc::DOMElement *elem, size_t ID);
		MsgServerInfo();

		XMLCh * GetName();
		
		const std::string& GetRootDir();
		const std::string& GetStartingDir();
		const MetadataInfo& GetMetaData();
			  
		Message::DOMSerializedMessage Serialize(xercesc::DOMDocument & document);
		virtual void DefaultProcessImpl(ClientControl& control);
	}; 

}}


#endif // _SERVER_SERVERINFO_H_
