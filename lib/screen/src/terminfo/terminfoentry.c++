#include <stack>
#include <sstream>
#include "terminfo.h++"
#include "terminfokeymap.h++"
#include "screen.h++"
#include "throw.h++"
#include "capabilities.h++"

using namespace Scr::TI;
using namespace std;

/*refer to man term(5) for more details*/
static const unsigned short MagicNumber = 0432;

TerminfoEntry::TerminfoEntry (std::ifstream & ifile)throw()
{
	RexIOLog(LogLevelLow) << "TerminfoEntry::TerminfoEntry" <<endl;
	short magic;
	// verify magic number
	ifile.read (reinterpret_cast<char*>(&magic), 2);
	EASSERTP (magic==MagicNumber, FatalException, "Invalid terminfo file");
	
	// read basic data
	ifile.read (reinterpret_cast<char*>(&Meta), sizeof(Meta));
	
#define annotate(x) RexIOLog(LogLevelModerate) << #x<< "==" << Meta.x << endl;
	annotate(namesSize);
	annotate (numBooleans);
	annotate (numIntegers);
	annotate (numOffsets);
	annotate (stringTableSize);
	
	//booleans section + '\0' + Integers section + '\0' + Offsets
	//section  + String table itself
	
	size_t text_sz
			=Meta.namesSize+Meta.numBooleans+2*Meta.numIntegers
			+2*Meta.numOffsets+2+Meta.stringTableSize
			+((Meta.namesSize+Meta.numBooleans+1)&1);
	
	text = new char[text_sz];
	
	ifile.read (text, text_sz);
	
	char * tmptxt = text;
	Data.names=tmptxt;
	tmptxt+=Meta.namesSize; // '\0'
	Data.booleans=tmptxt;
	tmptxt+=Meta.numBooleans;
	
	// numbers section is 2-byte-aligned
	tmptxt+=((Meta.namesSize+Meta.numBooleans)&1) ;
	
	Data.numbers=
			reinterpret_cast<short*>(tmptxt);
	
	Data.strings = new char*[Meta.numOffsets];
	tmptxt+=2*Meta.numIntegers;
	
	for ( int i = 0 ; i!=Meta.numOffsets-1 ; i++)
	{
		if ((reinterpret_cast<short*>(tmptxt))[i]==-1)
		{
			Data.strings[i] = 0;
			RexIOLog(LogLevelVerbose) << "feature not supported" << endl;
		}
		else
		{
			Data.strings[i]=tmptxt+2*Meta.numOffsets
					+ (reinterpret_cast<short*>(tmptxt))[i];
			RexIOLog(LogLevelVerbose) << i << ". " << Data.strings[i] << endl;
		}
	}
	// Terminfo database retrieved. create keymap.
	
	keymap = new Keymap(*this);
}

Keymap & TerminfoEntry::GetKeymap() const
{
	return *keymap;
}

bool TerminfoEntry::GetBoolean (int i) const throw()
{
	if (i > Meta.numBooleans)
		return false;
	else
		return Data.booleans[i]==1;
}

short TerminfoEntry::GetInteger (int i) const throw()
{
	if (i > Meta.numIntegers)
		return  -1;
	else
		return Data.numbers[i];
}

const char * TerminfoEntry::GetString (int i) const throw()
{
	RexIOLog(LogLevelVerbose) << "GetString " << Data.strings[i] << endl;
	if (i > Meta.numOffsets)
		return 0;
	else
		return Data.strings[i];
}

std::string TerminfoEntry::ParseString (int i, Uint * param)
		const throw(CapabilityNotSupported, ParseError)
{
	boost::mutex::scoped_lock lock (textmod_mtx);
	const char * s=GetString (i);
	EASSERT (s, CapabilityNotSupported); // refuse to process not
	// supported capabilities
	RexIOLog(LogLevelModerate) << "Processing String " << s <<  '/' << param[0]<< endl;
	
	char * ps=const_cast<char*>(s); // safe const cast, as each change is 
									// reverted
	std::string result;
	stack<int> params;
	while (*ps)
	{
		if (*ps!='%')
		{
			result.push_back (*ps);
			ps++;
		}
		else //parse % sequence
		{
			ps++;
			if (*ps=='%')
			{
				result.push_back ('%');
				continue;
			}
			else if (*ps=='i')// add 1 to first two params
			{
				param[0]++;
				param[1]++;
			}
			else if (*ps=='p')// push parameter
			{
				ps++;
				params.push (param[*ps-'1']);
			}
			else if (*ps=='{')// push integer constant (support only positive)
			{
				Uint i = 0;
				while (*ps!='}')
				{
					i*=10;
					i+=*(++ps)-'0';
				}
				params.push (i);
			}
			else //  %[[:]flags][width[.precision]][doxXs]
			{
				//according to manual: "as in printf"... so we'll use
				//printf itself:D
				
				// first extract format string
				char * s = ps -1; // starting from %
				
				while (*ps!='d' && *ps!='o' && *ps!='x' && *ps!='X' && *ps!='s')
				{
					*ps++;
				}
				char t=ps[1];
				
				char tmp[80];
				{// mtx scope
					ps[1]=0; // format string must be null terminated
					
					RexIOLog(LogLevelModerate) << "Processing int " << s << ' ' << params.top () << endl;
					sprintf (tmp, s, params.top ());
					ps[1]=t;// restore format string (RELEASE MUTEX HERE)
				}
				result.append (tmp);
				params.pop ();
			}
			ps++;
		}
	}
	RexIOLog(LogLevelModerate) << "Terminfo processing result: " << result << endl ;
	return result;
	
}

