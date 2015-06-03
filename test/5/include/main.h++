#ifndef __MAIN_H__
#define __MAIN_H__
#include <pthread.h>
#include <set>
#include "demo.h++"

class Server;

extern int port;
extern Server s;

class ProgEntry
{
private:
    Scr::Demo* d; 
public:
    ProgEntry(Scr::Demo * _d):d(_d){std::cerr << "Adding entry!" << std::endl;}
	Scr::Demo& GetEntry() { return *d; }
    ~ProgEntry()
	{
	    d->Exit(4);
	    std::cerr << "Deleting entry -" <<std::endl;
	}
    friend bool operator<(const ProgEntry& a, const ProgEntry& b);
};

extern std::set<ProgEntry> allprogs;

void err(const char *s);
extern pthread_mutex_t startSequenceMutex;

#endif
