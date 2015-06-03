#include<rexio/screen.h++>
#include"manager.h++"
#include"main.h++"
#include<iostream>
#include<iomanip>
using namespace Scr;

Manager::Manager():Connection(std::cin,std::cout){;}

void Manager::OnStart()throw()
{
    *screen << Fg::Bright << Fg::White << Bg::Black
	    <<Control::Clear << Control::Refresh;
    std::cout << "Welcome to testing application. press q to quit.\n" <<
	      std::flush;    
    pthread_mutex_unlock(&startSequenceMutex);
}

void Manager::OnKeyDown(Scr::Key key)throw()
{
    if (key=='q') Exit(0);
    std::cout << static_cast<char>(key) << std::flush;
}
