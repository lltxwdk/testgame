#ifndef __UDP_TRANS_H__
#define __UDP_TRANS_H__
#include "UdpNetInterface.h"
#include "UdpSocket.h"
#include "EventLoop.h"
class UdpTrans :
    public IUdpTrans,
    public UdpSocket::UdpEventSink
{
public:
    UdpTrans(EventLoop* ev_loop);
    virtual ~UdpTrans();
private://IUdpTrans
    virtual bool Init(IUdpEvent* ev_handler, char* local_ip, unsigned short& local_port);
    virtual bool SendData(char* data, int data_len,
        unsigned int remote_ip, unsigned short remote_port);
    virtual void Close();
    virtual void Release();

private://UdpSocket::UdpEventSink
    virtual void OnRecvData();
    virtual void OnNetError(int err_code);
private:
    UdpSocket udp_sock_;
    IUdpEvent* ev_handler_;
    EventLoop* ev_loop_;
};

#endif

