#include "list.h++"

#include <dirent.h>
#include <sys/types.h>
#include <stddef.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <libxftp/commondomfeatures.h++>

using namespace xftp;
using namespace server;
using namespace xercesc;
using namespace streamcon;

void MsgList::ParseNodes(DOMElement *el, Node& node)
{
	DOMNodeList* children = el->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		if (children->item (i)-> getNodeType ()!=DOMNode:: ELEMENT_NODE )
			continue;
		
		DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
		
				if(XMLString::equals((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull}, 
									 el->getTagName()))
				{
					node.metadata.info.filename = true;
					node.metadata.filename.assign(
						toNative(dynamic_cast<DOMCharacterData*>
								 (el->getFirstChild())->getData()));
				}
				else if(XMLString::equals ((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_s, chLatin_i, chLatin_z, chLatin_e, chNull},
									 el->getTagName()))
				{
					node.metadata.info.filesize = true;
					node.metadata.filesize = 
						atoi(toNative(dynamic_cast<DOMCharacterData*>
								 (el->getFirstChild())->getData()).c_str());
				}
		else if(XMLString::equals((XMLCh[]){chLatin_n, chLatin_o, chLatin_d, chLatin_e, chLatin_s, chNull}, 
								  el->getTagName())) 
		{
			DOMNodeList* children = el->getChildNodes();
			const  XMLSize_t nodeCount = children->getLength();
			for( XMLSize_t i = 0; i < nodeCount; ++i )
			{		
				
				DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));				
				if (children->item (i)-> getNodeType ()!=DOMNode:: ELEMENT_NODE )
					continue;
				node.nodes.push_back(Node()); // add element to the list
				ParseNodes(el, node.nodes.back()); // and recurse to it

			}
		}
	}
}

MsgList::MsgList(DOMElement *elem, size_t ID) : ServerMessage (ID, ProcessImplDefault(this)), entries(0)
{	
	DOMNodeList* children = elem->getChildNodes();
	const  XMLSize_t nodeCount = children->getLength();
	for( XMLSize_t i = 0; i < nodeCount; ++i )
	{		
		DOMElement * el = dynamic_cast<DOMElement *>(children->item(i));
		if (children->item (i)-> getNodeType ()!=DOMNode:: ELEMENT_NODE )
			continue;

		if(XMLString::equals((XMLCh[]){chLatin_n, chLatin_o, chLatin_d, chLatin_e, chNull}, 
							 el->getTagName()))
			ParseNodes(el, node);
	}
}

MsgList::MsgList() : ServerMessage (-1, ProcessImplDefault(this)) , entries(0)
{
	;
}

void MsgList::Fill(Node& _node, const std::string& path,
				   int recurse) throw(OpenDirError)
{
	DIR *d;
	{
		ScopedLock lock(opendir_Mutex);		
		d = opendir(path.c_str());
		if(!d) {
			if(errno == EACCES)
				THROW(DirAccessDenied);
			else if(errno == ENOTDIR || errno == ENOENT)
				THROW(InvalidDir);
			else
				THROW(OpenDirError);			
		}
	}

	std::cout << "\rFilling list (" << entries << ") (" << recurse << ") - ";
	struct dirent dent;
	struct stat st;
	Metadata meta;

	struct dirent *pos;

	
	/* NOTE: There may be issues with readdir_r on Solaris */
	/* See: http://womble.decadentplace.org.uk/readdir_r-advisory.html */
	while(!readdir_r(d, &dent, &pos)) {
		if(!pos) // end of list
			break;
		
		lstat((path + "/" + std::string(dent.d_name)).c_str(), &st);

		meta.info.filename = true;
		meta.filename = dent.d_name;
		if(meta.filename == "." || meta.filename == "..") {
			continue;
		}
		
		std::cout << "\r!Filling list (" << ++entries << ") (" << recurse << ") - " << path;

		meta.info.filesize = true;
		meta.filesize = st.st_size;

		if(S_ISDIR(st.st_mode))
			meta.info.flags.branch = true;
		else
			meta.info.flags.branch = false;

		if(S_ISLNK(st.st_mode))
			meta.info.flags.link = true;
		else
			meta.info.flags.link = false;

		if(S_ISREG(st.st_mode))
			meta.info.flags.data = true;	   
		else
			meta.info.flags.data = false;
		
		Node newnode(meta);
		
		_node.PushNode(newnode);
	}

	closedir(d);

	for(Node::NodeList::iterator i = _node.GetNodes().begin();
		i != _node.GetNodes().end(); ++i) {
		Node &newnode = *i;
		if(recurse && newnode.metadata.info.flags.branch
				&& not newnode.metadata.info.flags.link) {
			try {
				Fill(newnode, path + "/" +
					 newnode.metadata.filename, recurse-1);
			}
			catch(DirAccessDenied) {
				;
			}
			catch(InvalidDir) {
				;
			}
		}
	}
}