const std::string
TerminfoEntry::GotoYX (const Scr::Position & newPosition) const throw(CapabilityNotSupported)
{
	Position tmp (newPosition);
	return ParseString (CursorAddress, & (tmp.row));
}

const std::string TerminfoEntry::GotoYX (const Scr::Position & newPosition,
		const Scr::Position & oldPosition)
const throw(CapabilityNotSupported)// there is field for optimization w/ relative movements, but don't
            //  implement it yet
{
	return GotoYX (newPosition);
}

const std::string TerminfoEntry::SetDisplayStyle (const Scr::DisplayStyle s) 
const throw(CapabilityNotSupported)
{
// stringstream ss;
//  	ss<< "\x1b["
//  			<< ((s.GetFgStyle ()==Scr::Fg::Bright)?"1;":"0;")// set bright if
//  			// needed
//  			<< s.GetFgColor () << ';' << s.GetBgColor ()
//  			<<'m';
//	string result("\x1b[");
	int i=0;
	char result[10];
	result[i++]=0x1b;
	result[i++]='[';
	result[i++]='0'+(s.GetFgStyle ()==Scr::Fg::Bright);
	if (s.GetFgColor ()>=Scr::Fg::Black)
	{
		result[i++]=';';
		result[i++]='0'+Scr::Fg::Black/10;
		result[i++]='0'+s.GetFgColor ()-Scr::Fg::Black;
	}
	if (s.GetBgColor ()>=Scr::Bg::Black)
	{
		result[i++]=';';
		result[i++]='0'+Scr::Bg::Black/10;
		result[i++]='0'+s.GetBgColor ()-Scr::Bg::Black;
	}
	result[i++]='m';
	return string(result,i);
}

const std::string TerminfoEntry::SetDisplayStyle (const Scr::DisplayStyle s,
												  const Scr::DisplayStyle os) 
const throw(CapabilityNotSupported)
{
	return SetDisplayStyle(s);
	int i=0;
	char result[10];
	result[i++]=0x1b;
	result[i++]='[';
	result[i++]='0'+(s.GetFgStyle ()==Scr::Fg::Bright);
	if (s.GetFgColor ()!=os.GetFgColor()
		and s.GetFgColor ()>=Scr::Fg::Black)
	{
		result[i++]=';';
		result[i++]='0'+Scr::Fg::Black/10;
		result[i++]='0'+s.GetFgColor ()-Scr::Fg::Black;
	}
	if (s.GetBgColor ()!=os.GetBgColor()
		and s.GetBgColor ()>=Scr::Bg::Black)
	{
		result[i++]=';';
		result[i++]='0'+Scr::Bg::Black/10;
		result[i++]='0'+s.GetBgColor ()-Scr::Bg::Black;
	}
	result[i++]='m';
	return string(result,i);
}

const std::string TerminfoEntry::ShowCursor () const throw(CapabilityNotSupported)
{
	const char * s = GetString (CursorVisible);
	if (s not_eq NULL)
		return string (s);
	else
	{
		s = GetString (CursorNormal);
		if (s not_eq NULL)
			return string (s);
		else
			THROW(CapabilityNotSupported);
	}
}

const std::string TerminfoEntry::HideCursor () const throw(CapabilityNotSupported)
{
	const char * s = GetString (CursorInvisible);
	if (s)
	{
		return string (s);
	}
	else
	{
		THROW(CapabilityNotSupported);
	}
}

const std::string TerminfoEntry::CursorHome () const throw(CapabilityNotSupported)
{
	const char * s = GetString (TI::CursorHome);
	if (s)
	{
		return string (s);
	}
	else
	{
		THROW(CapabilityNotSupported);
	}
}
TerminfoEntry::~TerminfoEntry ()throw()
{
	using namespace std;
	RexIOLog(LogLevelLow) << "freing TI entry for" << Data.names << endl;
	delete[] text;
	delete[] Data.strings;
	delete keymap;
}

