#include "supercontrol.h++"
#include<boost/filesystem/operations.hpp>
#include<iostream>
void  xftpclient::SuperControl::LRemove (Tokenizer &optns)
{
	using namespace boost::filesystem;
	using namespace std;
	Tokenizer::iterator i =optns.begin ();
	if (i!=optns.end())
	{
		path filename(*i);
		if (!exists(filename))
	
			cout <<  "No such file or directory" << endl;
		else
		try
		{
			remove(filename);
		}
		catch (...)
		{
			cout << "Can't remove file." << endl;
		}
	}
	else
		cout << "Name?" << endl;	
}
