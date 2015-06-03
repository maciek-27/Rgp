#include "character.h++"
#include "dungeon.h++"
#include <map>
#include <sstream>
#include <fstream>
using namespace MUD;


Character::Character(std::istream & textual)
{
	char BUF[512];
	char BUF2[512];
	textual.getline(BUF,sizeof(BUF),'\n');
	std::stringstream ss1(BUF);
	ss1.getline(BUF2,sizeof(BUF2),':');
	name.assign(BUF2);
	ss1.getline(BUF2,sizeof(BUF2),':');
	password.assign(BUF2);
	ss1>>coords.first;
	ss1>>BUF[0];
	ss1>>coords.second;
}

bool Character::CheckPassword(const std::string & _password)
{
	return _password==password;
}
void Character::Serialize(std::ostream & textual)
{
	textual << name << ':' << password << ':' << coords.first << ':' << coords.second << '\n';
}
void Character::Move(std::pair<int,int> direction) {
	std::pair<int,int> tmp(coords);
	tmp.first+=direction.first;
	tmp.second+=direction.second;

 	if ( Dungeon::Instance().IsObstacle(tmp,0))
	{
		if (Dungeon::Instance().IsObstacle(coords,0))
		{
			coords=tmp;
			Move(direction);
		}
		else
			return;
	}
 	else
		coords=tmp;
}


namespace {
	typedef std::map<std::string,pCharacter> acmap;
	acmap allCharacters;
	void ReadAllCharactersFromDisk()
	{
		using namespace std;
		cout << "Reading all characters from disk... " << endl;
		//fstream filestr ("test.txt", fstream::in | fstream::out);
		ifstream filestr ("characters");
		while (filestr)
		{
			pCharacter pch(new Character(filestr));
			if (pch->GetName()=="")
				continue;
			allCharacters[pch->GetName()]=pch;
			cout << '[' <<pch->GetName() << ']' << ' ' << flush;
		}
		
		cout << "done" << endl;
	}
	void SaveAllCharactersToDisk()
	{
		using namespace std;
		cout << "Saving all characters to disk... " << flush;
		ofstream filestr ("characters");
		for ( acmap::iterator it = allCharacters.begin() ;
			  it!=allCharacters.end();
			  ++it)
		{
			it->second->Serialize(filestr);
		}
		cout << "done" << endl;
	}
		
}
StoredCharacters::StoredCharacters(){ReadAllCharactersFromDisk();}
StoredCharacters::~StoredCharacters(){SaveAllCharactersToDisk();}


pCharacter MUD::SelectCharacter(const std::string & _name)
{
	acmap::iterator it = allCharacters.find(_name);
	if (it==allCharacters.end())
		return pCharacter( (Character*)0 );
	else return it->second;
}
void MUD::SetUpNewCharacter(const std::string & _name, const std::string & _password)
{
	std::stringstream textual;
	textual << _name << ':' << _password << ':' << 0 << ':' << 0 << '\n';
	pCharacter pch(new Character(textual));
	allCharacters[pch->GetName()]=pch;
}
