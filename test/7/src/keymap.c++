#include <rexio/keyboard.h++>
#include<iostream>
#include "keymap.h++"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <cstdio>
using namespace std;
using namespace Scr;
using namespace boost::lambda;


	KeyMap KM1;

void KeyMap::PrintKey(FILE * f, Uint key, Uint margin)
{
	Key k(key);
	if (k.IsASpecialKey())
		fprintf(f,"%*c\x1b[1m\342\216\251%s\342\216\253\x1b[0m",
				static_cast<int>(margin),' ',k.GetKeyName());
	else
		fprintf(f,"%*c\x1b[1m\342\216\251%c\342\216\253\x1b[0m",
				static_cast<int>(margin),' ',int(k));					
}

void KeyMap::PrintHelp(FILE * f)
{
	for (Actions::iterator i = A1.begin() ; i!=A1.end() ; ++i)
	{
		fprintf(f,"%*s -",KeyboardHelpIndent,i->first.c_str());

		KeyAssignments::iterator j =
			find_if(KA1.begin(),KA1.end(),
					bind(&KeyAssignments::value_type::second,_1)==i);

		if (j!= KA1.end())
		{
			PrintKey(f,j->first,0);
			
			for (++j; j!= KA1.end(); ++j)
				if (j->second==i)
				{
					putc(',',f);
					PrintKey(f,j->first,1);
				}
			putc('\n',f);
		}
		else
			fprintf(f," \x1b[1mUnassigned\x1b[0m\n");
			
	}
}

bool KeyMap::Execute(Scr::Uint key)
{
	if (key <='Z' and key >='A')
		key+='a'-'A';
	KeyAssignments::iterator i = KA1.find(key);
	if ( i not_eq KA1.end() )
	{
		i->second->second();
		return true;
				}
	else
		return false;
}
