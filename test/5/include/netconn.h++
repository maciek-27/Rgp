#ifndef __NETCONN_H__
#define __NETCONN_H__
#include <rexio/screen.h++>
#include <list>

typedef void (*ConnectionFunc)(std::istream & in, std::ostream & out) ;

class UserInfo
{
public:
	UserInfo(const std::wstring& user)throw() : userName(user) {
		;
	}
	UserInfo()throw() {;};
	std::wstring userName;
	Scr::DisplayStyle userColor;	
};

class Server
{

private:
    bool active;
public:
    Server();
	std::list<std::wstring> nicks;

    void Start(int portnum, ConnectionFunc _f);
    void Stop();
	
	void MessageEvent(const UserInfo& info,
					  const std::wstring& msg)throw();
	void JoinEvent(const UserInfo& info)throw();
	void LeaveEvent(const UserInfo& info)throw();
};
#endif
