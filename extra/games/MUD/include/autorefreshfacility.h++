#ifndef __AUTO_REFRESH_FACILITY__H__
#define __AUTO_REFRESH_FACILITY__H__
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <set>
#include "gamewindow.h++"

namespace MUD
{

	struct AutoRefreshFacility {
		friend class GlobalGameStatus;
	private:
		static AutoRefreshFacility  * instance;
		
		AutoRefreshFacility();
		void DoAR();
		
		void StopThread();

	public:
		void RegisterWindow (Scr::Tk::RootWindow* W);
		void UnregisterWindow (Scr::Tk::RootWindow* W);
		void MarkDone (Scr::Tk::RootWindow* W);
		static AutoRefreshFacility & Instance();
		~AutoRefreshFacility();
	};
}
#endif
