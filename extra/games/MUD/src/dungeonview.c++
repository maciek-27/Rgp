#include <algorithm>
#include <tr1/functional>
#include "character.h++"
#include "dungeon.h++"
#include "dungeonview.h++"
#include "dungeonview.h++"
#include "map.h++"
#include <boost/bind.hpp>
#include <unistd.h>
#include <iostream>
#include "autorefreshfacility.h++"
using namespace MUD;

namespace // Autorefreshfacility private
{
	// set of all windows  -  used by AutoRefreshFacility
	// exclusively.  no need to direct
	// usage outside class. Use RegisterWindow and UnregisterWindow
	// instead.
		typedef std::set<Scr::Tk::RootWindow*> WinSet;
		WinSet Windows;
		
		WinSet WindowsForNextRedraw;
		
		// for Windows usage synchronization
		bool running;
		boost::mutex refmtxWindows;

		std::auto_ptr<boost::thread> auto_refresh_tread;
}
AutoRefreshFacility  * AutoRefreshFacility::instance;
AutoRefreshFacility::AutoRefreshFacility() {instance=this;}

void AutoRefreshFacility::DoAR()
{
	using namespace std::tr1;
	while ( 1 )
	{
		usleep(100000);
		{
			if (! running)return; //protecting this line of code w/
								  //mutex will cause deadlock w/ StopThread
				
			boost::mutex::scoped_lock lck(refmtxWindows);
			std::for_each(WindowsForNextRedraw.begin(),WindowsForNextRedraw.end(),
						  bind(&Scr::Tk::RootWindow::ForceOnRedraw,placeholders::_1)
				);
			WindowsForNextRedraw=Windows;
		}
	}
}
void AutoRefreshFacility::StopThread()
{
	using namespace std;
	running = false;
	auto_refresh_tread->join();
	auto_refresh_tread.reset();
	cout << "Done."<<endl;
}
void AutoRefreshFacility::RegisterWindow (Scr::Tk::RootWindow* W)
{
	using namespace boost;
	mutex::scoped_lock lck(refmtxWindows);
	Windows.insert(W);
	if (Windows.size()==1)
	{
		using namespace std;
		cout << "Window created and registered. Spawning auto-refresh thread. " << flush;
		running = true;
		auto_refresh_tread.reset(
			new thread(bind(&AutoRefreshFacility::DoAR,this)));
		cout << "Done."<<endl;
	}
}

void AutoRefreshFacility::UnregisterWindow (Scr::Tk::RootWindow* W)
{
	boost::mutex::scoped_lock lck(refmtxWindows);
	Windows.erase(W);
	WindowsForNextRedraw.erase(W);
	if (Windows.size()==0)
	{
		std::cout << "Last window unregistered. Stopping auto-refresh thread. " << std::flush;
		StopThread();
	}
}
void AutoRefreshFacility::MarkDone (Scr::Tk::RootWindow* W)
{
	boost::mutex::scoped_lock lck(refmtxWindows);
	WindowsForNextRedraw.erase(W);
}

AutoRefreshFacility::~AutoRefreshFacility()
{
	
	refmtxWindows.lock();
	if (running)
	{
		std::cout << "Won't delete AutoRefreshFacility before removal of all windows." << std::endl;
		while (running)
			{
				std::cout << "waiting..." << std::endl;
				refmtxWindows.unlock();
				sleep (1);
				refmtxWindows.lock();
			}
	}
	refmtxWindows.unlock();
	std::cout << "AutoRefreshFacility destroyed." << std::endl;
}

AutoRefreshFacility & AutoRefreshFacility::Instance()
{
	return *instance;
}

MUD::DungeonView::DungeonView(pCharacter pch):pChr(pch)// :position(WM15D5D.createCharacter())
{
	Dungeon::Instance().AddMarker(pChr);
}
void MUD::DungeonView::OnStart() throw()
{
	AutoRefreshFacility::Instance().RegisterWindow(&GetParent().GetRootWindow());
}
void MUD::DungeonView::OnKeyDown(Scr::Key  key) throw ()
{
 	using namespace Scr;
 	using std::make_pair;
 	try
 	{
 		switch (key)
 		{
 		case 'Q':
 		case 'q':
 		case Key::Escape:
 			GetParent().GetRootWindow().Exit(0);
 			break;
 		case Key::Up:
			// place marker on board. it might be useful. 
			Dungeon::Instance().DelMarker(pChr);
 			pChr->Move(make_pair(-1,0));
			Dungeon::Instance().AddMarker(pChr);
 			break;
 		case Key::Down:
			Dungeon::Instance().DelMarker(pChr);
 			pChr->Move(make_pair(1,0));
			Dungeon::Instance().AddMarker(pChr);
 			break;
 		case Key::Right:
			Dungeon::Instance().DelMarker(pChr);
 			pChr->Move(make_pair(0,1));
			Dungeon::Instance().AddMarker(pChr);
 			break;
 		case Key::Left:
			Dungeon::Instance().DelMarker(pChr);
 			pChr->Move(make_pair(0,-1));
			Dungeon::Instance().AddMarker(pChr);
 			break;
 		}
 	}catch(...){}

// 	// force repainting all other windows.
	AutoRefreshFacility::Instance().MarkDone(&GetParent().GetRootWindow());
}
void MUD::DungeonView::OnRedraw(Scr::Screen & scr) throw ()
{
	Dungeon::Instance().DrawScreen(scr,pChr);
}

MUD::DungeonView::~DungeonView()throw ()
{
	Dungeon::Instance().DelMarker(pChr);
	AutoRefreshFacility::Instance().UnregisterWindow(&GetParent().GetRootWindow());
}

