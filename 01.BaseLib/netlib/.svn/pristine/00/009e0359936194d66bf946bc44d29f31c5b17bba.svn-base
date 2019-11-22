#ifndef __UDPTRANSLIB_H__
#define __UDPTRANSLIB_H__
#include <vector>
#include "UdpNetInterface.h"
#include "EventLoopMgr.h"
class UdpTransLib :
    public IUdpTransLib
{
public:
    UdpTransLib();
    virtual ~UdpTransLib();
private://IUdpTransLib
    virtual bool Init(int thread_num, int mode);
    virtual void Uninit();
    virtual IUdpTrans* CreateUdpSocket(int& thread_id);
    virtual void DeleteUdpSocket(IUdpTrans* udp_soc);
private:
    EventLoopMgr event_loops_mgr_;
};

#endif


