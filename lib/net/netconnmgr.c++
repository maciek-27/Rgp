#incl
pthread_mutex_t allprogsStackMutex;

class ProgEntry
{
private:
    Demo* d; 
public:
    ProgEntry(Demo * _d):d(_d){;}
    ~ProgEntry()
	{
	    d->Exit(4);
	    cout << "-";
	}
    friend bool operator<(const ProgEntry& a, const ProgEntry& b);
    
};

// for set<ProgEntry> underlaying tree. 
bool operator<(const ProgEntry& a, const ProgEntry& b) {
    // comparing addresses of a and b would be wrong, as they may
    // differ while referencing the same Demo class object.
    return a.d < b.d;
}

set<ProgEntry> allprogs;
Server s;

void starter(std::istream & in, std::ostream & out)
{
    Demo prog(in,out);

	pthread_mutex_lock(&allprogsStackMutex);// prevent accidental stack
    allprogs.insert(ProgEntry(&prog));// data structure destruction
    pthread_mutex_unlock(&allprogsStackMutex);
    cout << "Trying to initialize connection" << endl;
    try
    {
	int i = prog.Start(); // start
	cout << "Connection finished with code " << i <<endl;// result
							     // on success
    }
    catch (exception)// exception caught. try to recover by ignoring it.
    {
	cout << "Connection finished with exception, but app is fine." <<endl;
    }
    pthread_mutex_lock(&allprogsStackMutex);
    cout << "Requesting erase of 1 app out of "<<allprogs.size()<<endl;;

    // if ProgEntry exists (not deleted by localInterface func)
    if (allprogs.find(ProgEntry(&prog)) != allprogs.end())
	allprogs.erase(ProgEntry(&prog));//erase it.
    cout << endl;
    pthread_mutex_unlock(&allprogsStackMutex);
    return;
}
