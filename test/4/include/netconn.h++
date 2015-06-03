#ifndef __NETCONN_H__
#define __NETCONN_H__

typedef void (*ConnectionFunc)(std::istream & in, std::ostream & out) ;

class Server
{
private:
    bool active;
public:
    Server();
    void Start(int portnum, ConnectionFunc _f);
    void Stop();
};
#endif
