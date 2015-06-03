#include "supercontrol.h++"
#include <cstdlib>
#include <iostream>
#include <boost/functional.hpp>
void xftpclient::SuperControl::PrintNode (xftp::server::Node  & node)
{
	using namespace std;
	recurse++;
	xftp::server::Metadata M = node.GetMetadata ();
	
	cout << setw (recurse+1) <<"*" << setw (30-recurse)
	<< M.filename << "   " << M.filesize << endl;
	
	
	for_each (node.GetNodes ().begin (), node.GetNodes ().end (),
			boost::bind1st (std::mem_fun (&SuperControl::PrintNode), this));
	recurse--;
	
}