void MsgList::Fill(const std::string& path, int recurse)
	throw(OpenDirError)
{
	node.metadata.info.flags.branch = true;
	node.metadata.info.flags.link = false;
	node.metadata.info.filename = true;
	node.metadata.filename = path;
	
	struct stat st;
	if(stat(path.c_str(), &st)) 
	{
		
//		std::cout << "stat failed for" << path << std::endl;
		if(errno == EACCES)
			THROW(DirAccessDenied);
		else if(errno == ENOTDIR || errno == ENOENT)
			THROW(InvalidDir);
		else
			THROW(OpenDirError);			
	}
	node.metadata.filesize = st.st_size;

	if(S_ISDIR(st.st_mode)) {
		Fill(node, path, recurse);
		std::cout << std::endl;
	}
	else
	{
//		std::cout << "single file listing" << std::endl;
		node.metadata.info.flags.branch = false;
		node.metadata.info.filesize = true;
		if(S_ISREG(st.st_mode))
			node.metadata.info.flags.data = true;	   
		else
			node.metadata.info.flags.data = false;
	}
}

XMLCh * MsgList::GetName()
{
	static XMLCh name[] = {chLatin_l, chLatin_i, chLatin_s, chLatin_t,
						    chNull};
	return name;
}

std::list<Node>& Node::GetNodes()
{
	return nodes;
}

const Metadata& Node::GetMetadata()
{
	return metadata;
}

void Node::SetMetadata(const Metadata& _metadata)
{
	metadata = _metadata;
}

Node::NodeList::iterator Node::PushNode(const Node& node)
{
	nodes.push_back(node);
	return --nodes.end();
}

Node::NodeList::iterator Node::PushNode(
	Node::NodeList::iterator pos, const Node& node)
{
	return nodes.insert(pos, node);
}

void MsgList::ListNodes(DOMSerializedMessage& Result,
						DOMElement *DEnode, Node& node,
						xercesc::DOMDocument & document)
{
	/*DOMElement *DEmetadata = */document.createElement((XMLCh[]){chLatin_m, chLatin_e, chLatin_t, chLatin_a, chLatin_d, chLatin_a, chLatin_t, chLatin_a, chNull});
	
	// metatata shall be filled with apropriate data
	DOMElement *DEflags = document.createElement((XMLCh[]){chLatin_f, chLatin_l, chLatin_a, chLatin_g, chLatin_s, chNull});
	if(node.metadata.info.flags.branch) {
		DEflags->appendChild(
			document.createElement((XMLCh[]){chLatin_b, chLatin_r, chLatin_a, chLatin_n, chLatin_c, chLatin_h, chNull}));
	}
	if(node.metadata.info.flags.data) {
		DEflags->appendChild(
			document.createElement((XMLCh[]){chLatin_d, chLatin_a, chLatin_t, chLatin_a, chNull}));
	}
	if(node.metadata.info.flags.link) {
		DEflags->appendChild(
			document.createElement((XMLCh[]){chLatin_l, chLatin_i, chLatin_n, chLatin_k, chNull}));
	}
	if(node.metadata.info.filename) {
		DOMElement *DEfilename = document.createElement((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull});
		DEfilename->appendChild(document.createTextNode(fromNative(node.metadata.filename).c_str()));
		DEnode->appendChild(DEfilename);
	}
	if(node.metadata.info.filesize) {
		DOMElement *DEfilesize = document.createElement((XMLCh[]){chLatin_f, chLatin_i, chLatin_l, chLatin_e, chLatin_s, chLatin_i, chLatin_z, chLatin_e, chNull});
		
		DEfilesize->appendChild(
			document.createTextNode(
				fromNative(toString((uint32_t)node.metadata.filesize)
					).c_str()));
		DEnode->appendChild(DEfilesize);
	}

	DOMElement* DEnodes;
	if(node.nodes.size()) {
		DEnodes = document.createElement((XMLCh[]){chLatin_n, chLatin_o, chLatin_d, chLatin_e, chLatin_s, chNull});
		DEnode->appendChild(DEnodes);
	}
	
	for(std::list<Node>::iterator i = node.nodes.begin();
		i != node.nodes.end();++i) {
		DOMElement* DEnode2 = document.createElement((XMLCh[]){chLatin_n, chLatin_o, chLatin_d, chLatin_e, chNull});
		
		ListNodes(Result, DEnode2, *i, document);
		DEnodes->appendChild(DEnode2);
	}
	
	DEnode->appendChild(DEflags);
	
}

const Node& MsgList::GetNode() const
{
	return node;
}

Node& MsgList::GetNode()
{
	return node;
}

Message::DOMSerializedMessage MsgList::Serialize(xercesc::DOMDocument & document)
{
	DOMSerializedMessage Result (InitializeSerialization(document));
   
	xml::CommonDOMFeatures::SetElementNamespace(*Result,
			xml::CommonDOMFeatures::GetServerNamespaceURI());
	DOMElement *DEnode = 
		document.createElement((XMLCh[]){chLatin_n, chLatin_o, chLatin_d, chLatin_e, chNull});
	Result->appendChild(DEnode);
	ListNodes(Result, DEnode, node,document);

	return Result;
};

void MsgList::DefaultProcessImpl(ClientControl& control)
{
	;
}
