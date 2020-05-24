#pragma once
#include "TypeDef.h"
#include "TcpNetInterface.h"
#include "SRProtoParser.h"
class SRStreamHandler
{
public:
    virtual void OnRecvSRPack(uint16_t cmd_type, char* raw_packet, uint16_t raw_packet_len) = 0;
	virtual void OnConnected() = 0;
    virtual void OnClosed() = 0;
};

class SRProtoConnBase : public ITcpEvent
{
public:
    SRProtoConnBase(ITcpConn* tcp_conn, SRStreamHandler* srpack_handler);
    ~SRProtoConnBase();
public:
    bool Init(uint32_t max_sendbuf_len, uint32_t max_recvbuf_len);
    bool Connect(const char* remote_ip, unsigned short remote_port);
    void Close();
    bool IsClosed();
    bool SendPduData(uint16_t cmd_id, char* data, uint32_t data_len);
    bool SendSRPack(char* pack, uint32_t pack_len);
    uint64_t GetSocketClosedTime();
private:
    //ITcpEvent interface impl
    virtual void OnConnected();
    virtual void OnDisconnect(int err_code);
    virtual int OnRecvData(char* data, int data_len);
private:
    ITcpConn* tcp_conn_;
    SRStreamHandler* srpack_handler_;
    volatile uint64_t last_disconnect_time_;
};

