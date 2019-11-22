#include "gtest/gtest.h"
#include <stdarg.h>
/*
class TcpClientTest : public testing::Test, public ITimerEvent, public PduHandler
{
protected:
    virtual void SetUp()
    {
        tcp_handler_ = new DefaultNetTcpEventHandler();
        udp_handler_ = new DefaultNetUdpEventHandler();
        m_pNetEventLoop = CreateNetEventLoop();
        ITimerEvent *pTimerEvent = this;
        m_pNetEventLoop->Start(tcp_handler_, udp_handler_, pTimerEvent);
        m_pTcpNetClient = new TcpNetClient(m_pNetEventLoop, this);
        EXPECT_TRUE(TraceLog::log().openLog("hello-1.log", 0, true));
        return;
    }
    virtual void TearDown()
    {
        ReleaseNetEventLoop(m_pNetEventLoop);
        delete udp_handler_;
        delete tcp_handler_;
        return;
    }
    
    //impl for ITimerEvent
    virtual void OnTimerEvent()
    {
        EXPECT_TRUE(true);
    }
    
    //pdu handler impl
    virtual void OnHandlePdu(uint16_t cmd_type, char* packet, uint16_t packet_len ,
        uint32_t remote_ip, uint16_t remote_port)
    {
        
    }
    virtual void OnSendPdu() {}
    
    INetEventLoop* m_pNetEventLoop;
    DefaultNetUdpEventHandler* udp_handler_;
    DefaultNetTcpEventHandler* tcp_handler_;
    TcpNetClient *m_pTcpNetClient;
};

TEST_F(TcpClientTest, AvailableServer)
{
    if( m_pTcpNetClient == NULL )
    {
        EXPECT_TRUE(false);
    }
    else
    {
        EXPECT_TRUE(true);
    }
    InetAddress nAddr("103.17.41.137", 80);
    EXPECT_TRUE(m_pTcpNetClient->Open(nAddr.ip_, nAddr.port_));
    LogINFO("tcp open and sleep\r\n");
    CBaseThread::Sleep(1000);
    EXPECT_FALSE(m_pTcpNetClient->IsClosed());
    LogINFO("tcp is closed");
    char nBuffer[1024] = "hello world!";
    EXPECT_LT(0, m_pTcpNetClient->SendRawData(nBuffer, strlen(nBuffer)));
    EXPECT_GE(0, m_pTcpNetClient->SendRawData(NULL, 0));
    EXPECT_LT(0, m_pTcpNetClient->SendPduData(88, nBuffer, strlen(nBuffer)));
    LogINFO("tcp send data");
    uint32_t local_ip = 0;
    uint16_t local_port = 0;
    m_pTcpNetClient->GetLocalAddr(local_ip, local_port);
    InetAddress nLocalAddr(local_ip, local_port);
    LogINFO("get local addr: %s", nLocalAddr.AsString().c_str());
    EXPECT_EQ(0, local_ip);
    EXPECT_NE(0, local_port);
    EXPECT_FALSE(m_pTcpNetClient->IsClosed());
    LogINFO("tcp is closed");
    m_pTcpNetClient->Close();
    LogINFO("tcp is close");
    EXPECT_TRUE(m_pTcpNetClient->IsClosed());
    LogINFO("tcp is closed");
}

TEST_F(TcpClientTest, InvalidableServer)
{
    if( m_pTcpNetClient == NULL )
    {
        EXPECT_TRUE(false);
    }
    else
    {
        EXPECT_TRUE(true);
    }
    InetAddress nAddr("103.17.41.1", 80);
    EXPECT_TRUE(m_pTcpNetClient->Open(nAddr.ip_, nAddr.port_));
    LogINFO("tcp open and sleep\r\n");
    CBaseThread::Sleep(1000);
    EXPECT_TRUE(m_pTcpNetClient->IsClosed());
    LogINFO("tcp is closed");
}
*/