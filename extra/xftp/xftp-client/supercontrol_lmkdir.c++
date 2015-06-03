#include "supercontrol.h++"
#include<boost/filesystem/operations.hpp>
#include<iostream>
void xftpclient::SuperControl::LMkDir (Tokenizer & optns)
{
	using namespace boost::filesystem;
	using namespace std;
	Tokenizer::iterator i =optns.begin ();
	if (i!=optns.end())
	{
		path filename(*i);
		if (exists(filename))
	
			cout << filename << " already exists." << endl;
		else
		try
		{
			create_directory(filename);
		}
		catch (...)
		{
			cout << "Can't create directory." << endl;
		}
	}
	else
		cout << "Name?" << endl;
}
