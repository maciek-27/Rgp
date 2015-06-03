#include "throw.h++"
#include "keyboard.h++"
#include "commons.h++"
using namespace Scr;

#define KEYD(x) case x: return # x 
const char * Key::GetKeyName()throw()
{
	switch (key)
	{
		KEYD(Enter);
		KEYD(Tab);
		KEYD(Escape);
		KEYD(Delete);
		KEYD(Backspace);
			
		KEYD(F1);
		KEYD(F2);
		KEYD(F3);
		KEYD(F4);
			
		KEYD(F5);
		KEYD(F6);
		KEYD(F7);
		KEYD(F8);

		KEYD(F9);
		KEYD(F10);
		KEYD(F11);
		KEYD(F12);
		
	case Left: return "\342\206\220";
	case Up: return "\342\206\221";
	case Right: return "\342\206\222";
	case Down: return "\342\206\223";
		///	KEYD(Up);
		//KEYD(Down);
		//KEYD(Right);
		//KEYD(Left);

		KEYD(CtrlUp);
		KEYD(CtrlDown);
		KEYD(CtrlRight);
		KEYD(CtrlLeft);

		KEYD(Insert);
		KEYD(Home);
		KEYD(PageUp);
		KEYD(PageDown);
		KEYD(End);

	default:
		return "unknown";
	}
}

char Key::GetBasicKey()throw(NotABasicKey)
{
	EASSERT(IsABasicKey(),NotABasicKey);
	char masked = key& basicKeyMask;
	return masked;
}

Key::Special Key::GetSpecialKey()throw(NotASpecialKey)
{
	EASSERT(IsASpecialKey(),NotASpecialKey);
	return static_cast<Key::Special>(key);
}
