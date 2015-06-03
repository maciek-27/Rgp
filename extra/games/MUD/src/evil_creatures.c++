#include <boost/thread/thread.hpp>
#include <cstdlib>
#include <iostream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "character.h++"
#include "dungeon.h++"
#include "evil_creatures.h++"
using namespace MUD;

namespace{
class EvilCreature
{
private:
	struct evil_creature_status
	{
		evil_creature_status():running(true){}
		bool running;
		pCharacter pChr;
	} status;
	
	static void RunEvilCreature(evil_creature_status *status)
		{
			using namespace std;
			cout.put('+');
			cout.flush();

			std::stringstream textual;
			textual << "rat" << ':' << "" << ':' << 0 << ':' << 0 << '\n';
			status->pChr.reset(new Character(textual));
			while (status->running)
			{
				status->pChr->Move(make_pair((rand()%2)*2 -1,(rand()%2)*2 -1));
				Dungeon::Instance().AddMarker(status->pChr);
				
				usleep (rand()%1000000);
				Dungeon::Instance().DelMarker(status->pChr);
			}
			cout.put('-').flush();
		}
	boost::thread ecthread;
public:
	EvilCreature()
		{
		}
	EvilCreature(const EvilCreature & ec1):ecthread(RunEvilCreature,&status)
		{
		}
	EvilCreature & operator= (const EvilCreature & ec1)
		{
			abort();
		}
	~EvilCreature()
		{
			status.running=false;
			ecthread.join();
		}
};
}
std::vector<EvilCreature> ecs;
SupportForEvilCreatures::SupportForEvilCreatures()
{
	int initial_ec_num=5;
	std::cout << "Spawning " << initial_ec_num << " evil creatures (count '+' ) : "
			  << std::flush;
	ecs.resize(initial_ec_num);
	std::cout << "Done" << std::endl;
}

SupportForEvilCreatures::~SupportForEvilCreatures()
{
	std::cout << "Waiting for " << ecs.size() << " evil creatures to halt (count '-') :"
			  << std::flush;
	ecs.clear();
	std::cout << "Done"  << std::endl;

}
