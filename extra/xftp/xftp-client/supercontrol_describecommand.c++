#include <algorithm>

#include <ext/functional>
#include "supercontrol.h++"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <deque>
#include <cmath>
#ifndef WIN32
#include <sys/ioctl.h>
#endif
#define _1 boost::lambda::_1
#if defined _GNU_SOURCE
// other platforms supply compose2 as part of std;
using __gnu_cxx::compose2;
#endif
using namespace boost::lambda;
using namespace std;

namespace
{
	typedef struct
	{
		const char* begin;
		
		int strlen; // strlen may not represent actual length of string.
					// It is used only to calculate spacing and indents
					// are implemented by.. increasing strlen of first
					// word in line.
		
		int strsize;// Number of bytes of word.
	} WordData;

	typedef deque<WordData> Line;

	// Prints single line using left-and-right alignment. Omits
	// too-short words in the ending for aesthetic reasons.
	// return value: size of text, that still has to be printed
	int PrintLine(Line & line, int max_length)
	{
		static const int min_last_word_length = 2;
		// don't allow ending line with shorter word
		
		// find last word in specific line. this shall be a first (from ending)
		// word, that is at least min_last_word_length long.
		Line::iterator end =
				find_if(line.rbegin(),line.rend(),bind(&WordData::strlen,_1)>=
					min_last_word_length).base();


		// compute expected space as max_length - sum of strlen's
		int expected_space = max_length;
		for_each(line.begin(),end, expected_space-=bind(&WordData::strlen,_1));
		
		for (Line::iterator i = line.begin() ; i!=end ; ++i)
		{//for each word
			cout.write(i->begin,i->strsize);
			if (expected_space)
			{
				int space = round ( (1.0*expected_space)/(end-i-1) );
				fill_n (ostream_iterator<char>(cout),space, ' ');
				expected_space-=space;
			}
		}
		cout.put ('\n');
		// erase words, that have already been printed from buffer. they shall
		// be printed in next line
		line.erase(line.begin(),end);

		// compute minimum remaining characters number
		int result=0;
		for_each(line.begin(),line.end(), 
				result+=bind(&WordData::strlen,_1));
		result+=line.size();
		return result;
	}
	
	// simplified version of PrintLine  prints left-aligned text with
	// sigle spacing. Prints whole line.
	void PrintLastLine(Line & line)
	{
		if (line.empty())
			return;
		for (Line::iterator i = line.begin() ; i!=line.end() ; ++i)
		{
			cout.write(i->begin,i->strsize);
			cout.put (' ');
		}
		cout.put ('\n');
		line.clear();
	}

	//! std::unary_function implementing test
	class DoWePrintIt: public unary_function<char,bool>
	{
	public:
		//! return true if character is printable and nonwhitespace
		bool operator() (char c) const
		{ return static_cast<unsigned char>(c) > ' ';}
	};
}


void xftpclient::SuperControl::DescribeCommand(CommandIterator & i)
{
	Line line;
	cout << "* ";
	const int left_margin = 10;			// left margin
										// right margin = left_margin + max_offset
	const int right_margin = 10;
	const int indent = 6;				// indent of first line (relative to 
										// left margin) of paragraph
	const int indent_first = -16;		// indent of first paragraph, if differs
										// from next paragraphs

	// extract current window width, if possible
#ifdef    TIOCGWINSZ
	int max_offset;
	struct winsize ws;
	if (ioctl(0, TIOCGWINSZ, (char *)&ws) >= 0)
		max_offset = ws.ws_col-left_margin-right_margin;
	// otherwise just set default value
	else
#else
	const int 
#endif
		max_offset=60;			        // width of text
	int offset;
	{
		int ss = strlen (i.get_key ());
		offset = 2+ss+indent+indent_first;
		// offset - current offset (from left margin)
		line.push_back( (WordData){i.get_key (),offset,ss} );
		offset++;
	}
	for (string::iterator tmp, pc = i->second.begin ();
		 pc!=i->second.end ();
		 pc=find_if (tmp, i->second.end (),
					 compose2(logical_or<bool>(),DoWePrintIt(),_1=='\n')) )
	{
		int strlen=0;
		if (*pc=='\n')
		{// starting new paragraph
			PrintLastLine(line);
			offset=0;
			strlen=indent;
			pc=find_if (pc, i->second.end (), DoWePrintIt());
			// find anything printable
			if (pc!=i->second.end ()) // curren't paragraph isn't last one
				fill_n (ostream_iterator<char>(cout), left_margin+indent, ' ');
			else
				return;
		}
		
		tmp=find_if (pc, i->second.end (), not1( DoWePrintIt() ) );
		int strsize = tmp-pc; // string size in bytes
		strlen += strsize-count_if (pc,tmp, (_1 & 0xC0) == 0x80);;
			// length of current word (as utf8)
	
		offset+=strlen;
		
			// if current word doesn't match right margin, break line
		if (offset>max_offset)
		{// auto breaking line
			offset=PrintLine(line,max_offset)+strlen;
			fill_n (ostream_iterator<char>(cout), left_margin, ' ');
		}
		offset++;// at least one space
		line.push_back( (WordData){&*pc,strlen,strsize} );
	}
	PrintLastLine(line);
}
