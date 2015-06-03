#include "demo.h++"
#include "netconn.h++"
#include "main.h++"

using namespace Scr;
using namespace Scr::Tk;

Demo::Demo(std::istream & in,
		   std::ostream & out)throw() :
	RootWindow(in, out), 
	maing(GetHeight(), GetWidth()),
	infoBar("RexIO chat application."),
	centerg(GetHeight() - 2, GetWidth()),
	msgInput(GetWidth()),
	msgList(0, 0),
	nickList(0, 0)
{
	infoBar.objectName = "infobar";
	msgList.objectName = "msglist";
	nickList.objectName = "nicklist";
	msgInput.objectName = "msginput";
}

void Demo::MessageInput::OnKeyDown(Key key)throw()
{
	if(key.IsASpecialKey())
		if(key.GetSpecialKey() == Key::Enter) {
			s.MessageEvent(static_cast<Demo&>(GetParent().GetRootWindow()).userInfo, GetText());
			SetText(L"");
			return;
		}
	SetActive(true);
	Inputbox::OnKeyDown(key);
}

Demo::MessageInput::MessageInput(Uint width)throw() : Inputbox(width, L"")
{
	SetMaxLength(100);
}

Demo::LoginWindow::LoginButton::LoginButton()throw() :
	Scr::Tk::Button(1, 12, "Connect")
{
	;
}

void Demo::LoginWindow::LoginButton::OnAction()throw()
{
	Demo &demo = static_cast<Demo&>
		(GetParent().GetRootWindow());
	demo.maing.SetHidden(false);
	demo.login.SetHidden(true);
	demo.OnFocus(TabFocus);
	demo.userInfo = UserInfo(demo.login.nameInput.GetText());
	s.JoinEvent(demo.userInfo);
}

void Demo::OnResize()throw()
{
	RootWindow::OnResize();
	maing.SetSize(Size(GetHeight(), GetWidth()));
	msgInput.SetSize(1, GetWidth());
	if(elements[&login] != elements.end())
		login.SetPosition((GetHeight() - login.GetHeight())/2,
						  (GetWidth() - login.GetWidth())/2);
}

void Demo::OnStart()throw()
{
	AddWidget(login);

	AddWidget(maing);
	maing.SetHidden(true);
	maing.AddWidget(infoBar);
	maing.AddWidget(centerg);
	maing.AddWidget(msgInput);

	centerg.AddWidget(msgList, 4);
	centerg.AddWidget(nickList);	
}

void Demo::MessageEvent(const UserInfo &info,
						const std::wstring& msg)throw()
{
	msgList.msgs.push_back(msg);
	msgList.umsgs.push_back(info);
	RedrawRequest();
}

void Demo::JoinEvent(const UserInfo& info)throw()
{
	nickList.nicks = s.nicks;
	RedrawRequest();
}

void Demo::LeaveEvent(const UserInfo& info)throw()
{
	nickList.nicks.remove(info.userName);
	s.nicks.push_back(info.userName);
	RedrawRequest();
}

Demo::~Demo()throw()
{
	s.LeaveEvent(userInfo);
}

