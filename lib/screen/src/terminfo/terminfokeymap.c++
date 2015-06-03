#include "terminfokeymap.h++"
#include "capabilities.h++"

using namespace Scr::TI;

Keymap::Keymap (const TerminfoEntry & te)throw()
{
	InitializeKeymap (te);
}

//const char * s
#define map_key_and_capability(cap,key)									\
{																\
	s = te.GetString(cap);										\
	if (s != NULL and s[0])/*string exists and is nonempty*/	\
	{															\
		keyboard.insert(s,static_cast<Scr::Key>(key));			\
	}															\
}
#define map_key(k) map_key_and_capability(Scr::TI::Key ## k,Scr::Key::k)
void Keymap::InitializeKeymap (const TerminfoEntry & te)throw()
{
	const char * s;
//	map_capability(TI::KeyF1,Key::F1);
	
	keyboard.insert("\x1b[H",Scr::Key::Home);// default code for Home 
	//(if terminfo specifies another it's meaning, it will be overriden)
	keyboard.insert("\x1b[F",Scr::Key::End	);
 	keyboard.insert("\x1b[A",Scr::Key::Up	);
 	keyboard.insert("\x1b[B",Scr::Key::Down	);
 	keyboard.insert("\x1b[C",Scr::Key::Right);
 	keyboard.insert("\x1b[D",Scr::Key::Left	);
	
 	keyboard.insert("\x1b[1;5A",Scr::Key::CtrlUp	);
 	keyboard.insert("\x1b[1;5B",Scr::Key::CtrlDown	);
 	keyboard.insert("\x1b[1;5C",Scr::Key::CtrlRight);
 	keyboard.insert("\x1b[1;5D",Scr::Key::CtrlLeft	);
	map_key (F1);
	map_key (F2);
	map_key (F3);
	map_key (F4);
	map_key (F5);
	map_key (F6);
	map_key (F7);
	map_key (F8);
	map_key (F9);
	map_key (F10);
	map_key (F11);
	map_key (F12);
	map_key (F13);
	map_key (F14);
	map_key (F15);
	map_key (F16);
	map_key (F17);
	map_key (F18);
	map_key (F19);
	map_key (F20);
	map_key (F21);
	map_key (F22);
	map_key (F23);
	map_key (F24);
	map_key (F25);
	map_key (F26);
	map_key (F27);
	map_key (F28);
	map_key (F29);
	map_key (F30);
	map_key (F31);
	map_key (F32);
	map_key (F33);
	map_key (F34);
	map_key (F35);
	map_key (F36);
	
	map_key_and_capability(Scr::TI::KeyBtab,Scr::Key::BackTab);
	map_key_and_capability(Scr::TI::KeyDc,Scr::Key::Delete);
	map_key_and_capability(Scr::TI::KeyPpage,Scr::Key::PageUp);
	map_key_and_capability(Scr::TI::KeyNpage,Scr::Key::PageDown);
	map_key_and_capability(Scr::TI::Tab,Scr::Key::Tab);
	map_key (Home);
	map_key (End);
	map_key (Enter);
	map_key (Left);
	map_key (Right);
	map_key (Up);
	map_key (Down);
	
}

Keymap::validity
		Keymap::TestCode (const char * code)throw()
{
	Dictionary<Scr::Key>::iterator it;
	it = keyboard.partial_find(code);
	return it.validity_test();
}

Scr::Key Keymap::GetCode (const char * code)throw(Connection::UnsupportedKey)
{
	Dictionary<Scr::Key>::iterator it;
	it = keyboard.find(code);
	if (it.valid())
		return *it;
	else
		THROW(Connection::UnsupportedKey);
}
