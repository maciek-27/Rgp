#ifndef KEYMAP_HPP_DEF
#define KEYMAP_HPP_DEF
#include <map>
#include <boost/function.hpp>

const int KeyboardHelpIndent = 20;
	class KeyMap
	{
	public:
		class ActionNotDefined: public std::exception {};
		typedef boost::function<void ()> Action;
		typedef std::map<std::string,Action > Actions;
		typedef std::map<Scr::Uint,Actions::iterator > KeyAssignments;
	private:
		Actions A1;
		KeyAssignments KA1;
	public:
		void AddAction(std::string name, Action a)
			{
				A1[name]=a;
			}
		void AssignKey(Scr::Uint key, std::string ActionName)
			{
				KA1[key]=A1.find(ActionName);
			}
		void AddActionAssignKey(std::string name, Action a,Scr::Uint key)
			{
				KA1[key]=A1.insert(Actions::value_type(name,a)).first;			
			}
		
		bool Execute(Scr::Uint key);
		std::string  GetActionName(Scr::Uint key)
			{
				KeyAssignments::iterator i = KA1.find(key);
				if ( i not_eq KA1.end() )
				{
					return i->second->first;
				}
				else
				{
					throw ActionNotDefined();
				}
				
			}
	private:
		//Aux routine for PrintKey
		void PrintKey(FILE * f, Scr::Uint key, Scr::Uint margin);
	public:
		void PrintHelp(FILE * f);
	};
	extern KeyMap KM1;

#endif
