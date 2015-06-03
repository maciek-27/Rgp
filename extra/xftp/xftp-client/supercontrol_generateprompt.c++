#include "boost/filesystem/operations.hpp"
#include "supercontrol.h++"
const std::string xftpclient::SuperControl::GeneratePrompt() const
{
	using namespace std;
	using namespace boost::filesystem;
	string result;
	char prev = 0;
	bool escape = false;
	// command prompt template syntax is explained in interactive help. to
	// quickly access it without need to compile this program, just access
	// supercontrol_run.c++ file to review it (command help is generated
	// in this file)
	for (string::const_iterator i = PromptTemplate.begin();
		 i!=PromptTemplate.end();
		 ++i)
	{
		if (*i == '\\')
		{
			if  (escape)
				escape=false,result.push_back('\\');
			else
				escape=true;
		}
		// magic $ is not escaped - omit it, print following meta-variable
		// instead
		else if (*i=='$' and !escape)
			; // donothing
		// magic $ is "escaped" - print it verbatim
		else if (*i=='$' and escape)
			result.push_back('$');
		else if (prev == '$' and !escape)
		{
			// meta-variables represent  local/remote paths (whole or last leaf)
			// there are only 4 such variables supported
			switch (*i)
			{
			case'r':result.append(CurrentPath.leaf ());
				break;
			case 'R':result.append(CurrentPath.string ());
				break;
			case 'l':result.append(current_path().leaf ());
				break;
			case 'L':result.append(current_path().string ());
				break;
			default:
					result.append("???");
			}
		}
		else
		{
			// print character verbatim. if something was to be escaped, it
			// was - cancel escape.
			result.push_back(*i);
			escape=false;
		}
		prev=*i;
	}
	return result;
}

