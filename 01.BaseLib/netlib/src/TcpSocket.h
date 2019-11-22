#pragma once
#include "Socket.h"
#include "EventHandler.h"

class TcpNormalSocket : public AsyncSocket, public EventHandler
{
    typedef enum emtcp_stat
    {
        tcp_connecting,
        tcp_connected,
        tcp_disconnect
    } TcpStat;
public:    

    class TcpEventSink
    {
    public:
        virtual void OnConnected() = 0;
        virtual void OnNetError(int err_code) = 0;
        virtual uint32_t OnRecvData(char* data, int data_len) = 0;
        virtual void OnSendBlocked() = 0;
        virtual void OnSendOver() = 0;
    };
    TcpNormalSocket(TcpEventSink* sink);
    ~TcpNormalSocket();

    bool AttachAsyncSocket(int sock_fd);
public:
    void Init(uint32_t max_sendbuf_len, uint32_t max_recvbuf_len);
    bool Connect(char* remote_ip, uint16_t remote_port);
    int SendData(char* data, uint32_t data_len);
    void Close()
    {
        CloseSocket();
        tcp_stat_ = tcp_disconnect;
    }
private:
    virtual int GetEventFd();
    virtual bool IsNeedWriteEvent();
    virtual void HandleEvent(bool ev_err, bool ev_read, bool ev_write);
private:
    void Reset()
    {
        cur_recv_pos_ = 0;
        memset(recv_buf_, 0, max_recvbuf_len_);

        CCriticalAutoLock guard(send_buf_lock_);
        cur_send_pos_ = 0;
        memset(send_buf_, 0, max_sendbuf_len_);
    }

    int RecvData();
    int FlushSendBuf(int& err_code);
    void ErrorNotify(int err_code, const std::string &desc)
    {
        tcp_stat_ = tcp_disconnect;
        if (tcp_event_handler_)
        {
            tcp_event_handler_->OnNetError(err_code);
        }
		LogINFO("net err, code:%d, desc:%s, ehandler:%p, this:%p", err_code, desc.c_str(), tcp_event_handler_, this);
    }
private:
    volatile TcpStat tcp_stat_;

    char* recv_buf_;// [TCP_MAX_BUF_LEN];
    volatile uint32_t cur_recv_pos_;

    CCriticalSection send_buf_lock_;
    char* send_buf_;// [TCP_MAX_BUF_LEN];
    uint32_t cur_send_pos_;

    uint32_t max_sendbuf_len_;
    uint32_t max_recvbuf_len_;

    TcpEventSink* tcp_event_handler_;
};

