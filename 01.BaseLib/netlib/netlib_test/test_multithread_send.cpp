#include "test.h"
#include "BaseThread.h"
#include "Utils.h"

char server_addr[50] = "10.10.6.109";
unsigned short server_port = 8089;
int s_data_len = 500;
int s_max_count = 1 * 1000;
#define s_max_client 1
#define s_interval 10

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
    }
    virtual void OnNetError(int err_code)
    {

    }

    virtual RawPacket* GetMem() { return &raw_packet_; }

    //内存未使用，交回至上层处理
    virtual void ReleaseMem(RawPacket* packet) {}

    IUdpTrans* udp_trans_;
    unsigned short port_;
    int index_;
    RawPacket raw_packet_;
};
#endif

class udp_client : public udp_peer
{
public:
    udp_client();
    ~udp_client();
    void Start();
    bool IsSendOver();
    static uint32_t ThreadFun(void* arg);
    void MyRun();
    int send_max_count_;
    int recv_count_;

    virtual void OnRecvData(RawPacket* pack, unsigned int remote_ip, unsigned short remote_port);

private:
    int send_count_;
    CBaseThread thread_;
};

udp_client::udp_client()
{
    send_count_ = 0;
    recv_count_ = 0;
}

udp_client::~udp_client()
{

}

void udp_client::Start()
{
    unsigned int thd = 0;
    thread_.BeginThread(ThreadFun, this, thd);
}

bool udp_client::IsSendOver()
{
    if (send_count_ >= send_max_count_)
    {
        return true;
    }

    return false;
}

uint32_t udp_client::ThreadFun(void* arg)
{
    udp_client* client = (udp_client*)arg;
    client->MyRun();

    return 0;
}

void udp_client::OnRecvData(RawPacket* pack, unsigned int remote_ip, unsigned short remote_port)
{
    recv_count_++;
}

void udp_client::MyRun()
{
    while (true)
    {
        Utils::Sleep(s_interval);
        send_count_++;

        char buf[1000] = { 0 };
        int ip = IPV4StrToInt(server_addr);
        udp_trans_->SendData(buf, s_data_len, ip, server_port);
        if (send_count_ >= send_max_count_ )
        {
            break;
        }
    }

    LogINFO("Client send ok, index:%d, count :%d", index_, send_count_);
}

void test_multithread_send(IUdpTransLib* udp_translib)
{
    LogINFO("create client udp");
    
    udp_client udp_client_array[s_max_client];
    for (size_t i = 0; i < s_max_client; i++)
    {
        int threadid = -1;
        unsigned short port = 0;
        udp_client_array[i].udp_trans_ = udp_translib->CreateUdpSocket(threadid);
        udp_client_array[i].udp_trans_->Init(&udp_client_array[i], "0.0.0.0", port);
        udp_client_array[i].port_ = port;
        udp_client_array[i].index_ = i;
        udp_client_array[i].send_max_count_ = s_max_count;

        LogINFO("create udp send index:%d ,port:%d", i, port);
    }
    LogINFO("enter to send");

    for (size_t i = 0; i < s_max_client; i++)
    {
        udp_client_array[i].Start();
    }

    while (true)
    {
        bool send_over = true;
        LogINFO("Check send end..");
        Utils::Sleep(1000);
        for (size_t i = 0; i < s_max_client; i++)
        {
            if (!udp_client_array[i].IsSendOver())
            {
                send_over = false;
                continue;
            }

            LogINFO("Udp client recv data count:%d", udp_client_array[i].recv_count_);
        }

        if (send_over)
        {
            LogINFO("Udp send ok..");
            Utils::Sleep(10000);
            break;
        }
    }

    LogINFO("close udp");
    getchar();

    for (size_t i = 0; i < s_max_client; i++)
    {
        udp_client_array[i].udp_trans_->Close();
    }
    getchar();

}


