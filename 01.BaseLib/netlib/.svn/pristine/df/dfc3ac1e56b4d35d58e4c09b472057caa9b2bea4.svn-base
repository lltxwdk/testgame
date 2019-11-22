#include "test.h"

class tcp_peer : public ITcpEvent
{
public:
    tcp_peer()
    {
        total_datalen_ = 0;
    }
    virtual void OnConnected()
    {
        LogINFO("connect :%d", index_);
    }
    virtual void OnDisconnect(int err_code)
    {
        LogINFO("OnDisconnect index:%d, err:%d", index_, err_code);
    }
    virtual int OnRecvData(char* data, int data_len)
    {
        LogINFO("OnRecvData index:%d, len:%d", index_, data_len);
        total_datalen_ += data_len;
        return data_len;
    }
    ITcpConn* tcpconn_;
    int index_;
    int total_datalen_;
private:
};

class listener : public ITcpListenEvent
{
public:
    listener()
    {
        cur_index_ = 0;
    }
    virtual void OnAccept(ITcpConn* new_tcpconn, char* remote_ip, unsigned short remote_port)
    {
        svr_peers[cur_index_].tcpconn_ = new_tcpconn;
        svr_peers[cur_index_].tcpconn_->Init(&svr_peers[cur_index_], 2000, 2000);
        svr_peers[cur_index_].index_ = cur_index_ + 1000;
        svr_peers[cur_index_].total_datalen_ = 0;
        cur_index_++;
    }
    virtual void OnNetError(int err_code)
    {
        //assert(false);
    }
    ITcpListenner* listentcp_;

    tcp_peer svr_peers[100];
    int cur_index_;
};

#define TEST_PEERS 10
void test_tcp()
{
    listener tcp_lintener;
    ITcpTransLib* tcp_translib = CreateTcpTransLib();
    tcp_translib->Init(10, ASYNC_MODEL_EPOLL);
    tcp_lintener.listentcp_ = tcp_translib->CreateTcpListenner(&tcp_lintener);
    tcp_lintener.listentcp_->Listen("127.0.0.1", 5678);
    LogINFO("testcase1 listen 5678");

    LogINFO("enter to connect");
    getchar();
    tcp_peer peers[TEST_PEERS];
    for (size_t i = 0; i < TEST_PEERS; i++)
    {
        peers[i].tcpconn_ = tcp_translib->CreateTcpConn();
        peers[i].tcpconn_->Init(&peers[i], 2000*1000, 2000);
        peers[i].index_ = i;
        peers[i].tcpconn_->Connect("127.0.0.1", 5678);
    }

    LogINFO("enter to send");
    getchar();
    char buf[1000] = { 0 };
    //for (size_t i = 0; i < 1000; i++)
    {
        for (size_t i = 0; i < TEST_PEERS; i++)
        {
            memset(buf, i, 1000);
            peers[i].tcpconn_->SendData(buf, 100 + i);
#ifdef WIN32
            Sleep(100);
#endif
        }
    }

    LogINFO("print recv");
    getchar();
    for (size_t i = 0; i < TEST_PEERS; i++)
    {
        LogINFO("index:%d,\ttotallen:%d", \
            tcp_lintener.svr_peers[i].index_, tcp_lintener.svr_peers[i].total_datalen_);
    }

    LogINFO("close conn");
    getchar();
    for (size_t i = 0; i < TEST_PEERS; i++)
    {
        peers[i].tcpconn_->Close();
    }

    LogINFO("stop listen");
    getchar();
    tcp_lintener.listentcp_->Close();

    LogINFO("stop translib");
    getchar();
    tcp_translib->Uninit();
}
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
#endif
void test_udp()
{
    LogINFO("begin udp");
    getchar();
    IUdpTransLib* udp_translib = CreateUdpTransLib();
    udp_translib->Init(10, ASYNC_MODEL_EPOLL);

    LogINFO("create recv udp");
    getchar();
    udp_peer udp_recvpeers[TEST_PEERS];
    for (size_t i = 0; i < TEST_PEERS; i++)
    {
        int threadid = -1;
        unsigned short port = 0;
        udp_recvpeers[i].udp_trans_ = udp_translib->CreateUdpSocket(threadid);
        udp_recvpeers[i].udp_trans_->Init(&udp_recvpeers[i], "127.0.0.1", port);
        udp_recvpeers[i].port_ = port;
        udp_recvpeers[i].index_ = i;

        LogINFO("create udp recv index:%d ,port:%d", i, port);
    }

    LogINFO("create send udp");
    getchar();
    udp_peer udpsendpeer;
    int threadid = -1;
    unsigned short port = 0;
    udpsendpeer.udp_trans_ = udp_translib->CreateUdpSocket(threadid);
    udpsendpeer.udp_trans_->Init(&udpsendpeer, NULL, port);

    LogINFO("begin send udp");
    getchar();
    char buf[1000] = { 0 };
    int ip = IPV4StrToInt("127.0.0.1");
    LogINFO("get ip num %X, send port:%u", ip, port);
    for (size_t i = 0; i < TEST_PEERS; i++)
    {
        bool suc = udpsendpeer.udp_trans_->SendData(buf, 100 + i, ip, udp_recvpeers[i].port_);
#ifdef WIN32
        Sleep(100);
#endif
        //LogINFO("send udp data index:%d ,port:%d", i, udp_recvpeers[i].port_);
    }

    LogINFO("close udp");
    getchar();
}

int test_normal(int argc, char* argv[])
{
    LogINFO("Test normal start");
    
    test_tcp();
    test_udp();

    LogINFO("Test normal end\n\n");
	return 0;
}

