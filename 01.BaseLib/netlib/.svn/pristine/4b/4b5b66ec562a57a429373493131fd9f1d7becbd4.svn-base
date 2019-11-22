#include "test.h"
#include "BaseThread.h"
#include "Utils.h"

char echo_server_addr[50] = "0.0.0.0";
unsigned short echo_server_port = 8089;

#if 0
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
        unsigned short port;*/
        //int len = udp_trans_->RecvData(buf, 1000, ip, port);
        //LogINFO("udp recv index:%d ,port:%d, rport:%d, len:%d", index_, port_, port, len);
    }
    virtual void OnNetError(int err_code)
    {

    }

    virtual RawPacket* GetMem(){ return &raw_packet_; };

    virtual void ReleaseMem(RawPacket* packet){}

    IUdpTrans* udp_trans_;
    unsigned short port_;
    int index_;
    RawPacket raw_packet_;
};
#endif
class udp_server : public udp_peer
{
public:
    udp_server(IUdpTransLib* udp_translib);

    void Start();

    virtual void OnRecvData(RawPacket* pack, unsigned int remote_ip, unsigned short remote_port);
    int recv_count_;
    uint64_t total_data_len_;

    static uint32_t ThreadFun(void* arg);
    void MyRun();
    CBaseThread thread_;
    IUdpTransLib* udp_translib_;
};

udp_server::udp_server(IUdpTransLib* udp_translib)
{
    recv_count_ = 0;
    total_data_len_ = 0;
    udp_translib_ = udp_translib;
}

void udp_server::OnRecvData(RawPacket* pack, unsigned int remote_ip, unsigned short remote_port)
{
    //char buf[1000] = { 0 };
    //unsigned int ip;
    //unsigned short port;
    //int len = udp_trans_->RecvData(buf, 1000, ip, port);
    if (pack->raw_packet_len_ > 0)
    {
        udp_trans_->SendData(pack->raw_packet_, pack->raw_packet_len_, remote_ip, remote_port);
        recv_count_++;
        total_data_len_ += pack->raw_packet_len_;
        //LogERR("udp server recv data count:%d", recv_count_);
    }
    else
    {
        LogERR("udp server recv data len:%d", pack->raw_packet_len_);
    }
}

uint32_t udp_server::ThreadFun(void* arg)
{
    udp_server* server = (udp_server*)arg;
    server->MyRun();

    return 0;
}

void udp_server::MyRun()
{
    LogINFO("echo server set up");

    int threadid = -1;
    udp_trans_ = udp_translib_->CreateUdpSocket(threadid);
    udp_trans_->Init(this, "0.0.0.0", echo_server_port);
    port_ = echo_server_port;
    index_ = -1;

    while (true)
    {
        Utils::Sleep(10 * 1000);
        LogINFO("Udp server recv:%d, total len:%d", recv_count_, total_data_len_);
    }
}

void udp_server::Start()
{
    unsigned int thd = 0;
    thread_.BeginThread(ThreadFun, this, thd);
}

void test_udp_echoserver_send(IUdpTransLib* udp_translib)
{
    udp_server *p = new udp_server(udp_translib);
    p->Start();
}


