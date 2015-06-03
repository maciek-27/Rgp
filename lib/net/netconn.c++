
#include <tr1/memory>

#include <iostream>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ext/stdio_filebuf.h>
#include <unistd.h> /* sleep*/
#include <stack>
#include <cstring>
#include <rexio/net.h++>
#include <rexio/throw.h++>
#include <rexio/commons.h++>
#include <set>
using namespace std;
using namespace RexIO::Networking;
using namespace std::tr1;

namespace {
    pthread_mutex_t allprogsStackMutex;

    /*!
     * Management object for any RootWindows ran by specific instance as thread
     **/
    class ProgEntry {
    private:
        Scr::Tk::RootWindow* d;
    public:
        //! Constructor associates object with specific RootWindow

        ProgEntry(Scr::Tk::RootWindow * _d) : d(_d) {
            ;
        }

        //! destructor implements basic memory management activity : deletes managed
        //! entity

        ~ProgEntry() {
            d->Exit(4);
            cout << "Deleting connection\n";
        }
        friend bool operator<(const ProgEntry& a, const ProgEntry& b);

    };

    // for set<ProgEntry> underlaying tree.

    bool operator<(const ProgEntry& a, const ProgEntry& b) {
        // comparing addresses of a and b would be wrong, as they may
        // differ while referencing the same Scr::Tk::RootWindow class object.
        return a.d < b.d;
    }
} // end - empty namespace

set<ProgEntry> allprogs;

#include <memory> // for smart pointers
void ServerImpl::starter(Scr::Tk::RootWindow * w) {
    std::auto_ptr<Scr::Tk::RootWindow> prog(w);

    pthread_mutex_lock(&allprogsStackMutex); // prevent accidental stack
    allprogs.insert(ProgEntry(prog.get())); // data structure destruction
    pthread_mutex_unlock(&allprogsStackMutex);
    cout << "Trying to initialize connection" << endl;
    try {
        int i = prog->Start(); // start
        cout << "Connection finished with code " << i << endl; // result
        // on success
    }    catch (exception)// exception caught. try to recover by ignoring it.
    {
        cout << "Connection finished with exception, but app is fine." << endl;
    }
    pthread_mutex_lock(&allprogsStackMutex);
    cout << "Requesting erase of 1 app out of " << allprogs.size() << endl;
    ;

    // if ProgEntry exists (not deleted by localInterface func)
    if (allprogs.find(ProgEntry(prog.get())) != allprogs.end())
        allprogs.erase(ProgEntry(prog.get())); //erase it.
    cout << endl;
    pthread_mutex_unlock(&allprogsStackMutex);
    return;
}




using Scr::FatalException;
namespace RexIO { namespace Networking {
/*Class for internal use representing reprezenting initialization
 and termination of connection*/
class __Connection {
private:
    int fd;
    pthread_t th;

    FILE * oF;
    __gnu_cxx::stdio_filebuf<char> * obuf;
    ostream * ostr;

    FILE * iF;
    __gnu_cxx::stdio_filebuf<char> * ibuf;
    istream * istr;
    ServerImpl * simpl;
    static void * ServeConnection(void * conn);
    shared_ptr<sockaddr>addr_in;
public:

    __Connection(int _fd, ServerImpl * simpl,shared_ptr<sockaddr>&addr);
    ~__Connection();
};
/* connection */
}}
/* callback function serving connection*/
//ConnectionFunc f;

/*for joining "dead" threads*/
stack<pthread_t> CleanerStack;
pthread_mutex_t CleanerStackMutex;

__Connection::__Connection(int _fd, ServerImpl * simpl,shared_ptr<sockaddr>&addr)
:addr_in(addr) {
    fd = _fd;
    this->simpl = simpl;
    cout << "Accepted connection; fd = " << fd << endl;
    iF = fdopen(fd, "r");
    oF = fdopen(fd, "w");
    ibuf = new __gnu_cxx::stdio_filebuf<char>(iF, std::ios_base::in, 1);
    obuf = new __gnu_cxx::stdio_filebuf<char>(oF, std::ios_base::out, 1);
    istr = new std::istream(ibuf);
    ostr = new std::ostream(obuf);

    pthread_create(&th, NULL, ServeConnection, this);
}

__Connection::~__Connection() {
    delete istr;
    delete ibuf;
    fclose(iF);

    delete ostr;
    delete obuf;
    fclose(oF);
    close(fd);
    pthread_mutex_lock(&CleanerStackMutex);
    CleanerStack.push(th);
    pthread_mutex_unlock(&CleanerStackMutex);

}

void * __Connection::ServeConnection(void * _conn) {
    __Connection * conn = (__Connection *) _conn;
    cout << "in thread for conn fd: " << conn->fd << endl;

    Scr::Tk::RootWindow * rw =
    conn->simpl->GenWindow(*(conn->istr),*(conn->ostr));
    conn->simpl->starter(rw);
    delete conn;
    return 0;
}

void * CleanerFunc(void * activity_mark) {

    while (* static_cast<bool*> (activity_mark)) {
        sleep(2);
        pthread_mutex_lock(&CleanerStackMutex);
        while (!CleanerStack.empty()) {
            pthread_t t = CleanerStack.top();
            pthread_join(t, NULL);
            CleanerStack.pop();
            cout << "Joined thread" << endl;
        }
        pthread_mutex_unlock(&CleanerStackMutex);
    }
    return 0;
}

ServerImpl::ServerImpl() {
    ;
}

void ServerImpl::Start(int portnum) {

    active = true;
    struct sockaddr_in srv;
    socklen_t socklen;
    int iSockFD;
    if ((iSockFD = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        THROWP(FatalException, "socket");

    int opt = 1, len = 4;
    setsockopt(iSockFD, SOL_SOCKET, SO_REUSEADDR, &opt, len);

    memset(&srv, 0, sizeof (srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = htonl(INADDR_ANY);
    srv.sin_port = htons(portnum);

    socklen = sizeof (srv);

    if (bind(iSockFD, (struct sockaddr *) & srv, socklen) < 0)
        THROWP(FatalException, "bind");

    int fd;
    listen(iSockFD, 5);

    if (pthread_mutex_init(&CleanerStackMutex, NULL))
        THROWP(FatalException, "mutex_initialize");
    pthread_t cleaner_thread;

    if (fcntl(iSockFD, F_SETFL, O_NDELAY) < 0)
        THROWP(FatalException, "Can't make nonblocking socked");

    if (pthread_create(&cleaner_thread, NULL, CleanerFunc, &active))
        THROWP(FatalException, "pthread_create (&cleaner_thread,NULL,CleanerFunc,NULL)");

    while (active) {
        tr1::shared_ptr<sockaddr>addr_in(new sockaddr);
        fd = accept(iSockFD, addr_in.get(), & socklen);
        if (fd > 0)
            new __Connection(fd,this,addr_in);
        else
            usleep(1000);
    }
    if (pthread_join(cleaner_thread, NULL))
        THROWP(FatalException, "pthread_join(&cleaner_thread,NULL)");
    pthread_mutex_destroy(&CleanerStackMutex);
    close(iSockFD);
}

void ServerImpl::Stop() {
    active = false;
    allprogs.clear();
}
