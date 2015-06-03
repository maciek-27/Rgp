#ifndef __MANAG_H__
#define __MANAG_H__
#include<rexio/screen.h++>

class Manager:public Scr::Connection
{
public:
    Manager();
    void OnStart()throw();
    void OnKeyDown(Scr::Key key)throw();
};
#endif
