#include<rexio/tk/toolkit.h++>
#include<iostream>
#include <fstream>
using namespace std;
using namespace Scr;
using namespace Scr::Tk;

class AplikacjaDemo:public RootWindow
{
public:
//	VerticalGroup group;
	FramedWindowBase<FramedWindowBase<VerticalGroup> > group;
	VerticalGroup group2;

    Label tytulAplikacji;
	HorizontalScrollbar scrollbar;

	Selectbox select;

	class DemoInput: public Inputbox
	{public:
		bool firstFocus;
		DemoInput(const std::wstring &text,
			const Scr::DisplayStyle &style = DisplayStyle())throw();
		void OnFocus(FocusPolicy policy)throw();
	} nameAndSurname, nameAndSurname2, address,  address2;		

    class DemoCheckbox: public Checkbox
    {public:
		DemoCheckbox(std::string _label)throw();
	} pieczarki, pieczarki2,szynka,ser,szynka2, ser2;
    class Czyszczenie: public Button
    {public:
		Czyszczenie()throw();
		void OnAction()throw();
    } czyszcz, czyszcz2;
    
    class Konczenie: public Button
    {
    public:
		std::string content;
		Konczenie()throw();
		void OnAction()throw();
		void SetStylesheet(Stylesheet* _styleSheet)throw() {
			Button::SetStylesheet(_styleSheet);
			__FetchPropertyDefault(content, "content",
								   "Hey! How about you test the stylesheet?");
			SetLabel(content);
		}
		~Konczenie()throw() {;};
    }koncz, koncz2;
    AplikacjaDemo()throw();	
    ~AplikacjaDemo()throw();
	
	void OnRedraw(Scr::Screen &screen)throw();
	void OnResize()throw();
	void OnKeyDown(Key key)throw() {
		RootWindow::OnKeyDown(key);
		if(key == 'q')
			Exit(1);
	}
} * app;

AplikacjaDemo::DemoInput::DemoInput(const std::wstring & text, 
									const Scr::DisplayStyle& style)throw()
    :Inputbox(25, text,
			  DisplayStyle(Fg::White, Fg::Dark, Bg::Black), 
			  DisplayStyle(Fg::White, Fg::Bright, Bg::Black),
			  InputboxStyle(style)), firstFocus(true)
{
}

void AplikacjaDemo::DemoInput::OnFocus(FocusPolicy policy)throw()
{
	Inputbox::OnFocus(policy);
	if(firstFocus) {
		SetText(L"");
		firstFocus = false;
	}
}

AplikacjaDemo::DemoCheckbox::DemoCheckbox(std::string _label)throw()
    :Checkbox(Label(_label))
{	
	SetMaxHeight(1);	
}

AplikacjaDemo::Czyszczenie::Czyszczenie()throw()
    :Button(1, 25, "Wyzeruj formatkę")
{
	SetMaxHeight(3);
	objectName = "clear";
}

void AplikacjaDemo::Czyszczenie::OnAction()throw()
{
    app->pieczarki.SetState(false);
    app->szynka.SetState(false);
    app->ser.SetState(false);
	app->nameAndSurname.SetText(L"Imię i nazwisko");
	app->address.SetText(L"Miejsce zamieszkania");
}

AplikacjaDemo::Konczenie::Konczenie()throw()
    :Button(1, 25, "\341\220\201Koniec") // 4-byte UTF-8 not supported
				         // by Konsole \360\235\214\216
{
	SetMaxHeight(UintMax);
	objectName = "end";
}

void AplikacjaDemo::Konczenie::OnAction()throw()
{
    app->Exit(0);
}

AplikacjaDemo::AplikacjaDemo()throw()
    :RootWindow(std::cin,std::cout),
	 group(23, 50, GetStyle()), group2(20, 30, GetStyle()),
	 tytulAplikacji("Wybierz dodatki do pizzy", GetStyle()), scrollbar(15),
	 nameAndSurname(L"Imię i nazwisko"),
	 nameAndSurname2(L"Imię i nazwisko"),
	 address(L"Miejsce zamieszkania", DisplayStyle(Fg::Green, Fg::Bright, Bg::Yellow)),
	 address2(L"Miejsce zamieszkania", DisplayStyle(Fg::Green, Fg::Bright, Bg::Yellow)),
     pieczarki("Pieczarki"),
     pieczarki2("Pieczarki"),
     szynka("Szynka"),ser("Dodatkowy ser"),
     szynka2("Szynka"),ser2("Dodatkowy ser")
{	
	AddWidget(group);
	AddWidget(group2);

	AddWidget(scrollbar);
	scrollbar.SetPosition(2, 30);

	address.objectName = "adres";
	tytulAplikacji.SetMaxHeight(3);

    group.AddWidget(tytulAplikacji);	
    group.AddWidget(pieczarki);
    group.AddWidget(szynka);
    group.AddWidget(ser);
	group.AddWidget(nameAndSurname);
	group.AddWidget(address);
	group.AddWidget(select);
	select.AddOption(("dupa1"));
	select.AddOption(("dupa2"));
    group.AddWidget(czyszcz);	

	group.AddWidget(koncz);
	group.SetPosition(1, 1);

	group2.AddWidget(pieczarki2);
	group2.AddWidget(szynka2);
	group2.AddWidget(ser2);
	group2.AddWidget(nameAndSurname2);
	group2.AddWidget(address2);
	group2.AddWidget(czyszcz2);
	group2.AddWidget(koncz2);
	group2.SetPosition(1, 1 + group.GetWidth());	
}

void AplikacjaDemo::OnResize()throw()
{
//	group.SetPosition(GetHeight()/2 - group.GetHeight()/2, 
//					  GetWidth()/2 - group.GetWidth()/2);
//	group.SetPosition(0, 0);
//	group.SetSize(Size(GetHeight()/2, (GetWidth()/2)));

	RootWindow::OnResize();	
}

void AplikacjaDemo::OnRedraw(Scr::Screen& screen)throw()
{
	RootWindow::OnRedraw(screen);
	screen << DisplayStyle(Fg::Red, Fg::Bright, Bg::Black)
		   << Control::GotoYX(1,5) << "Dupa"
		   << Control::GotoYX(1,15) << "<1,15"
		   << Control::GotoYX(2,5) << "こんいちは"
		   << Control::GotoYX(2,8) << "こ"
		   << Control::GotoYX(2,15) << "<2,15"

		;
	
	screen << DisplayStyle(Fg::Black, Fg::Dark, Bg::Yellow)
		   << Control::GotoYX(2,3) << "w";
	
	screen.GotoYX(GetHeight()/2+7, GetWidth()/2-3);
	screen.Rectangle(L'ち',Size(3,3));
	screen.GotoYX(GetHeight()/2+3, GetWidth()/2-3);
	screen.Rectangle(L'j',Size(3,3));
}

AplikacjaDemo::~AplikacjaDemo()throw()
{

}

int main (int argc,char ** argv)
{
	app = new AplikacjaDemo;	
	int result = app->Start(argc, argv);	
	delete app;
	return result;
}
/*end of main function of program*/ 
