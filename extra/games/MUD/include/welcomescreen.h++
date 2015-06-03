#ifndef __WELCOME_SCREEN_H__
#define __WELCOME_SCREEN_H__
#include <rexio/tk/verticalgroup.h++>
#include <rexio/tk/horizontalgroup.h++>
#include <rexio/tk/label.h++>
#include <rexio/tk/inputbox.h++>
#include <rexio/tk/button.h++>
#include <cassert>
#include <sstream>
#include "textbox.h++"
#include "u8conv.h++"
namespace MUD
{
//! 
//!
class WelcomeScreen : public Scr::Tk::HorizontalGroup
{
	class LoginForm: public Scr::Tk::VerticalGroup
	{
		static const int min_height;
		static const int min_width;
	public:
		LoginForm(Scr::Uint _height,Scr::Uint _width,
				  const Scr::DisplayStyle& _style):
			BoxGroup(std::min<int>(min_height,_height),
					 std::min<int>(min_width,_width),_style),
			VerticalGroup(std::min<int>(min_height,_height),
						  std::min<int>(min_width,_width),_style)
			{
				//SetAlignPolicy(Scr::Tk::BoxGroup::Begin);
				SetMaxHeight(0);
				SetMaxWidth(min_width);
			}
		void AddWidget(Widget& widget)throw()
			{
				BoxGroup::AddWidget(widget);
				SetMaxHeight(GetMaxHeight()+widget.GetHeight());
			}
		void DelWidget(Widget& widget)throw()
			{
				SetMaxHeight(GetMaxHeight()-widget.GetHeight());
				BoxGroup::DelWidget(widget);
			}
		using Scr::Tk::Widget::GetParent;
	}login_form;
	
	Scr::Tk::Label hello;
	Scr::Tk::Label login;
	Scr::Tk::Inputbox login_prompt;
	Scr::Tk::Label password;
	Scr::Tk::Label separator;
	Scr::Tk::Label password_repeat;
	
	MUD::Aux::TextBox textbox;

	
	
	//! inputbox echoing '*' instead of proper characters (GetText()
	//! stull returns string entered).
	class PasswordBox: public Scr::Tk::Inputbox
	{
	public:
		PasswordBox():
			Inputbox(L"",Scr::Tk::INPUTBOX_DEFAULT_STYLE,
						Scr::Tk::INPUTBOX_DEFAULT_ACTIVESTYLE,
					 Scr::Tk::INPUTBOX_DEFAULT_IBOXSTYLE)
			{}
		virtual void OnRedraw(Scr::Screen& screen)throw()
			{
				std::wstring tmp(text);
				fill(text.begin(),text.end(),L'*');
				Inputbox::OnRedraw(screen);
				
				// swaps pointers only
				text.swap(tmp);
			}
	}password_prompt,password_prompt2;

	//! Is used to activate game
	class SubmitButton: public  Scr::Tk::Button
	{
	public:
		SubmitButton():
			Button("Enter",Scr::Tk::BUTTON_DEFAULT_STYLE,
				   Scr::Tk::BUTTON_DEFAULT_ACTIVESTYLE) {}			

		virtual void OnAction()throw()
			{
				Scr::Tk::RootWindow &rw(GetParent().GetRootWindow());
				Scr::Tk::Widget &ws(rw.GetActiveWidget());
				
				// swap main widget : add new DingeonView, eliminate
				// old WelcomeScreen
				WelcomeScreen& par=dynamic_cast<WelcomeScreen&>(
					dynamic_cast<LoginForm&>(GetParent()).GetParent());
		        std::string name=Aux::to_u8(par.login_prompt.GetText());
				std::string psw=Aux::to_u8(par.password_prompt.GetText());
            	pCharacter pch = SelectCharacter(name);
				std::cout << "Trying to connect as " << name << " with password "<<psw<< ' ' ;
            	if (pch.get()==0)
            		par.login.SetText("Login*");
				else
	            if (pch->CheckPassword(psw))
            	{
					std::cout << "OK" << std::endl;
					rw.AddWidget(*new DungeonView(pch));
					std::cout << "DungeonView created; dropping WelcomeScreen " << std::flush;
					std::cout << "OK" << std::endl;
					rw.DelWidget(ws);
					delete &ws;
					return;						
				}
				else
					par.password.SetText("Password*");
				std::cout << "failed" << std::endl;

				par.textbox.ClearText();
				par.textbox.AppendText(L"Please make sure, that all authentication tokens are correct."
					" You may want to create new account instead of logging into an existing one");
			}
	}submit_button;

