
#include "mapdata.h++"
#include "pathfinder.h++"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <algorithm>
#include <ext/algorithm>
#include <functional>
#include <ext/functional>
using namespace std;
using namespace boost::lambda;
#if defined _GNU_SOURCE
using __gnu_cxx::compose1;
#endif



const unsigned char
PathFinder::Directions::even_col_mask = 0xd7 ;// 10111110b
const unsigned char
PathFinder::Directions::odd_col_mask  = 0x7d ;// 11101011b

const
Scr::Vector
PathFinder::Directions::AvailDirections[] =
{
	Scr::Vector(-1,0),
	Scr::Vector(-1,1),
	Scr::Vector(0,1),
	Scr::Vector(1,1),
	Scr::Vector(1,0),
	Scr::Vector(1,-1),
	Scr::Vector(0,-1),
	Scr::Vector(-1,-1)
};

PathFinder::Directions::Directions(MapData * pMD, Scr::Position p)
{

	for (
		unsigned char legal_directions =
			( p.col&1 ) ? odd_col_mask : even_col_mask, i=0;
		legal_directions;
		legal_directions>>=1,i++)
		if (legal_directions&1)
		{
			Scr::Position np(p+AvailDirections[i]);
			if ( np.row < pMD->GetHeight()
			and  np.col < pMD->GetWidth() )
				push_back(np);
		}		// we don't have to check lower bound, as row and col
				// are unsigned, and become LARGE when are expected to
				// be negative.
}


PathFinder::PFDataEntry::PFDataEntry(MapData * pMD, Scr::Position p, int * l):
	D1(pMD,p)
{
	copy(l,l+L::static_size,L1.elems);
}
PathFinder::PFDataEntry::PFDataEntry(MapData * pMD, Scr::Position p, L l):
	D1(pMD,p),L1(l)
{
}
bool PathFinder::BetterCoords(Scr::Position p1,Scr::Position p2)
{
	// if field isn't described, it certainly isn't part of ANY
	// correct path
	if (!PFD1[p1.row][p1.col].get())
		return false;
	if  (!PFD1[p2.row][p2.col].get())
		return true;

	// Unoccupied fields are better, as they allow us save AP
	return 
		lexicographical_compare(PFD1[p1.row][p1.col]->L1.begin(),
								PFD1[p1.row][p1.col]->L1.end(),
								PFD1[p2.row][p2.col]->L1.begin(),
								PFD1[p2.row][p2.col]->L1.end());
}

void PathFinder::MarkPathRecursive(Scr::Position p, PFDataEntry::L l)
{
	if (not PFD1[p.row][p.col].get())
	{
		PFD1[p.row][p.col].reset(new PFDataEntry(pMD1,p,l));
		Path & LD1 = PFD1[p.row][p.col]->D1;
		LD1.erase(
			remove_if(LD1.begin(),
					  LD1.end(),
					  compose1(logical_not<bool>(),OD1)),
			LD1.end());
	}
	else if	(lexicographical_compare(l.begin(), l.end(),
									 PFD1[p.row][p.col]->L1.begin(),
									 PFD1[p.row][p.col]->L1.end()))
		PFD1[p.row][p.col]->L1=l;
	
	else
		return ;
	
		
	PFDataEntry & PFDE1 = *PFD1[p.row][p.col];
	
	if (l[0]+Distance(p,finish) >lm)
	{
		StartPoints.push_back(p);
		return ;
	}
	PFDataEntry::L l2(l);
	if ( p == finish )
	{
		lm=l[0];
		StartPoints.push_back(p);
		return ;
	}
	for (Path::iterator i = 
			 PFDE1.D1.begin();
		 i!=
			 PFDE1.D1.end();
		 ++i
		)
	{
		if (pMD1->GetEntity(i->row,i->col).get()) 
			++(l2[1]);		
		l2[0]=l[0]+CC1(*i);
		l2[2]=abs(static_cast<long>(i->row-start.row));
		MarkPathRecursive(*i,l2 );
	}
}

