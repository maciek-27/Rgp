#include "terminfodatabase.h++"
#include "terminfo.h++"
#include "throw.h++"
#include "commons.h++"

using namespace Scr::TI;

 boost::mutex GlobalTIMTX;

/*!
  initialized after first call to GetTermInfo
 */
static TerminfoCore * GlobalInstance = 0;

static TerminfoDatabase * db = 0;

static Scr::Uint  NumberOfEntries = 0;

TerminfoCore::TerminfoCore() throw()
	:entries()//global entries resource.
{
	db = new TerminfoDatabase();
}

static int ctr = 0;
TerminfoCore::~TerminfoCore() throw()//global entries resource.
{
	ctr++;
	using namespace std;
	RexIOLog(LogLevelModerate) << "Deleting Terminfo database" << endl;
	delete db;
	db = 0;
	GlobalInstance = 0;
	for (Dictionary<TerminfoEntry*>::iterator it =entries.begin();
		 it!=entries.end(); ++it)
		delete *it;
}

const TerminfoEntry & TerminfoCore::__GetTerminfo(const char * name)
	throw(NotSupportedTerminalType)
{
	boost::mutex::scoped_lock lock (GlobalTIMTX);
	
	Dictionary<TerminfoEntry*>::iterator it =
			entries.find (name);

	

	if (it == entries.end())// if such TerminfoEntry isn't yet retrieved
	{	                   //  create new one
		boost::shared_ptr<std::ifstream> fp;
		try
		{
			fp= (db->OpenFile(name));
		}
		catch (NotSupportedTerminalType)
		{
			throw;
		}
		catch (DatabaseException)
		{
			THROWP(FatalException,"Database fault");
			// something bad must have happened
		}
		catch(...)
		{
			THROW(FatalException);
		}
		TerminfoEntry * e = new TerminfoEntry(*(fp));
		entries.insert(name,e);
		NumberOfEntries++;
		return * e;
	}
	else
	{// return existing one otherwise.
		NumberOfEntries++;
		return ** it;
	}
	
}

void TerminfoCore::Initialize()throw(FailedToOpenDatabase)
{
	
	boost::mutex::scoped_lock lock (GlobalTIMTX);
	if (!GlobalInstance)
		GlobalInstance= new TerminfoCore();
	
	EASSERT(db->GetDatabaseStatus(),FailedToOpenDatabase);
}

bool TerminfoCore::GetDatabaseStatus()throw(DatabaseNotOpen)
{
	if (db)
		return db->GetDatabaseStatus();
	else
		THROW(DatabaseNotOpen);
}

const TerminfoEntry & TerminfoCore::GetTerminfo(const char * name)
	throw(NotSupportedTerminalType,FailedToOpenDatabase)
{
	Initialize();
	return GlobalInstance->__GetTerminfo(name);
}

void TerminfoCore::CleanUp()throw()
{
	delete GlobalInstance;
	GlobalInstance=0;
}

void TerminfoCore::FreeTerminfoEntry()throw()
{
	using namespace std;
	boost::mutex::scoped_lock lock (GlobalTIMTX);
	NumberOfEntries--;
	RexIOLog(LogLevelModerate) << "FreeTerminfoEntry " << NumberOfEntries << endl;
	if (NumberOfEntries == 0)
	{
		if (GlobalInstance)
			GlobalInstance->CleanUp();
		else
			THROW(LogicError);
	}
}
