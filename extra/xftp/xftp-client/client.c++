#include <libxftp/xftpclient.h++>
#include <libxftp/clientcontrol.h++>
#include <libxftp/message.h++>
#include <libxftp/servercontrol.h++>
#include <boost/filesystem.hpp>

#include "supercontrol.h++"
#include "superdata.h++"
#include "client.h++"
#include "superduperclient.h++"



using namespace xftp;

namespace xftpclient 
{
	boost::filesystem::path CurrentPath;
	std::tr1::shared_ptr<Message> RemoteList;
}

using namespace xftpclient;
// for i18n
#include<libintl.h>
#include <locale.h>


SuperDuperClient app;

int main(int argc,char**argv)
{
	using namespace std;
	setlocale(LC_ALL, "");
	bindtextdomain("xftp", "locale");//"usr/share/locale"
	textdomain("xftp");
	
	try {
		app.Connect(
			argc==1?"127.0.0.1":argv[1], defaultPort, defaultStreams);
	} catch(Connection::ConnectionSevered) {
		cout << gettext("Disconnected.") << endl;
	} catch(Connection::ConnectionRefused) {
		cout << gettext("Refused.") << endl;
	} catch(exception & e) {
		cout << gettext("Error: ") << e.what() << endl;
	} catch(...){
		cout << gettext("Unexpected error.")  << endl;
	}
	return 0;
}
//  xgettext -d xftp -o locale/xftp.pot -s xftp-client/*.c++
//  msginit -l pl_PL.utf8 -o locale/pl/LC_MESSAGES/xftp.po -i xftp.pot

