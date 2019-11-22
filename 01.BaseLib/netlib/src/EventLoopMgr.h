#ifndef __EVENT_LOOP_MGR_H__
#define __EVENT_LOOP_MGR_H__
#include <vector>
#include "EventLoop.h"
class EventLoopMgr
{
public:
    EventLoopMgr();
    ~EventLoopMgr();
public:
    bool Init(int thread_num, int model);
    EventLoop* AllocEventLoop(int& thread_id);
    void Uninit();
private:
    std::vector<EventLoop*> event_loops_;
};
#endif


