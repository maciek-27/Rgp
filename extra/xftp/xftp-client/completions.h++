#ifndef ___XFTPCLIENT__REAL_APP__COMPLETIONS_
#define ___XFTPCLIENT__REAL_APP__COMPLETIONS_

#include "client.h++"
namespace xftpclient
{
	/*!
	 * Function used by GNU Readline
	 */
	char **
	completions (const char *text, int start, int end);
}
#endif
