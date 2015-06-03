#ifndef _SERVER_LIST_H_
#define _SERVER_LIST_H_

#include "message.h++"
#include "server/serverinfo.h++"
#include <list>
#include "libstreamcon/throw.h++"
#include "libstreamcon/threads.h++"
#include <libxftp/xftpcommons.h++>

namespace xftp { namespace server {

	struct Metadata {
		MetadataInfo info;
		std::string filename;
		long int filesize;
	};

	class Node {
	protected:
		Metadata metadata;
		std::list<Node> nodes;
	public:
		Node(const Metadata& _metadata = Metadata()) : metadata(_metadata)
			{;}
		typedef std::list<Node> NodeList;
		std::list<Node>& GetNodes();

		/*
		  \param node
		  \return iterator to the inserted element.
		  Insert a child node.
		*/
		NodeList::iterator PushNode(const Node& node);
		/*!
		  \return iterator to the inserted element.
		  Insert a child node before pos.
		*/
		NodeList::iterator PushNode(NodeList::iterator pos,
									const Node& node);
		
		const Metadata& GetMetadata();
		void SetMetadata(const Metadata&);
		friend class MsgList;
	};
	
	class MsgList : public ServerMessage {
	private:
		Node node;
		int entries;

		__DE(OpenDirError, streamcon::Exception);
		__DE(InvalidDir, OpenDirError);
		__DE(DirAccessDenied, OpenDirError);

		void ParseNodes(xercesc::DOMElement *el, Node& node);
		void ListNodes(DOMSerializedMessage& Result,
					   xercesc::DOMElement *DEnode, Node& node,
					   xercesc::DOMDocument & document);
		void Fill(Node& _node, const std::string& path,
				  int recurse = 0) throw(OpenDirError);

	protected:
	public:
		MsgList(xercesc::DOMElement *elem, size_t ID);
		MsgList();

		void Fill(const std::string& path, int recurse = 0)
			throw(OpenDirError);

		XMLCh * GetName();

		const Node& GetNode() const;
		Node& GetNode();

		Message::DOMSerializedMessage Serialize(xercesc::DOMDocument & document);
		virtual void DefaultProcessImpl(ClientControl& control);
	};
}}
	
#endif // _SERVER_LIST_H_
