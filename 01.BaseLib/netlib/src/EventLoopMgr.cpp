#include "NetPortable.h"
#include "EventLoopMgr.h"

EventLoopMgr::EventLoopMgr()
{
}


EventLoopMgr::~EventLoopMgr()
{
}

bool EventLoopMgr::Init(int thread_num, int model)
{
    assert(thread_num > 0 && thread_num < 1000);
#ifdef _WIN32
    WSAData wsaData;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        return false;
    }
#endif
    EventLoop* loop_ins = NULL;
    for (int i = 0; i < thread_num; i++)
    {
        loop_ins = CreateEventLoop(model);
        if (!loop_ins->Start())
        {
            assert(false);
        }
        event_loops_.push_back(loop_ins);
    }
    return true;
}

EventLoop* EventLoopMgr::AllocEventLoop(int& thread_id)
{
    assert(!event_loops_.empty());
    if (thread_id < 0)
    {
        uint32_t rand_num = rand();
        size_t index = rand_num % event_loops_.size();
        thread_id = index;
        return event_loops_[index];
    }
    if ((uint32_t)thread_id >= event_loops_.size())
    {
        assert(false);
        return NULL;
    }
    return event_loops_[thread_id];
}

void EventLoopMgr::Uninit()
{
    for (size_t i = 0; i < event_loops_.size(); i++)
    {
        event_loops_[i]->Stop();
        DeleteEventLoop(event_loops_[i]);
    }
    event_loops_.clear();
#ifdef _WIN32
    WSACleanup();
#endif
}
