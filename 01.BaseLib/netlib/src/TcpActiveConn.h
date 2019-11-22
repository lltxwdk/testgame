#ifndef __TCP_ACTIVE_CONN_H__
#define __TCP_ACTIVE_CONN_H__

#include "TcpConn.h"
class TcpActiveConn : public TcpConn
{
public:
    TcpActiveConn(EventLoop* ent_loop_ins);
    virtual ~TcpActiveConn();
private://ITcpConn
    virtual bool Init(ITcpEvent* ent_handler, int max_sendbuf_len, int max_recvbuf_len);
    virtual bool Connect(char* remote_ip, unsigned short remote_port);

private://TcpNormalSocket::TcpEventSink
    virtual void OnConnected();
private:

    bool CreateAsyncSocket();
};
#endif


