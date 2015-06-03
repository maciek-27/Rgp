#ifndef __DEMO_H__
#define __DEMO_H__

#include <rexio/tk/toolkit.h++>
#include "netconn.h++"
#include "manager.h++"

namespace Scr {
	class Demo:public Tk::RootWindow
	{
	public:
		UserInfo userInfo;
	protected:
		class MessageInput : public Tk::Inputbox
		{
		public:
			MessageInput(Uint width)throw();
			void OnKeyDown(Key key)throw();
			void OnFocus(FocusPolicy focustype)throw() {				
				Tk::Inputbox::OnFocus(focustype);				
			} // steal focus
		};
		class LoginWindow : public Scr::Tk::FramedWindow
		{
		public:
			LoginWindow()throw() :
				FramedWindow(20, 50),
				welcome("Welcome to RexIO chat!"),
				loginInfo("Provide your nickname and press Connect"),
				nameInput(30, L"..Your nickname here.."),
				okButton() {
				objectName="login";
				welcome.objectName="welcome2";
				welcome.SetWidth(46);
				AddWidget(welcome);
				AddWidget(loginInfo);
				loginInfo.SetPosition(2, 0);

				nameInput.SetMaxLength(30);
				nameInput.objectName="nickinput";
				AddWidget(nameInput);
				nameInput.SetPosition(4, 2);
				okButton.objectName="okbutton";
				AddWidget(okButton);
				okButton.SetPosition(4, 34);

				rexioInfo[0].SetText("RexIO is library for console user interfaces.");
				rexioInfo[0].objectName = "rexio1";
				rexioInfo[1].SetText("It provides support for a vast variaty of");
				rexioInfo[1].objectName = "rexio2";
				rexioInfo[2].SetText("terminals and connection types (unified in-");
				rexioInfo[2].objectName = "rexio3";
				rexioInfo[3].SetText("terface for local and remote terms, TERMINFO");
				rexioInfo[3].objectName = "rexio4";
				rexioInfo[4].SetText("and more. See wwww.rexio,org for reference.");
				rexioInfo[4].objectName = "rexio5";

				for(int i = 0;i< 5;i++) {
					AddWidget(rexioInfo[i]);
					rexioInfo[i].SetPosition(7 + i, 0);
					rexioInfo[i].SetSize(1, 47);
					rexioInfo[i].SetStyle(DisplayStyle(Fg::Transparent,
													   Fg::Bright,
													   Bg::Transparent));
				}
				AddWidget(rexlogo);
				rexlogo.SetPosition(12, 8);
			}
			class LoginInput : public Scr::Tk::Inputbox
			{
			public:
				LoginInput(Uint size, const std::wstring &text)throw()
					: Scr::Tk::Inputbox(size, text) {;};
				bool firstfocus;
				void OnFocus(FocusPolicy focustype)throw() {					
					if(!firstfocus) {
						firstfocus = true;
						SetText(L"");
					}
					Scr::Tk::Inputbox::OnFocus(focustype);
				}
			};

			class LoginButton : public Scr::Tk::Button
			{
			public:

				LoginButton()throw();
				
				void OnAction()throw();
				~LoginButton()throw() {;}
				
			};	

			Scr::Tk::Label welcome;
			Scr::Tk::Label loginInfo;
			LoginInput nameInput;						

			LoginButton okButton;
			
			Scr::Tk::Label rexioInfo[5];
			RexLogo rexlogo;

			RTTI_OBJ(LoginWindow, FramedWindow);
		};
		LoginWindow login;

		class NickList : public Scr::Tk::Window
		{
		public:
			std::list<std::wstring> nicks;

			NickList(Uint _height, Uint _width)throw() :
				Scr::Tk::Window(_height, _width)
				{;}
			void OnRedraw(Screen &scr)throw() {
				Window::OnRedraw(scr);
				try {
					int cnt = 0;
					for(std::list<std::wstring>::iterator
							i = nicks.begin() ; i != nicks.end() ; i++) {						
						scr << Control::GotoYX(cnt++, 0);
						scr << (*i);
					}
//					scr << Control::Refresh;
				} catch(...) {
					;
				}
			}
			~NickList()throw(){;};
			RTTI_OBJ(NickList, Window);
		};
		class MsgList : public Scr::Tk::Window
		{
		public:
			std::list<std::wstring> msgs;
			std::list<UserInfo> umsgs;
			Scr::DisplayStyle nickColor;

			MsgList(Uint _height, Uint _width)throw() :
				Scr::Tk::Window(_height, _width)
				{;}
			void OnRedraw(Screen &scr)throw() {
				Window::OnRedraw(scr);
				try {
					Uint cnt = 0;
					std::list<UserInfo>::reverse_iterator ui =
						umsgs.rbegin();
					for(std::list<std::wstring>::reverse_iterator
							i = msgs.rbegin() ; i != msgs.rend() ; i++) {
						if(cnt > GetHeight() - 1)
							break;
						
						scr << Control::GotoYX(GetHeight() - 1 - (cnt++),
											   0) << nickColor << 
							(*ui).userName << ": " << GetStyle() << (*i);
						ui++;						
					}
//					scr << Control::Refresh;
				} catch(...) {
					;
				}
			}
			virtual void SetStylesheet(Stylesheet* _styleSheet)throw() {
				Window::SetStylesheet(_styleSheet);
				__FetchProperty(nickColor, "nickColor");
			}
			~MsgList()throw(){;};
			RTTI_OBJ(MsgList, Window);
		};
		
		Scr::Tk::VerticalGroup maing;
		
		Scr::Tk::Label infoBar;
		Scr::Tk::HorizontalGroup centerg;
		MessageInput msgInput;
		
		MsgList msgList;
		NickList nickList;
		
		std::vector<Label> nicklist;
		std::vector<Label> msglist;
	public:
		Demo(std::istream & in, std::ostream & out)throw();
		void OnResize()throw();
		void OnStart()throw();
		void MessageEvent(const UserInfo& info,
						  const std::wstring& msg)throw();
		void JoinEvent(const UserInfo& info)throw();
		void LeaveEvent(const UserInfo& info)throw();
		
		~Demo()throw();
	};
}
#endif
