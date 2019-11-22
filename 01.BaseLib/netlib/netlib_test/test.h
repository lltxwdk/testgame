#pragma once

#include <vector>
#include "TraceLog.h"
//#include "NetCommon.h"
#include "TcpNetInterface.h"
#include "UdpNetInterface.h"

int test_normal(int argc, char* argv[]);
void test_multithread_send(IUdpTransLib* udp_translib);
void test_udp_echoserver_send(IUdpTransLib* udp_translib);

class udp_peer : public IUdpEvent
{
public:
    udp_peer()
    {

    }
    virtual void OnRecvData(RawPacket* pack, unsigned int remote_ip, unsigned short remote_port)
    {
        /*char buf[1000] = { 0 };
        unsigned int ip;
        unsigned short port;
        int len = udp_trans_->RecvData(buf, 1000, ip, port);
        LogINFO("udp recv index:%d ,port:%d, rport:%d, len:%d", index_, port_, port, len);*/
        char src_ip[32] = { 0 };
        IPV4IntToStr(remote_ip, src_ip);
        LogINFO("OnRecvData : local port:%u, src ip:%X(%s), port:%u, packet len %u", port_, remote_ip, src_ip, remote_port, pack->raw_packet_len_);
    }
    virtual void OnNetError(int err_code)
    {

    }

    virtual RawPacket* GetMem() { return &raw_packet_; }

    virtual void ReleaseMem(RawPacket* packet) {}

    IUdpTrans* udp_trans_;
    unsigned short port_;
    int index_;

    RawPacket raw_packet_;
};

//#endif
