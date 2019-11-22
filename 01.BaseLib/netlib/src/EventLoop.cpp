#include "NetLibCommon.h"
#include "SelectEventLoop.h"
#include "EpollEventLoop.h"

EventLoop* CreateEventLoop(int model)
{
#ifdef WIN32
    model = ASYNC_MODEL_SELECT;
#endif
    if (ASYNC_MODEL_SELECT == model)
    {
        return new SelectEventLoop();
    }
    assert(ASYNC_MODEL_EPOLL == model);
    return new EpollEventLoop();
}

void DeleteEventLoop(EventLoop*& ins)
{
    delete ins;
    ins = NULL;
}
