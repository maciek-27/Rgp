#ifndef _CLIENT_LIST_H_
#define _CLIENT_LIST_H_

#include "message.h++"
#include "server/serverinfo.h++"
namespace xftp { namespace client {
	
	class MsgList : public ClientMessage {
	private:
		std::tr1::shared_ptr<std::string> dir;
		int recurse;
		MetadataInfo metaDataRequest;
		std::pair<int, int> range;

	public:
		MsgList(xercesc::DOMElement *elem, size_t ID);
		
		MsgList();
		
		XMLCh * GetName();
		
		inline void SetRecurse(int i){recurse=i;}
		inline void SetDir(const std::string& s)
		{
			if (not dir)
				dir.reset(new std::string(s));
			else
				dir->assign(s);
		}
		const std::string& GetDir();
		int GetRecurse();
		const MetadataInfo& GetMetaDataRequest();
		std::pair<int, int> GetRange();
		int GetFrom();
		int GetTo();
		
		virtual
		DOMSerializedMessage
		Serialize(xercesc::DOMDocument & document);
		void DefaultProcessImpl(ServerControl& );
	};
}}
	
#endif // _CLIENT_LIST_H_
