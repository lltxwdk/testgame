#include "NetPortable.h"
#include "TcpTransLib.h"
#include "TcpListenner.h"
#include "TcpActiveConn.h"
#include "TcpPassiveConn.h"
TcpTransLib::TcpTransLib()
{
}

TcpTransLib::~TcpTransLib()
{
}

bool TcpTransLib::Init(int thread_num, int mode)
{
    return event_loops_mgr_.Init(thread_num, mode);
}

void TcpTransLib::Uninit()
{
    event_loops_mgr_.Uninit();
}

EventLoop* TcpTransLib::AllocEventLoop()
{
    int thrid = -1;
    return event_loops_mgr_.AllocEventLoop(thrid);
}

ITcpListenner* TcpTransLib::CreateTcpListenner(ITcpListenEvent* ent_handler)
{
    return new TcpListenner(AllocEventLoop(), ent_handler, *this);
}

ITcpConn* TcpTransLib::CreateTcpConn()
{
    TcpActiveConn* tcp_conn = new TcpActiveConn(AllocEventLoop());
    return tcp_conn;
}

void TcpTransLib::CreatNewConn(int sock_fd, char* remote_ip, uint16_t remote_port, ITcpListenEvent* ent_handler)
{
    TcpPassiveConn* tcp_conn = new TcpPassiveConn(AllocEventLoop());
    if (!tcp_conn->AttachSocketFd(sock_fd))
    {
        assert(false);
        delete tcp_conn;
        return;
    }
    assert(ent_handler);
    ent_handler->OnAccept(tcp_conn, remote_ip, remote_port);
}

ITcpTransLib* CreateTcpTransLib()
{
    return new TcpTransLib;
}

void DeleteTcpTransLib(ITcpTransLib* trans_lib)
{
    delete (TcpTransLib*)trans_lib;
}
