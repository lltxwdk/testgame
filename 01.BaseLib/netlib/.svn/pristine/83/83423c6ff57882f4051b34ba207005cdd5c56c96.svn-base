#ifndef __TCPCONN_H__
#define __TCPCONN_H__

#include "TcpNetInterface.h"
#include "EventLoop.h"
#include "TcpSocket.h"
class TcpConn :
    public ITcpConn,
    public TcpNormalSocket::TcpEventSink
{
public:
    TcpConn(EventLoop* ent_loop_ins);
    virtual ~TcpConn();
private://ITcpConn
    virtual bool Init(ITcpEvent* ent_handler, int max_sendbuf_len, int max_recvbuf_len) = 0;
    virtual bool Connect(char* remote_ip, unsigned short remote_port)= 0;
    virtual int SendData(char* data, int data_len);
    virtual void Close();
    virtual void Release();
protected:
    void InitConn(ITcpEvent* ent_handler, int max_sendbuf_len, int max_recvbuf_len);
private://TcpNormalSocket::TcpEventSink
    virtual void OnConnected() = 0;
    virtual void OnNetError(int err_code);
    virtual uint32_t OnRecvData(char* data, int data_len);
    virtual void OnSendBlocked();
    virtual void OnSendOver();
protected:
    EventLoop* ent_loop_ins_;
    TcpNormalSocket tcp_sock_;
    ITcpEvent* tcp_event_;
};
#endif


