#include "TcpListenSocket.h"


TcpListenSocket::TcpListenSocket(ListennerSink* listenner_sink)
{
    listenner_sink_ = listenner_sink;
}


TcpListenSocket::~TcpListenSocket()
{
}

bool TcpListenSocket::Listen(const char* listen_ip, uint16_t listen_port)
{
    if (IsValidSock())
    {
        return false;
    }
    if (!CreateAsyncSocket(true))
    {
        return false;
    }
    if (!ReuseAddr())
    {
        CloseSocket();
        return false;
    }
    if (!Bind(listen_ip, listen_port))
    {
        CloseSocket();
        return false;
    }

    if (::listen(GetSocketFd(), SOMAXCONN) < 0)
    {
        int err = GetSockError();
        CloseSocket();
        return false;
    }
    return true;
}
void TcpListenSocket::Close()
{
    CloseSocket();
}
//
//void TcpListenSocket::HandleReadEvent()
//{
//    struct sockaddr_in client_addr;
//    socklen_t size = sizeof(struct sockaddr_in);
//    int sock_client = ::accept(GetSocketFd(), (struct sockaddr*)(&client_addr), &size);
//    if (sock_client < 0)
//    {
//        int err_code = GetErrCode();
//        if (IsErrorEINTR(err_code))
//        {
//            return;
//        }
//        if (IsErrorEAGAIN(err_code))
//        {
//            return;
//        }
//        LogERR("accept error! reason is", strerror(errno));
//        if (listenner_sink_)
//        {
//            listenner_sink_->OnNetError(err_code);
//        }
//        return;
//    }
//    if (listenner_sink_)
//    {
//        IPV42STR(client_addr.sin_addr, ip_str);
//        listenner_sink_->OnAccept(sock_client, ip_str, ntohs(client_addr.sin_port));
//    }
//}
//
//void TcpListenSocket::HandleErrorEvent()
//{
//    int err_code = GetErrCode(); 
//    if (listenner_sink_)
//    {
//        listenner_sink_->OnNetError(err_code);
//    }
//}

void TcpListenSocket::HandleEvent(bool ev_err, bool ev_read, bool ev_write)
{
    if (ev_err)
    {
        int err_code = GetErrCode();
        if (listenner_sink_)
        {
            listenner_sink_->OnNetError(err_code);
        }
        return;
    }
    if (!ev_read)
    {
        return;
    }
    struct sockaddr_in client_addr;
    socklen_t size = sizeof(struct sockaddr_in);
    int sock_client = ::accept(GetSocketFd(), (struct sockaddr*)(&client_addr), &size);
    if (sock_client < 0)
    {
        int err_code = GetErrCode();
        if (IsErrorEINTR(err_code))
        {
            return;
        }
        if (IsErrorEAGAIN(err_code))
        {
            return;
        }
        LogERR("accept error! reason is:%s", strerror(err_code));
        if (listenner_sink_)
        {
            listenner_sink_->OnNetError(err_code);
        }
        return;
    }
    if (listenner_sink_)
    {
        IPV42STR(client_addr.sin_addr, ip_str);
        listenner_sink_->OnAccept(sock_client, ip_str, ntohs(client_addr.sin_port));
    }
}
