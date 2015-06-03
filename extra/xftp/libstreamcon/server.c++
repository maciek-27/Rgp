#include "server.h++"

using namespace streamcon;

Server::Server() throw()
{
	;
}

Server::~Server() throw()
{
	std::cout << "Freeing connections" << std::endl;
	for(std::set<Connection*>::iterator tmp = connections.begin();
		tmp != connections.end();tmp++) {
		delete *tmp;
	}
}

void Server::AddConnection(Connection& connection)
	throw(ConnectionAlreadyAdded)
{
	if(! connections.insert(&connection).second) {
		// element has not been added, conclude the following ...
		THROW(ConnectionAlreadyAdded);
	}
}

void Server::DelConnection(Connection& connection)
	throw(NoSuchConnection)
{
	if(connections.find(&connection) == connections.end())
		THROW(NoSuchConnection);
	connections.erase(&connection);
	delete &connection;
}
