#pragma once
#include <vector>
#include "TcpListenner.h"
#include "EventLoopMgr.h"
class TcpTransLib :
    public ITcpTransLib, 
    public TcpListenner::TcpConnCreator
{
public:
    TcpTransLib();
    virtual ~TcpTransLib();

private://ITcpTransLib
    virtual bool Init(int thread_num, int mode);
    virtual void Uninit();
    virtual ITcpListenner* CreateTcpListenner(ITcpListenEvent* ent_handler);
    virtual ITcpConn* CreateTcpConn();

private://TcpListenner::TcpConnCreator
    virtual void CreatNewConn(int sock_fd, char* remote_ip, 
        uint16_t remote_port, ITcpListenEvent* ent_handler);
private:
    EventLoop* AllocEventLoop();
private:
    EventLoopMgr event_loops_mgr_;
};

