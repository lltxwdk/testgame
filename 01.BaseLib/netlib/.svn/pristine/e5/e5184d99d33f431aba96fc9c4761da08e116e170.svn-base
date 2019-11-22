#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__
#include "Socket.h"
#include "EventHandler.h"
class UdpSocket :
    public AsyncSocket, public EventHandler
{
public:
    class UdpEventSink
    {
    public:
        virtual void OnRecvData() = 0;
        virtual void OnNetError(int err_code) = 0;
    };
public:
    UdpSocket(UdpEventSink* sink);
    virtual ~UdpSocket();
    bool Init(char* local_ip, uint16_t& local_port);
    bool SendData(char* data, int data_len,
        uint32_t remote_ip, uint16_t remote_port);
    int RecvData(char* data, int data_len,
        uint32_t& remote_ip, uint16_t &remote_port, int& err_code);
    void Close();
private://EventHandler
    virtual int GetEventFd(){ return GetSocketFd(); }
    virtual bool IsNeedWriteEvent(){ return false; }
    virtual void HandleEvent(bool ev_err, bool ev_read, bool ev_write);
private:
    UdpEventSink* event_sink_;
};

#endif