	class RegisterButton: public Scr::Tk::Button
	{
	public:
		RegisterButton():
			Button("New Account",Scr::Tk::BUTTON_DEFAULT_STYLE,
				   Scr::Tk::BUTTON_DEFAULT_ACTIVESTYLE) {}			
		virtual void OnAction()throw()
			{
				GetParent().PassFocusRequest(Scr::Tk::Widget::AllFocus);
				WelcomeScreen& par=dynamic_cast<WelcomeScreen&>(
					dynamic_cast<LoginForm&>(GetParent()).GetParent());
				par.hello.SetText("New Account");
				par.login_form.DelWidget(par.register_button);
				par.login_form.AddWidget(par.password_repeat);
				par.login_form.AddWidget(par.password_prompt2);
 				par.login_form.AddWidget(par.register_commit_button);

 				par.login_form.DelWidget(par.submit_button);
				par.login_form.OnResize();
				par.textbox.ClearText();
				par.textbox.AppendText(L"You are creating new character. Character name can't be changed"
									   ", so please be careful when choosing name. Character name will be your login. "
									   " \n\nPlease enter password twice. Please don't use the same password, as you use "
									   "in any sensitive system (i.e. bank), as telnet transmission is insecure.");
			}
	}register_button;
	class RegisterCommitButton: public Scr::Tk::Button
	{
	public:
		RegisterCommitButton():
			Button("OK",Scr::Tk::BUTTON_DEFAULT_STYLE,
				   Scr::Tk::BUTTON_DEFAULT_ACTIVESTYLE) {}			
		virtual void OnAction()throw()
			{
					
				GetParent().PassFocusRequest(Scr::Tk::Widget::AllFocus);
				WelcomeScreen& par=dynamic_cast<WelcomeScreen&>(
					dynamic_cast<LoginForm&>(GetParent()).GetParent());
		        std::string name=Aux::to_u8(par.login_prompt.GetText());
				std::string psw=Aux::to_u8(par.password_prompt.GetText());
				std::string psw2=Aux::to_u8(par.password_prompt2.GetText());

				par.textbox.ClearText();
				if (name=="")
					par.textbox.AppendText(L"Name can't be empty");
				else if (name[0]==' ')
					par.textbox.AppendText(L"Name can't be empty");
				else if (SelectCharacter(name).get()!=0)
					par.textbox.AppendText(L"Name, you've selected is already used");
				else if (psw!=psw2)
					par.textbox.AppendText(L"Passwords don't match");
				else
				{
					par.textbox.AppendText(L"New character is created. you can now log in.");
					par.login_form.DelWidget(par.password_repeat);
					par.login_form.DelWidget(par.password_prompt2);
					par.login_form.DelWidget(par.register_commit_button);
					
					par.login_form.AddWidget(par.submit_button);
					SetUpNewCharacter(name,psw);
				}
				
			}
	}register_commit_button;
public:
	
	WelcomeScreen(Scr::Uint _height,	Scr::Uint _width,
				  const Scr::DisplayStyle& _style
				   = Scr::DisplayStyle(Scr::Fg::White,Scr::Fg::Dark,Scr::Bg::Black)):
		BoxGroup(_height, _width,_style),
		HorizontalGroup(_height, _width,_style),
		login_form(_height,_width,_style),
		hello("Please log in", Scr::DisplayStyle(
				  Scr::Fg::White,Scr::Fg::Bright,Scr::Bg::Red)),
		login("Login",Scr::Tk::LABEL_DEFAULT_STYLE),
		login_prompt(L"",Scr::Tk::INPUTBOX_DEFAULT_STYLE,
					 Scr::Tk::INPUTBOX_DEFAULT_ACTIVESTYLE,
					 Scr::Tk::INPUTBOX_DEFAULT_IBOXSTYLE),
		password("Password",Scr::Tk::LABEL_DEFAULT_STYLE),
		separator("  ",Scr::Tk::LABEL_DEFAULT_STYLE),
		password_repeat("Confirm password",Scr::Tk::LABEL_DEFAULT_STYLE),
		textbox(_height,_width,_style)
	{
		AddWidget(login_form);
		SetAlignPolicy(Scr::Tk::BoxGroup::Distribute);
		login_form.SetAlignPolicy(Scr::Tk::BoxGroup::Begin);
		login_form.AddWidget(hello);
		login_form.AddWidget(login);
		login_form.AddWidget(login_prompt);
		login_form.AddWidget(password);
		login_form.AddWidget(password_prompt);
		login_form.AddWidget(submit_button);
		login_form.AddWidget(register_button);
		separator.SetMaxWidth(2);
		AddWidget(separator);
		AddWidget(textbox);
		textbox.AppendText(L"Welcome to experimental game with ascii-art graphics. "
						   "We hope, you will enjoy playing this as much, as we enjoyed creating it.\n\n"
						   "Use Tab key to select form fields.\n\n"
						   "You can always halt the game by pressing Ctrl+d.\n\n"
						   "Programming Team:\n Maciej Kamiński\nArtur Jutrzenka");
	}
};
}
#endif
