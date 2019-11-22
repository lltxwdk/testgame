#include "TcpPassiveConn.h"


TcpPassiveConn::TcpPassiveConn(EventLoop* ent_loop_ins)
 : TcpConn(ent_loop_ins)
{
}

TcpPassiveConn::~TcpPassiveConn()
{
}

bool TcpPassiveConn::AttachSocketFd(int sock_fd)
{
    return tcp_sock_.AttachAsyncSocket(sock_fd);
}

bool TcpPassiveConn::Init(ITcpEvent* ent_handler, int max_sendbuf_len, int max_recvbuf_len)
{
    assert(NULL == tcp_event_);
    InitConn(ent_handler, max_sendbuf_len, max_recvbuf_len);
    assert(tcp_sock_.IsValidSock());
    return ent_loop_ins_->AddEventFd(FD_TCP_NORMAL, &tcp_sock_);
}

bool TcpPassiveConn::Connect(char* remote_ip, unsigned short remote_port)
{
    assert(false);
    return false;
}
