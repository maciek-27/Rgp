#ifndef __NETCONN_H__
#define __NETCONN_H__
#include <iostream>
#include <rexio/tk/rootwindow.h++>
namespace RexIO { namespace Networking {
    /*!
     * Virtual base for server implementation has almost all code needed to
     * operate as RexIO server. This class facilitates thread management, window
     * creation and so on.
     * \note this class is not guaranteed to be thread safe. it uses some global
     * data structures, and was not designed with many RexIO servers operated
     * within one process, so please avoid id
     */
class ServerImpl {
    // some of internal classes
    friend class __Connection;
private:
    // used by stop
    bool active;
    static void starter(Scr::Tk::RootWindow * w);
protected:
    virtual Scr::Tk::RootWindow *
    GenWindow(std::istream & in, std::ostream & out) = 0;
public:
    //! default constructor
    ServerImpl();
    //! start listening on specified port number
    //! \param portnum port number
    void Start(int portnum);
    //! end listening, send "terminate" messages to all clients. Then end.
    //! \note this function is not guaranteed to succeed: if any thread is
    //! enters infinite loop, this function will wait until kill -9.
    void Stop();
};

//! templatized version of ServerImpl (WIN parameter is class derivated from
//! RootWindow
template < typename WIN >
class Server : public ServerImpl {
protected:

    Scr::Tk::RootWindow * GenWindow(std::istream & in, std::ostream & out) {
        return new WIN(in, out);
    }
};
}}
#endif
