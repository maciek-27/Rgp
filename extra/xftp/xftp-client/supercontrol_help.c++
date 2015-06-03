
#include "supercontrol.h++"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

void  xftpclient::SuperControl::Help (Tokenizer & optns)
{
	using namespace std;
	using namespace boost::lambda;
	Tokenizer::iterator cmd =
	find_if (optns.begin (), optns.end (), !bind (&string::empty,
												  boost::lambda::_1));
	if (cmd==optns.end ())
	{
		int tmp = 0;
		cout << left << "List of XFTP commands\n";
		for (CommandIterator i = commands.begin ();
			i!=commands.end () ; ++i)
			cout << setw(11) << i.get_key () << 
					( (tmp!=5)? (tmp++,"") :(tmp=0,"\n") );
		if (tmp) cout << endl;
		cout << "Type \"help command\" to describe specific command. \n"
			 << "Type \"help all\" to describe all commands. " << endl;
				
	}
	else if (*cmd=="all")
	{
		for (CommandIterator i = commands.begin ();
			i!=commands.end () ; ++i)
			DescribeCommand(i);
	}
	else
	{
		CommandIterator i = commands.find(cmd->c_str());
		if (i!=commands.end())
			DescribeCommand(i);
		else
			cout << "No such command. " << endl;
			
	}
	cout << flush;
}
