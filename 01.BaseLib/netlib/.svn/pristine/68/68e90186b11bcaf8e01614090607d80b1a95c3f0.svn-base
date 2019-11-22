#include "UdpTrans.h"
#include "UdpTransLib.h"


UdpTransLib::UdpTransLib()
{
}


UdpTransLib::~UdpTransLib()
{
}

bool UdpTransLib::Init(int thread_num, int mode)
{
#if defined (DEBUG)
    //TraceLog::GetInstance().EnabledTotalLog(true);
#endif
    TraceLog::GetInstance().EnabledTotalLog(false);
    TraceLog::GetInstance().TotalLog("netlib.log", "Netlib Start...\n");
    return event_loops_mgr_.Init(thread_num, mode);
}

void UdpTransLib::Uninit()
{
    event_loops_mgr_.Uninit();
}

IUdpTrans* UdpTransLib::CreateUdpSocket(int& thread_id)
{
    EventLoop* ev_loop = event_loops_mgr_.AllocEventLoop(thread_id);
    if (NULL == ev_loop)
    {
        return NULL;
    }
    return new UdpTrans(ev_loop);
}

void UdpTransLib::DeleteUdpSocket(IUdpTrans* udp_soc)
{
    delete udp_soc;
}

IUdpTransLib* CreateUdpTransLib()
{
    return new UdpTransLib();
}

void DeleteUdpTransLib(IUdpTransLib* trans_lib)
{
    delete (UdpTransLib*)trans_lib;
}


