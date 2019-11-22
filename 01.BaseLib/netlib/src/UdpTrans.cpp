#include "UdpTrans.h"


UdpTrans::UdpTrans(EventLoop* ev_loop)
    :udp_sock_(this), ev_loop_(ev_loop)
{
}


UdpTrans::~UdpTrans()
{
}

bool UdpTrans::Init(IUdpEvent* ev_handler, char* local_ip, unsigned short& local_port)
{
    if (!udp_sock_.Init(local_ip, local_port))
    {
        return false;
    }

    if (!ev_loop_->AddEventFd(FD_UDP, &udp_sock_))
    {
        udp_sock_.Close();
        return false;
    }
    ev_handler_ = ev_handler;
    return true;
}

bool UdpTrans::SendData(char* data, int data_len,
    unsigned int remote_ip, unsigned short remote_port)
{
    return udp_sock_.SendData(data, data_len, remote_ip, remote_port);
}

void UdpTrans::Close()
{
    ev_loop_->DelEventFd(&udp_sock_);
    udp_sock_.Close();
}
void UdpTrans::Release()
{
    delete this;
}
void UdpTrans::OnRecvData()
{
    RawPacket* pack = ev_handler_->GetMem();
    unsigned int remote_ip = 0;
    unsigned short remote_port = 0;
    int err_code = 0;
    int result = udp_sock_.RecvData(pack->raw_packet_, sizeof(pack->raw_packet_), remote_ip, remote_port, err_code);
    if (0 > result)
    {
        ev_handler_->ReleaseMem(pack);
        ev_loop_->DelEventFd(&udp_sock_);
        ev_handler_->OnNetError(err_code);
        return;
    }
    if (0 == result)
    {
        ev_handler_->ReleaseMem(pack);
        return;
    }
    pack->raw_packet_len_ = result;
    ev_handler_->OnRecvData(pack, remote_ip, remote_port);
}

void UdpTrans::OnNetError(int err_code)
{
    ev_loop_->DelEventFd(&udp_sock_);
    ev_handler_->OnNetError(err_code);
}
