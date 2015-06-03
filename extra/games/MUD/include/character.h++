/* 
 * File:   character.h++
 *
 * Created on Apr 13, 2009
 */
#ifndef _CHARACTER_H__
#define _CHARACTER_H__
#include <string>
#include <utility>
#include <tr1/memory>
#include <iostream>
#include <vector>
namespace MUD
{
/**
 * Class representing ingame character (of active player), that is
 * character controlled by player, present on game board.
 */
	// Serialization format is Colon Separated Values.
	// name:password:coordinate_y:coordiate_x
class Character
{
	std::string password;
	std::string name;
	std::pair<int,int> coords;
public:
	Character(std::istream & textual);
	// returns true if password for that particular character is right
	bool CheckPassword(const std::string & _password);
	void Serialize(std::ostream & textual);
	const std::string & GetName() { return name; }
	void Move(std::pair<int,int> direction);
	std::pair<int,int> GetPosition() { return coords;}
};
typedef std::tr1::shared_ptr<Character> pCharacter;

pCharacter SelectCharacter(const std::string & _name);
void SetUpNewCharacter(const std::string & _name, const std::string & _password);

// class for storing character on disk.
class StoredCharacters
{
	friend class GlobalGameStatus;
	StoredCharacters();
	~StoredCharacters();
};
}

#endif
