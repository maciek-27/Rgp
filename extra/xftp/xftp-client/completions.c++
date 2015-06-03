#include "completions.h++"
#include "client.h++"

#include <readline/readline.h>

namespace xftpclient
{
	char *
	dupstr (const char * s)
	{
		char *r;
		
		r = (char*)malloc (strlen (s) + 1);
		strcpy (r, s);
		return (r);
	}
	
	
	/* Generator function for command completion.  STATE lets us
   know whether to start from scratch; without any state
   (i.e. STATE == 0), then we start at the top of the list. */
	char *
	command_generator (const char *text, int state)
	{
		//static int list_index, len;
		//char *name;
		static int len;
		/* If this is a new word to complete, initialize now.  This
		 * includes saving the length of TEXT for efficiency, and
		 * initializing the index variable to 0. */
		static	CommandIterator i;
		if (!state)
		{
			i= commands.begin ();
			len = strlen (text);
		}
		
		
		for ( ;i!=commands.end () ; ++i)
		{
			if (strncmp (i.get_key (), text, len) == 0)
			{
				char * r = dupstr (i.get_key ());
				++i;
				return (r);
			}
		}
		/* If no names matched, then return NULL. */
		return ((char *)NULL);
	}
	
	
	char **
	completions (const char *text, int start, int end)
	{
		char **matches;
		
		matches = (char **)NULL;
		
		/* If this word is at the start of the line, then it is a command
		 * to complete.  Otherwise it is the name of a file in the current
		 * directory. */
		if (start == 0)
			matches = rl_completion_matches (text, command_generator);
		return (matches);
	}
	
	
}

