#include<rexio/screen.h++>
#include<iostream>
#include<iomanip>
#include<cstdio>
class my_conn:public Scr::Connection
{
private:

	static const int subw_width=40;
    int counter;
    wchar_t * linia;
    
    void HW()
		{
			screen->SetBgColor(Scr::Bg::Blue);
			screen->SetFgColor(Scr::Fg::Yellow);
			screen->SetFgStyle(Scr::Fg::Bright);
			screen->Clear();
			//screen->GotoYX(5,11);
	    
			*screen<<Scr::Control::GotoYX(5,12)<<"Joł Земля \332\232";
			screen->SetFgStyle(Scr::Fg::Dark);
			*screen<<L"World (Pozdrowienia od Ędwarda Ąckiego)";
			*screen << Scr::DisplayStyle(Scr::Fg::White,Scr::Fg::Bright,Scr::Bg::Blue);
			screen->GotoYX(8,2);
			screen->HorizontalLine(static_cast<wchar_t>(0x2550),42);
			screen->GotoYX(9,2);
			screen->VerticalLine(static_cast<wchar_t>(0x2551),11);
			screen->GotoYX(9,43);
			screen->VerticalLine(static_cast<wchar_t>(0x2551),11);
			screen->GotoYX(20,2);
			screen->HorizontalLine(static_cast<wchar_t>(0x2550),42);
			screen->GotoYX(8,2);
			screen->AddCharacter(static_cast<wchar_t>(0x2554));
			screen->GotoYX(8,43);
			screen->AddCharacter(static_cast<wchar_t>(0x2557));
			screen->GotoYX(20,2);
			screen->AddCharacter(static_cast<wchar_t>(0x255A));
			screen->GotoYX(20,43);
			screen->AddCharacter(static_cast<wchar_t>(0x255D));
			
			try
			{
			screen->GotoYX(30, 10);
			*screen << "こんいちは";
			screen->GotoYX(30, 21);
			*screen << "<Pozycja 30, 21";
			screen->GotoYX(31, 21);
			*screen << "<Pozycja 31, 21";
			screen->GotoYX(31, 10);
			*screen << "Koszmar ...";
			screen->GotoYX(30, 2);
			*screen << "Tu można";
			}
			catch(Scr::Screen::RangeError)
			{
			screen->GotoYX(0, 0);
			screen->AddText("Make screen bigger to see some japaneese text");
			
			}
		}
    
public:

	
    my_conn():Scr::Connection(std::cin,std::cout)
		{
			counter = 0;
			linia = new wchar_t[subw_width+1]; 
			for (int i = 0 ; i != subw_width ; i++)
				linia[i]=0x2500;
			linia[subw_width]=0;
		}
    void OnStart()throw()
		{
			try
			{
				HW();
			}
			catch (Scr::Screen::RangeError)
			{
				Exit(1);
			}
			catch (...)
			{
				Exit(2);
			}
			screen->Refresh();
		}

    void WypiszK(Scr::Key key)
		{
			std::auto_ptr<Scr::Screen>ss(
				screen->CreateSubScreen( 9+counter%10,3 ,2,subw_width));

			screen->SetBgColor(Scr::Bg::Red);
			ss->Clear();

			if (key.IsABasicKey())
			{
				*ss<<Scr::Fg::Yellow<<Scr::Fg::Dark
				   <<"litera: "
				   <<Scr::Fg::Green<<Scr::Fg::Dark
				   << ( (key>=' ' && key <=127)? static_cast<char>(key):'?')
				   <<Scr::Fg::Yellow<<Scr::Fg::Dark;

				char  text[40];
				sprintf(text," (kod ascii:  %ld %lxh)",(unsigned long)key,(unsigned long)key);
				
				*ss << text				   <<Scr::Fg::Green<<Scr::Fg::Dark
					<<key.GetBasicKey();
			}
			else
				*ss << Scr::Fg::Yellow<<Scr::Fg::Dark
					<< "Przycisk: "<<Scr::Fg::Green<<Scr::Fg::Dark
					<<key.GetKeyName();

			*ss << Scr::Fg::White << Scr::Fg::Bright;  
			if ( (counter+1)%10 )
			{
				ss->GotoYX(1,0);
				ss->AddText(linia);

				screen->SetBgColor(Scr::Bg::Blue);
				screen->GotoYX(10+counter%10,2);
				screen->AddCharacter(static_cast<wchar_t>(0x255F));
				screen->GotoYX(10+counter%10,43);
				screen->AddCharacter(static_cast<wchar_t>(0x2562));
			}
			
			ss->Refresh();

		}
    void OnKeyDown(Scr::Key key)throw()
		{

			if (key==Scr::Key::EoF)
				Exit(0);
			else
				try// too large key code (over 4 digits) == string longer
				{  // window width == exception thrown.
					WypiszK(key);
				}
				catch(Scr::Screen::PrintOutOfRange & e)
				{
					screen->GotoYX(3,3);
					*screen<<e.what();
					screen->Refresh();
				}

			counter++;
		}
    void OnResize(Scr::Uint rows, Scr::Uint cols)throw()
		{;
			try
			{
				HW();
			}
			catch (Scr::Screen::RangeError)
			{
				Exit(1);
			}
			catch (...)
			{
				Exit(2);
			}
				std::auto_ptr<Scr::Screen>ss(
					screen->CreateSubScreen( rows-6,cols-21 ,5,20));
				*ss <<Scr::Bg::Black
					<<Scr::Control::Clear
					<<Scr::Control::GotoYX(1,1)<<"Zmiana rozmiaru:"
					<<Scr::Control::GotoYX(3,1)<<rows<<'x'<<cols
					<<Scr::Control::Refresh;

		}
    void OnExit(int kod)throw()
		{
			;//ekran pożegnalny??
		}
    ~my_conn()throw()
		{
			delete[] linia;//Przykładowo dealokacja jakichś zasobów
		}
};
int main (int argc,char ** argv)
{
    my_conn().Start();

    return 0;
}
/*end of main function of program*/