// the most trivial algorithm.
// it takes one second for 1000 tiles paths on 1000x2000 boards
// (Pentium IV)
void PathFinder::ComputePath()
{
	P1.clear();
	if (start==finish)
	{
		P1.push_front(start);
		return;
	}
	lm = Distance(start,finish)+CC1(start)+CC1(finish)-1;
	
	unsigned lm_t = Distance(start,finish)*10;
	PFDataEntry::L L0={{0,0,0}};
	if (not PFD1[start.row][start.col].get())
		MarkPathRecursive(start,L0);

	
	while( (not PFD1[finish.row][finish.col].get())
		   and (not StartPoints.empty())
		   and (lm <= lm_t)
		)
	
	{
			Path tmp;
			tmp.swap(StartPoints);
			for (Path::iterator i = tmp.begin(),
					 j=tmp.end();
				 i != j; i++ )
			{
				PFDataEntry::L
					l(PFD1[i->row][i->col]->L1);
				;;
				(PFD1[i->row][i->col]->L1[0])++;
				MarkPathRecursive(*i,l);
			}
			lm++;
			//StartPoints.clear();
	}
	if (PFD1[finish.row][finish.col].get() == NULL)
		return;

	P1.push_front(finish);
	if (start==finish)
		return;
	//return;
	while (* P1.begin() != start)
	{//P1.begin()->D1
		Path::iterator min;
		//min = min_element()
		Path & LD1(PFD1[P1.begin()->row][P1.begin()->col]->
				   D1);
		if (LD1.empty())
			return; // no way!
 		P1.push_front(
 			*min_element(LD1.begin(),LD1.end(),
 				bind(&PathFinder::BetterCoords,this,_1,_2)));
			
	}

	//Temporary tracing proc.

// 	cout << "\x1b[H\n\n";
// 	for (int i = 0 ; i!= pMD1->GetHeight() ; i++)
// 		cout << "\n\n";
// //	return;
// 	system("clear");
// 	cout << lm << "  \n";;
// 	for (int i = 0 ; i!= pMD1->GetHeight() ; i++)
// 	{
// 		for (int j = 0 ; j< pMD1->GetWidth() ; j+=2)
// 		{
// 			if (PFD1[i][j].get())
// 				cout << setw(3)  << PFD1[i][j]->L1[1] <<
// 					(pMD1->GetEntity(i,j).get()?'X':
// 					',')
// 					 << PFD1[i][j]->L1[2];
// 			else
// 				cout << "   --";
// 			cout << "      ";
// 		}
// 		cout << "\n";
		
// 		for (int j = 1 ; j< pMD1->GetWidth() ; j+=2)
// 		{
// 			cout << "      ";
// 			if (PFD1[i][j].get())
// 				cout << setw(3)  << PFD1[i][j]->L1[1] <<
// 					(pMD1->GetEntity(i,j).get()?'X':
// 					',')
// 					 << PFD1[i][j]->L1[2];
// 			else
// 				cout << "   --";
			
// 		}
// 		cout << "\n";
// 		}/**/
/*		cout  << endl;
 */
	
}

PathFinder::PathFinder(MapData * pMD,
					   Scr::Position s,
					   Scr::Position f,
					   ObstacleDetector OD,
				CostCalculator CC)
	: 
	pMD1(pMD),start(s),finish(f),OD1(OD),CC1(CC)
{

 	PFD1.reset(new boost::scoped_array<pPFDataEntry>[pMD1->GetHeight()]);

 	for ( unsigned int i = 0 ; i!= pMD1->GetHeight() ; ++i )
 		PFD1[i].reset(new pPFDataEntry[pMD1->GetWidth()]);	
	ComputePath();
}

void PathFinder::Reset()
{
	StartPoints.clear();
 	for ( unsigned int i = 0 ; i!= pMD1->GetHeight() ; ++i )
 		PFD1[i].reset(new pPFDataEntry[pMD1->GetWidth()]);	
}
void PathFinder::ChangeStart(Scr::Position s)
{
	start=s;
	Reset();
	ComputePath();
}
void PathFinder::ChangeFinish(Scr::Position f)
{
	finish=f;
	ComputePath();
}

void PathFinder::ConditionalReset(Scr::Position s,
								  Scr::Position f)
{
	finish=f;
	if (start!=s)
	{
		start=s;
		Reset();
	}
	ComputePath();
}

