#include "Socket.h"
#include "TraceLog.h"

AsyncSocket::AsyncSocket()
	:sock_fd_(INVALID_SOCKET)
{
}

AsyncSocket::~AsyncSocket()
{
    
}

#define SOCKET_RCVBUF_LEN (1024*1024)
#define SOCKET_SNDBUF_LEN (1024*1024)

#if 0 //def LINUX
// 检查系统SOCKET缓存设置, 如不满足则修改
void CheckAndSetSystemSocketBufferLen(bool rcv, int my_buf_len)
{
    FILE* fp = NULL;
    if (rcv)
    {
        fp = fopen("/proc/sys/net/core/rmem_max", "r+");
    }
    else
    {
        fp = fopen("/proc/sys/net/core/wmem_max", "r+");
    }
    if (fp)
    {
        unsigned int buf_len = 0;
        fscanf(fp, "%u", &buf_len);
        printf("get %s mem_max %u, my buf len is %u\n", rcv ? "rcv" : "snd", buf_len, my_buf_len);
        fseek(fp, 0L, SEEK_SET);
        if (buf_len < my_buf_len)
        {
            fprintf(fp, "%u", my_buf_len);
            fseek(fp, 0L, SEEK_SET);
            fscanf(fp, "%u", &buf_len);
            printf("after set, get %s mem_max %u\n", rcv ? "rcv" : "snd", buf_len);
        }
        fclose(fp);
    }
    else
    {
        printf("open file fail!\n");
    }
}
#endif

bool AsyncSocket::CreateAsyncSocket(bool is_tcp)
{
    int typ = is_tcp ? SOCK_STREAM : SOCK_DGRAM;
    int fd = socket(AF_INET, typ, 0);
    if (fd < 0)
    {
        LogERR("Create socket fail! error code:%u, discrbie:%s", errno, strerror(errno));
        return false;
    }
#if 0 //def LINUX
    CheckAndSetSystemSocketBufferLen(true, SOCKET_RCVBUF_LEN);
    CheckAndSetSystemSocketBufferLen(false, SOCKET_SNDBUF_LEN);
#endif
    int buf_size = SOCKET_RCVBUF_LEN; //500 * 1000;//500KB
    if (0 != setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char *)&buf_size, sizeof(buf_size)))
    {
        TraceLog::GetInstance().TotalLog("netlib.log", "Socket:%d, setsockopt SO_RCVBUF error!, %d\n ",
            fd, GetErrCode());
    }
    buf_size = SOCKET_SNDBUF_LEN;
    if (0 != setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char *)&buf_size, sizeof(buf_size)))
    {
        TraceLog::GetInstance().TotalLog("netlib.log", "Socket:%d, setsockopt SO_RCVBUF error!, %d\n ",
            fd, GetErrCode());
    }
#if 1
	int optVal = 0;
	int optLen = sizeof(optVal);
	int ret = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&optVal, (socklen_t*)&optLen);
	if (ret != 0)
	{
		TraceLog::GetInstance().TotalLog("netlib.log", "Socket:%d, getsockopt SO_RCVBUF error!, %d\n ",
			fd, GetErrCode());
	}
	else
	{
		printf("get socket rcvbuf len %d\n", optVal);
	}
#endif

    if (!SetNonblocking(fd))
    {
        LogERR("Create socket, SetNonblocking fail! error code:%u, discrbie:%s", errno, strerror(errno));
        close_socket(fd);
        return false;
    }

    if (is_tcp)
    {
        int val = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));
    }

    sock_fd_ = fd;
    return true;
}

bool AsyncSocket::AttachAsyncSocket(int sock_fd)
{
    if (INVALID_SOCKET == sock_fd)
    {
        assert(false);
        return false;
    }

    if (!SetNonblocking(sock_fd))
    {
        close_socket(sock_fd);
        return false;
    }
    sock_fd_ = sock_fd;
    return true;
}

bool AsyncSocket::SetNonblocking(int fd)
{
#ifdef _WIN32
    unsigned long l = 1;
    int n = ioctlsocket(fd, FIONBIO, &l);
    if (n != 0)
    {
        LogERR("errno = %d reason:%s %d\n", errno, strerror(errno), fd);
        return false;
    }
#else
    int get = fcntl(fd, F_GETFL);
    if (::fcntl(fd, F_SETFL, get | O_NONBLOCK) == -1)
    {
        LogERR("errno = %d reason:%s %d\n", errno, strerror(errno), fd);
        return false;
    }

    //recv timeout
    struct timeval timeout = { 3, 0 };//3s
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#endif

#ifdef IOS
    int set = 1;
    setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif

    return true;
}

void AsyncSocket::CloseSocket()
{
	if (sock_fd_ != INVALID_SOCKET)
	{
		close_socket(sock_fd_);
		sock_fd_ = INVALID_SOCKET;
	}
    
}

bool AsyncSocket::Bind(const char* local_ip, uint16_t &local_port)
{
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = local_ip ? inet_addr(local_ip) : 0;//htonl(local_ip);
    local.sin_port = htons(local_port);
    int ret = ::bind(sock_fd_, (struct sockaddr*)&local, sizeof(local));
    if (ret < 0)
    {
        LogERR("Bind socket (ip:%s,port:%u) fail! error code:%u, discrbie:%s", local_ip, local_port, errno, strerror(errno));
        //do nothing.close outside
        return false;
    }
    if (0 == local_port)
    {
        uint16_t port = 0;
        if (!getSourcePort0(sock_fd_, port))
        {
            LogERR("Bind socket (ip:%s,port:%u), getSourcePort0 fail! error code:%u, discrbie:%s", local_ip, local_port, errno, strerror(errno));
            //do nothing.close outside
            return false;
        }
        local_port = port;
    }
    return true;
}

int AsyncSocket::GetSockError()
{
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(sock_fd_, SOL_SOCKET, SO_ERROR, (char*)&error, &len) < 0)
    {
        //assert(false);
        return GetErrCode();
    }
    return error;
}

bool AsyncSocket::ReuseAddr()
{
    int ret = 0;
    int reuse = 1;
#ifdef _WIN32
    ret = ::setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(int));
#else
    ret = ::setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
#endif
    if (ret < 0)
    {
        return false;
    }
    return true;
}

