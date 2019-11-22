#pragma once
#include "NetPortable.h"
#include "TraceLog.h"
class AsyncSocket
{
public:
    AsyncSocket();
    ~AsyncSocket();
    bool CreateAsyncSocket(bool is_tcp);
    int GetSocketFd(){ return sock_fd_; }
    void CloseSocket();
    bool Bind(const char* local_ip, uint16_t &local_port);
    bool SetNonblocking(int fd);
    bool ReuseAddr();
    int GetSockError();
    bool IsValidSock() { return INVALID_SOCKET != sock_fd_; }
protected:
    bool AttachAsyncSocket(int sock_fd);
    int sock_fd_;
};

