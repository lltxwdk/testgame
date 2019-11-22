#include "TcpActiveConn.h"

TcpActiveConn::TcpActiveConn(EventLoop* ent_loop_ins)
    : TcpConn(ent_loop_ins)
{
}

TcpActiveConn::~TcpActiveConn()
{
}

bool TcpActiveConn::CreateAsyncSocket()
{
    if (tcp_sock_.IsValidSock())
    {
        //assert(false);
        return false;
    }
    if (!tcp_sock_.CreateAsyncSocket(true))
    {
        return false;
    }

    //if (!ent_loop_ins_->AddEventFd(FD_TCP_NORMAL, &tcp_sock_))
    //{
    //    tcp_sock_.CloseSocket();
    //    return false;
    //}
    return true;
}

bool TcpActiveConn::Init(ITcpEvent* ent_handler, int max_sendbuf_len, int max_recvbuf_len)
{
    assert(NULL == tcp_event_);
    InitConn(ent_handler, max_sendbuf_len, max_recvbuf_len);
    assert(!tcp_sock_.IsValidSock());
    return true;
}

bool TcpActiveConn::Connect(char* remote_ip, unsigned short remote_port)
{

    if (!CreateAsyncSocket())
    {
        return false;
    }
    LogINFO("connecting, ip:%s, port:%d,fd:%d, obj:%p\n", remote_ip, remote_port, tcp_sock_.GetSocketFd(), this);
    //ent_loop_ins_->SetWriteEventForFd(&tcp_sock_, true);

    if (!tcp_sock_.Connect(remote_ip, remote_port))
    {
        //ent_loop_ins_->DelEventFd(&tcp_sock_);
        tcp_sock_.CloseSocket();
        return false;
    }
	if (!ent_loop_ins_->AddEventFd(FD_TCP_NORMAL, &tcp_sock_))
	{
		tcp_sock_.CloseSocket();
		return false;
	}
	ent_loop_ins_->SetWriteEventForFd(&tcp_sock_, true);

    return true;
}

void TcpActiveConn::OnConnected()
{
	LogINFO("connected, fd:%d, obj:%p\n",tcp_sock_.GetSocketFd(), this);
    if (tcp_event_)
    {
        tcp_event_->OnConnected();
    }
}




