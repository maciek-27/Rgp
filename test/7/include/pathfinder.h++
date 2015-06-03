#ifndef PATH_FINDER_HPP_DEF__
#define PATH_FINDER_HPP_DEF__
#include <boost/function.hpp>
#include "rexio/commons.h++"
#include <list>
#include <boost/scoped_array.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>

// Path Finding algorithm and more...
class MapData;

//! Class impelemts path finding algorithm for whole game
//! simple interface (only 7 functions including ctor and implicit
//! dtor) covers fairly complex structure.
class PathFinder: boost::noncopyable
{
public:

	// Path::begin() returns starting field of to-be-computed path
	// Path::end()-- returns goal (or at least as close as possible to it)
	typedef std::list<Scr::Position> PositionList ;
	typedef PositionList Path ;

	//! MUST RETURN TRUE IF CHARACTER CAN PASS FIELD
	typedef boost::function <bool (Scr::Position)> ObstacleDetector;
	//! returns movement cost for specific field
	typedef boost::function <int (Scr::Position)> CostCalculator;
protected:
	//! list of directions: defines only  constructor
	class Directions: public PositionList
	{
		/*
		/   128  1  2
		/      \ | /
		/  64--  x  --4
		/      / | \
		/    32  16 8  */
		static const unsigned char even_col_mask;
		static const unsigned char odd_col_mask ;

		static const
		Scr::Vector AvailDirections[] ;
	public:
//		PositionList LegalDirections;
		//! ctor fills list with all directions, that
		//! are reachable from p in map pMD
		//! \param p position
		//! \param pMD mapdata
		Directions (MapData * pMD, Scr::Position p);
	};
	//! for each element of map create record of data used to
	//! determine, if this location is good waypoint. 
	class PFDataEntry
	{
	public:
		//! legal directions from here
		Directions D1;// directions legal from here

		//! L stands for length-of-path from starting point to here.
		typedef boost::array<unsigned,3> L;
		//! quality rating of this location (used when processing
		//! final result): when determinating best path locations are
		//! compared lexicographically by L1.
		L L1;//[0]length of path as number of tiles
		//[1]length of path as number of passable obstacles

		//!constructor
		//! \param  pMD - map data (used in constructor only, to
		//! compute D1 - not stored)
		//! \param  p - coordinates, this Entry refers to (used 
		//! in constructor only, to compute D1 - not stored)
		//! \param L1 initializer as array
		PFDataEntry(MapData * pMD, Scr::Position p, int * l);
		//! as above, but initializer in form of L
		PFDataEntry(MapData * pMD, Scr::Position p, L l);
	};
	typedef std::auto_ptr<PFDataEntry> pPFDataEntry;
	
	// STL containers of auto_ptr's are bad idea, as they are ineligable
	// for any operations that require copying (including quick sort
	// algorithm ), but we don't even need resizing... so let's use dumb
	// dynamic array (in boost packaging to ensure correct free)

	//! Array of per-field metadata stored as auto_ptr's (so we
	//! allocate only 1 pointer per unused field.
	typedef
	boost::scoped_array<boost::scoped_array<pPFDataEntry> > PFData;
	// (as overhead of reference counted shared_ptr is not what we want)

	//! instance of PFData;
	PFData PFD1; // path finder data (used during path computation)
	//! "open" list: positions, from which we will start processing
	//! next time
	PositionList StartPoints;
	//! final result (when is ready)
	Path P1;
	//! mapdata, we refer to
	MapData * pMD1;
	//! starting position
	Scr::Position start;
	//! goal
	Scr::Position finish;
	//! obstacle detecting function (returns true if we can pass this
	//! location) 
	ObstacleDetector OD1;
	//! function computing cost of movement to specific location
	CostCalculator CC1;
	//! length maximum (don't search longer paths)
	//! \note: this variable is modified by various functions, such as
	//! ComputePath or MarkPathRecursive, and there is no way of
	//! explicitly specifiing it.
	unsigned lm;
	//! Real "worker" function: fills PFD1 array until lm is matched
	//! of finish is found
	//! \param l length to this field
	void MarkPathRecursive(Scr::Position p, PFDataEntry::L l);

	//! strict weak ordering of points based on lex. comp. of
	//! PFDataEntries (for STL)
	//! \param p positions to compare
	bool BetterCoords(Scr::Position ,Scr::Position );
	//! return true, if found target
	void ComputePath();
#ifndef NDEBUG
	//! just calculated
	//! \note this feature is enabled only for debug builds: specific
	//! map  view can be triggered to show PF stats.
	PositionList Closed; 
#endif
public:
	//! ONLY constructor (no cpy ctors or assignment ops!)
	//! \param pMD map data, we refer to
	//! \param s start
	//! \param f finish
	//! \param OD Obstacle detector function (see ObstacleDetector
	//! typedef spec for requirements, this function has tho meet)
	//! \param CC Cost calculator  function (see ObstacleDetector
	//! typedef spec for requirements, this function has tho meet)
	PathFinder( MapData * pMD,
				Scr::Position s,
				Scr::Position f,
				ObstacleDetector OD,
				CostCalculator CC);
	
	//! Recalculate path w/o changing positions (useful to inform
	//! PathFinder, that the tactical situation has changed:
	//! i.e. obstacle has been destroyed, and passage is easier)
	void Reset();
	
	//! recalculate for diffrent start and finish. Reset all data only
	//! if s differs from old start
	//! \param s start
	//! \param f finish
	void ConditionalReset(	Scr::Position s,
							Scr::Position f);

	//! Change start (this requires resetting all)
	//! \param s start
	void ChangeStart(Scr::Position s);
	//! Change finish w/o resetting data already calculated 
	//! \param f finish
	void ChangeFinish(Scr::Position f);
	//! Fanfare!!! After all this complex operation we can just get
	//! our list of points!!
	Path & GetPath() {return P1;} 
#ifndef NDEBUG
	//! Get list of fields visited during pf algorithm
	//! \note this function is enabled only for debug builds: specific
	//! map  view can be triggered to show PF stats.
	PositionList & GetClosed() {return Closed;}
	//! Get list of points, that were planned to visit, but finally
	//! ignored due to lm (that is path length limit).
	//! \note this function is enabled only for debug builds: specific
	//! map  view can be triggered to show PF stats.
	PositionList & GetOpen() {return StartPoints;}
#endif
}; /// PathFinder

//! Generate shared_ptr to new PathFinder objec
//! \param pMD map data, we refer to
//! \param s start
//! \param f finish
//! \param OD Obstacle detector function (see ObstacleDetector
//! typedef spec for requirements, this function has tho meet)
//! \param CC Cost calculator  function (see ObstacleDetector
//! typedef spec for requirements, this function has tho meet)
//! \note: opposite to default constructor this is templatized
//! function, that allows ANY compatible entity to be converted to
//! ObstacleDetector and CostCalculator.
template <typename __ObstacleDetector, typename __CostCalculator>
std::tr1::shared_ptr< PathFinder >
FindPath( MapData * pMD,
		  Scr::Position s,
		  Scr::Position f,
		  __ObstacleDetector OD,
		  __CostCalculator CC
	)
{
	return 
		std::tr1::shared_ptr< PathFinder >
		(
			new PathFinder
			(pMD,s,f,PathFinder::ObstacleDetector(OD),
				PathFinder::CostCalculator(CC))
			);
} /// FindPath



#endif
