#include<rexio/screen.h++>
#include"demo.h++"
#include<iostream>
#include<iomanip>
#include<cstdio>
using namespace Scr;

void Demo::Greeting()
{
    screen->SetBgColor(Bg::Blue);
    screen->SetFgColor(Fg::Yellow);
    screen->SetFgStyle(Fg::Bright);
    screen->Clear();
    //screen->GotoYX(5,11);
	    
    *screen<<Control::GotoYX(5,12)<<"Hello ";
    screen->SetFgStyle(Fg::Dark);
	try
	{
		ttypeKnown=true;
		*screen<<screen->GetType() ;
	}
	catch (Screen::TerminalTypeUnknown)
	{
		ttypeKnown=false;
		*screen<< ", what is your terminal type?";
	}
		
}
    
Demo::Demo(std::istream & in,
	   std::ostream & out):Connection(in,out)
{
    counter = 0;
    linia = new char[subw_width+1];
    for (int i = 0 ; i != subw_width ; i++)
	linia[i]='-';
    linia[subw_width]=0;
}

void Demo::OnStart()throw()
{
    Greeting();
    try	
    {screen->Refresh();}
    catch (Screen::ConnectionError){Exit(1);}
}

void Demo::WypiszK(Key key)
{

	if (!ttypeKnown)
		Greeting(); // retry: maybe now we know term type (client
					// managed to introduce itself)
    try {
	std::auto_ptr<Screen>ss(
	    screen->CreateSubScreen( 9+counter%10,1 ,2,subw_width));
	
	char  text[40];
	sprintf(text," (kod ascii: %lud, %lxh)",
		(unsigned long)key,(unsigned long)key);
	
	screen->SetBgColor(Bg::Red);
	ss->Clear();
	*ss<<Fg::Yellow<<Fg::Dark
	   <<"blah: " // AddText("Dodano klawisz ");
	   <<Fg::Green<<Fg::Dark
	   << ( (key>=' ' && key <=127)? static_cast<char>(key):'?')
	   <<Fg::Yellow<<Fg::Dark;
	try
	{
		*ss << text;
	}
	catch (Screen::PrintOutOfRange)
    {
		*ss<<"Too long text";
    }

	ss->GotoYX(1,0);
	ss->AddText(linia);
	ss->Refresh();
    }
    catch (Screen::SubscreenOutOfRange)
    {
	return;
    }
    catch (Screen::ConnectionError)
    {
	Exit(1);
    }

}

void Demo::OnKeyDown(Key key)throw()
{
//	    screen->GotoYX(9+counter%10,9);
    try// too large key code (over 4 digits) == string longer
    {  // window width == exception thrown.
	WypiszK(key);
    }
    catch(Screen::PrintOutOfRange & e)
    {
	screen->GotoYX(3,3);
	*screen<<e.what();
	try	
	{screen->Refresh();}
	catch (Screen::ConnectionError){Exit(1);}
    }

    counter++;
	if (key == Key::EoF)
		Exit(1);
}
void Demo::OnResize(Uint rows, Uint cols)throw()
{;
    try	
    {
	Greeting();
	std::auto_ptr<Screen>ss(
	    screen->CreateSubScreen( rows-6,cols-21 ,5,20));
	*ss <<Bg::Black
	    <<Control::Clear
	    <<Control::GotoYX(1,1)<<"Zmiana rozmiaru:"
	    <<Control::GotoYX(3,1)<<rows<<'x'<<cols
	    <<Control::Refresh;
    }
    catch (Screen::SubscreenOutOfRange)
    {
	return;
    }
    catch(std::exception)
    {
	Exit(2);
    }
}

void Demo::OnExit()throw()
{
    ;
}

Demo::~Demo()throw()
{
    delete[] linia; 
}

/*end of main function of program*/
