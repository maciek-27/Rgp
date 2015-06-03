#include <algorithm>
#include <boost/lambda/bind.hpp>
#include <cassert>
#include <functional>
#include <iostream>
#include <list>
#include "sum_iterator.h++"
#include "textbox.h++"

using namespace MUD;
using namespace Aux; // for TextBox itself
using namespace boost::lambda;
void TextBox::OnRedraw(Scr::Screen& screen)throw()
{
	size_t y,x;
	y=x=0;
	screen << style << Scr::Bg::Yellow;
	screen.Clear();
	for (std::vector<std::wstring>::iterator it = text.begin();
		 it!=text.end() ; ++it)
	{
		// break line when approaching screen edge
		if (((*it)==L"\n") or it->size()+x+1>=GetWidth())
		{
			y++;
			if (y>=GetHeight())
				return; // don't write below lower screen edge
			x=0;
			
		}
		if (!(*it==L"\n"))
		{
			screen.GotoYX(y,x);
			screen.AddText(*it);
			
			// +1 because of automatically inserted space
			x+=it->size()+1; 
			
		}
		
	}
}

void TextBox::AppendText(const std::wstring & text)
{
	// pair of iterators will be - throughout whole following
	// algorithm - used to denote begining and ending of word (word is <a;b>)
	std::wstring::const_iterator a,b;
	b=text.begin();
	a=b;
	// text is analyzed letter-by-letter
	while (b!=text.end())
	{
		switch ( *a )
		{
			// we don't count spaces. omit them all
		case ' ':
			while (b!=text.end() && *b==L' ') ++b;
			a=b;
			break;
			// '\n' is special "word" - it is literally used to insert whitespaces
		case '\n':
			this->text.push_back(L"\n");
			++b;++a;
			break;
			// normal words consists of all characters excluding '-'
			// and '\n'
		default:
			while (b!=text.end() && *b!=L' ' && *b!=L'\n')
				++b;
			std::wstring s(a,b);
			this->text.push_back(s);
			a=b;
		}
		assert(a==b);
		
	}
	// set minimum width to length of longest word in set.
 	SetMinWidth(
  	  	std::transform(this->text.begin(),this->text.end(), maximum_iterator<size_t>(),
 					   bind (&std::wstring::length,_1)).get_maximum());
}

