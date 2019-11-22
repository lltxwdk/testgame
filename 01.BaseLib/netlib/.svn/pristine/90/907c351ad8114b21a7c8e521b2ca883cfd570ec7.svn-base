#include "TcpConn.h"


TcpConn::TcpConn(EventLoop* ent_loop_ins)
 : ent_loop_ins_(ent_loop_ins), 
   tcp_sock_(this),
   tcp_event_(NULL)
{
}

TcpConn::~TcpConn()
{
}

void TcpConn::InitConn(ITcpEvent* ent_handler, int max_sendbuf_len, int max_recvbuf_len)
{
    assert(NULL == tcp_event_);
    tcp_event_ = ent_handler;
    tcp_sock_.Init(max_sendbuf_len, max_recvbuf_len);
}

int TcpConn::SendData(char* data, int data_len)
{
    return tcp_sock_.SendData(data, data_len);
}

void TcpConn::Close()
{
	LogINFO("tcp close,fd:%d, %p\n" ,tcp_sock_.GetSocketFd(), this);
    if (tcp_sock_.IsValidSock())
    {
        ent_loop_ins_->DelEventFd(&tcp_sock_);
        tcp_sock_.Close();
    }
}

void TcpConn::Release()
{
    delete this;
}

void TcpConn::OnNetError(int err_code)
{
	LogINFO("on net err, %p, fd:%d, err:%d,desc:%s\n", this,tcp_sock_.GetSocketFd(), err_code, strerror(err_code));
    ent_loop_ins_->DelEventFd(&tcp_sock_);
    tcp_sock_.CloseSocket();
    if (tcp_event_)
    {
        tcp_event_->OnDisconnect(err_code);
    }
}

uint32_t TcpConn::OnRecvData(char* data, int data_len)
{
    if (tcp_event_)
    {
        return tcp_event_->OnRecvData(data, data_len);
    }
    return 0;
}

void TcpConn::OnSendBlocked()
{
    ent_loop_ins_->SetWriteEventForFd(&tcp_sock_, true);
}

void TcpConn::OnSendOver()
{
    ent_loop_ins_->SetWriteEventForFd(&tcp_sock_, false);
}

