#pragma once
#include "TypeDef.h"
#include "BaseThread.h"
#include "EventLoop.h"
class EpollEventLoop : public EventLoop
{
public:
    EpollEventLoop();
    virtual ~EpollEventLoop();
private:
    virtual bool Start();
    virtual void Stop();
    virtual bool AddEventFd(FD_TYPE fd_tpe, EventHandler* handler);
    virtual void DelEventFd(EventHandler* handler);
    virtual void SetWriteEventForFd(EventHandler* handler, bool need);
private:
    static uint32_t ThreadFun(void* arg);
    void MyRun();
private:
    int epoll_fd_;

    CBaseThread thread_;
    volatile bool running_;
};

