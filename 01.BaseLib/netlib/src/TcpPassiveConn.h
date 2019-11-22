#ifndef __TCPPASSIVECONN_H__
#define __TCPPASSIVECONN_H__

#include "TcpConn.h"
class TcpPassiveConn :
    public TcpConn
{
public:
    TcpPassiveConn(EventLoop* ent_loop_ins);
    virtual ~TcpPassiveConn();
    bool AttachSocketFd(int sock_fd);
private://ITcpConn
    virtual bool Init(ITcpEvent* ent_handler, int max_sendbuf_len, int max_recvbuf_len);
    virtual bool Connect(char* remote_ip, unsigned short remote_port);

private://TcpNormalSocket::TcpEventSink
    virtual void OnConnected(){ assert(false); }
};
#endif


