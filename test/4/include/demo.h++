#ifndef __DEMO_H__
#define __DEMO_H__
#include<rexio/screen.h++>
#include<memory>
class Demo:public Scr::Connection
{
private:
    static const int subw_width=40;
    int counter;
    char * linia;
    bool ttypeKnown;
    void Greeting();
    
public:
    Demo(std::istream & in,
	   std::ostream & out);
    void OnStart()throw();
    void WypiszK(Scr::Key key);
    void OnKeyDown(Scr::Key key)throw();
    void OnResize(Scr::Uint rows, Scr::Uint cols)throw();
    void OnExit()throw();
    ~Demo()throw();
};
#endif
