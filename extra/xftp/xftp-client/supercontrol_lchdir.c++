#include "supercontrol.h++"
#include <cstdlib>
#include <iostream>
#include <errno.h>
void  xftpclient::SuperControl::LChDir (Tokenizer &	 optns)
{
	using namespace std;
	Tokenizer::iterator i =optns.begin ();
	if (i==optns.end ())
		cout << "path?" << endl;
	// if possible, just change local working directory
	else if (chdir (i->c_str()))
		cout << "path inaccessible (" << strerror(errno) <<')'  << endl;
	
}
