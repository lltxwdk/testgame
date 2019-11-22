#include "UdpSocket.h"

UdpSocket::UdpSocket(UdpEventSink* sink)
    :event_sink_(sink)
{
}

UdpSocket::~UdpSocket()
{
}

bool UdpSocket::Init(char* local_ip, uint16_t& local_port)
{
    if (!CreateAsyncSocket(false))
    {
        return false;
    }

    
    TraceLog::GetInstance().TotalLog("netlib.log", "Create socket:%d\n ", GetSocketFd());
    return Bind(local_ip, local_port);
}

bool UdpSocket::SendData(char* data, int data_len,
    uint32_t remote_ip, uint16_t remote_port)
{
    struct sockaddr_in to;
    to.sin_family = AF_INET; 
    to.sin_addr.s_addr = remote_ip;
    to.sin_port = htons(remote_port);
    if (0 > ::sendto(GetSocketFd(), data, data_len, 0, (struct sockaddr*)&to, sizeof(to)))
    {
        /*int err_code = GetErrCode();
        char ip_str[32] = { 0 };
        ConvrtIPV4ToStr(remote_ip, ip_str);
        TraceLog::GetInstance().TotalLog("netlib.log", "Socket sendto data error. socket:%d, error code:%d, err desc:%s, ip:%s, port:%d, data len:%d \n",
            GetSocketFd(), err_code, strerror(err_code), ip_str, remote_port, data_len);*/
        return false;
    }
    return true;
}

int UdpSocket::RecvData(char* data, int data_len,
    uint32_t& remote_ip, uint16_t &remote_port, int& err_code)
{
    struct sockaddr_in from;
    socklen_t len = sizeof(from);
    int ret = ::recvfrom(GetSocketFd(), data, data_len, 0, (struct sockaddr*)&from, &len);
    if (ret <= 0)
    {
        err_code = GetErrCode();
        if (0 == ret)
        {
            remote_ip = from.sin_addr.s_addr;
            remote_port = ntohs(from.sin_port);
            char ip_str[32] = { 0 };
            ConvrtIPV4ToStr(remote_ip, ip_str);
            LogERR("Recv data error. socket:%d, result:%d, error code:%d, err desc:%s, ip:%s, port:%d",
                GetSocketFd(), ret, err_code, strerror(err_code), ip_str, remote_port);
        }

        if (IsErrorEINTR(err_code))
        {
            //assert(false);
            LogERR("udp socket EINTR, fd:%d, errno:%d, err desc:%s",
                GetSocketFd(), err_code, strerror(err_code));
            return 0;
        }
        if (IsErrorEAGAIN(err_code))
        {
            //assert(false);
            //LogERR("udp socket EAGIN, fd:%d, errno:%d, err desc:%s", 
            //    GetSocketFd(), err, strerror(err));
            return 0;
        }
        
        if (IsErrorERESET(err_code))
        {
            //assert(false);
            return 0;
        }
        return -1;
    }

    remote_ip = from.sin_addr.s_addr;
    remote_port = ntohs(from.sin_port);
    
    return ret;
}

void UdpSocket::Close()
{
    TraceLog::GetInstance().TotalLog("netlib.log", "Close socket:%d\n ", GetSocketFd());
    CloseSocket();
}

//void UdpSocket::HandleReadEvent()
//{
//    event_sink_->OnRecvData();
//}
//
//void UdpSocket::HandleErrorEvent()
//{
//    int err = GetErrCode();
//    if (IsErrorERESET(err))
//    {
//        return;
//    }
//    event_sink_->OnNetError(err);
//}
void UdpSocket::HandleEvent(bool ev_err, bool ev_read, bool ev_write)
{
    assert(!ev_write);
    if (ev_read)
    {
        event_sink_->OnRecvData();
    }
    if (ev_err)
    {
        int err = GetErrCode();
        if (IsErrorERESET(err))
        {
			LogERR("udp socket RESET, fd:%d, errno:%d, err desc:%s",
				GetSocketFd(), err, strerror(err));
            return;
        }
		LogERR("udp socket , fd:%d, err:%d, err desc:%s",
			GetSocketFd(), err, strerror(err));
        event_sink_->OnNetError(err);
    }
}