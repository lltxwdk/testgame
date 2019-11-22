#include "TcpSocket.h"

TcpNormalSocket::TcpNormalSocket(TcpEventSink* sink)
    :tcp_event_handler_(sink)
{
    max_sendbuf_len_ = 0;
    max_recvbuf_len_ = 0;
    recv_buf_ = NULL;
    send_buf_ = NULL;
    tcp_stat_ = tcp_disconnect;
}

TcpNormalSocket::~TcpNormalSocket()
{
    if (recv_buf_)
    {
        delete[] recv_buf_;
    }
    if (send_buf_)
    {
        delete[] send_buf_;
    }
}
bool TcpNormalSocket::AttachAsyncSocket(int sock_fd)
{
    if (IsValidSock())
    {
        assert(false);
        return false;
    }
    if (!AsyncSocket::AttachAsyncSocket(sock_fd))
    {
        return false;
    }
    assert(tcp_disconnect == tcp_stat_);
    tcp_stat_ = tcp_connected;
    return true;
}
void TcpNormalSocket::Init(uint32_t max_sendbuf_len, uint32_t max_recvbuf_len)
{
    assert(0 == max_sendbuf_len_);
    assert(0 == max_recvbuf_len_);
    assert(NULL == recv_buf_);
    assert(NULL == send_buf_);
    if (max_sendbuf_len < 1000)
    {
        max_sendbuf_len = 1000;
    }
    else if (max_sendbuf_len > 1000 * 1000 * 10)
    {
        max_sendbuf_len = 1000 * 1000 * 10;
    }
    if (max_recvbuf_len < 2000)
    {
        max_recvbuf_len = 2000;
    }
    else if (max_recvbuf_len > 1000 * 1000 * 10)
    {
        max_recvbuf_len = 1000 * 1000 * 10;
    }
    max_sendbuf_len_ = max_sendbuf_len;
    max_recvbuf_len_ = max_recvbuf_len;
    recv_buf_ = new char[max_recvbuf_len_];
    send_buf_ = new char[max_sendbuf_len_];
    Reset();
}

bool TcpNormalSocket::Connect(char* remote_ip, uint16_t remote_port)
{
    if (INVALID_SOCKET == sock_fd_)
    {
        assert(false);
        return false;
    }
    assert(tcp_disconnect == tcp_stat_);
    struct sockaddr_in remote;
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = ConvertIPV4ToInt(remote_ip);
    remote.sin_port = htons(remote_port);

    tcp_stat_ = tcp_connecting;
    int ret = ::connect(sock_fd_, (struct sockaddr*)(&remote), sizeof(remote));
	//LogINFO("connetc ret:%d, fd:%d, errno:%d", ret, sock_fd_, errno);
    int nError = 0;
    if (ret < 0 && !IsIgnoreSocketError(nError))
    {
        LogERR("connect is error, error code: %d", nError);
        tcp_stat_ = tcp_disconnect;
        return false;
    }
    return true;
}

int TcpNormalSocket::SendData(char* data, uint32_t data_len)
{
    if (tcp_disconnect == tcp_stat_)
    {
        return -1;
    }
    CCriticalAutoLock guard(send_buf_lock_);
    if (cur_send_pos_ + data_len > max_sendbuf_len_)
    {
        //send buf overflow
        return 0;
    }
    if (tcp_connecting == tcp_stat_)
    {
        memcpy(send_buf_ + cur_send_pos_, data, data_len);
        cur_send_pos_ += data_len;
        return 1;
    }

    if (0 == cur_send_pos_)
    {
        int send_len = ::send(sock_fd_, data, data_len, 0);
        //LogDEBUG("this:%p, fd:%d, cur_send_pos_:%d, ret len:%d, state:%d",
        //    this, sock_fd_, data_len, send_len, tcp_stat_);
        if (send_len == (int)data_len)
        {
            return 1;
        }
        if (send_len < 0)
        {
            int err_code = GetErrCode();
            if (IsErrorEAGAIN(err_code))
            {
                memcpy(send_buf_, data, data_len);
                cur_send_pos_ += data_len;
                tcp_event_handler_->OnSendBlocked();
                LogDEBUG("direct send blocked, fd:%d, len:%d", GetSocketFd(), data_len);
                return 1;
            }
            if (!IsErrorEINTR(err_code))
            {
                //error case
                return -1;
            }
        }
        else
        {
            assert(send_len < (int)data_len);
            uint32_t tmp_len = data_len - send_len;
            memcpy(send_buf_, data + send_len, tmp_len);
            cur_send_pos_ += tmp_len;
            tcp_event_handler_->OnSendBlocked();
            LogDEBUG("direct send imcomplete, fd:%d, len:%d, send len:%d", 
                GetSocketFd(), data_len, send_len);
            return 1;
        }
    }

    memcpy(send_buf_ + cur_send_pos_, data, data_len);
    cur_send_pos_ += data_len;

    int error = 0;
    if (0 > FlushSendBuf(error))
    {
        return -1;
    }
    return 1;
}

int TcpNormalSocket::RecvData()
{
    if (cur_recv_pos_ >= max_recvbuf_len_)
    {
        assert(false);
        ErrorNotify(-1, "recv data, buf overflow");
        return -1;
    }

    int err_code = 0;
    int recv_total = 0;
    int buf_len = 0;
    while (true)
    {
        assert(max_recvbuf_len_ > cur_recv_pos_);
        buf_len = max_recvbuf_len_ - cur_recv_pos_;
        int recv_len = ::recv(sock_fd_, recv_buf_ + cur_recv_pos_, buf_len, 0);
        if (recv_len < 0)
        {
            err_code = GetErrCode();
            if (IsErrorEINTR(err_code))
            {
                continue;
            }
            if (IsErrorEAGAIN(err_code))
            {
                break;
            }
			LogINFO("recv error, code:%d, fd:%d, buf_len:%d, rlen:%d", err_code, sock_fd_, buf_len, recv_len);
            ErrorNotify(err_code, "recv , len<0");
            break;
        }
        if (0 == recv_len)
        {
            ErrorNotify(0, "recv, len=0");
            break;
        }
        cur_recv_pos_ += recv_len;
        recv_total += recv_len;
        if (tcp_event_handler_)
        {
            uint32_t handled_len = tcp_event_handler_->OnRecvData(recv_buf_, cur_recv_pos_);
            assert(handled_len <= cur_recv_pos_);
            cur_recv_pos_ -= handled_len;
            if (cur_recv_pos_ > 0)
            {
                memmove(recv_buf_, recv_buf_ + handled_len, cur_recv_pos_);
            }
        }
        if (recv_len < buf_len)
        {
            break;
        }
    }
    return recv_total;
}

int TcpNormalSocket::FlushSendBuf(int& err_code)
{
    if (0 == cur_send_pos_)
    {
        return 0;
    }
    if (cur_send_pos_ > max_sendbuf_len_)
    {
        assert(false);
        return -1;
    }
    int send_cnt = 0;
    while (true)
    {
        int send_len = ::send(sock_fd_, send_buf_, cur_send_pos_, 0);
        LogDEBUG("this:%p, fd:%d, cur_send_pos_:%d, ret len:%d, state:%d", 
            this, sock_fd_, cur_send_pos_, send_len, tcp_stat_);
        if (send_len < 0)
        {
            err_code = GetErrCode();
            if (IsErrorEINTR(err_code))
            {
                continue;
            }
            if (IsErrorEAGAIN(err_code))
            {
                tcp_event_handler_->OnSendBlocked();
                LogDEBUG("flash send blocked, fd:%d, len:%d, send len:%d",
                    GetSocketFd(), cur_send_pos_, send_len);
                return 1;
            }

            //error case
            return -1;
        }

        assert(send_len <= (int32_t)cur_send_pos_);
        send_cnt += send_len;
        cur_send_pos_ -= send_len;
        if (cur_send_pos_ > 0)
        {
            memmove(send_buf_, send_buf_ + send_len, cur_send_pos_);
        }
        else
        {
            break;
        }
    }
    return send_cnt;
}

int TcpNormalSocket::GetEventFd()
{
    return GetSocketFd();
}

bool TcpNormalSocket::IsNeedWriteEvent()
{
    if (tcp_connecting == tcp_stat_)
    {
        return true;
    }

    CCriticalAutoLock guard(send_buf_lock_);
    return 0 < cur_send_pos_ && cur_send_pos_ <= max_sendbuf_len_;
}

void TcpNormalSocket::HandleEvent(bool ev_err, bool ev_read, bool ev_write)
{
    //LogDEBUG("this:%p, fd:%d, ee:%d, er:%d, ew:%d, state:%d", this, GetSocketFd(), ev_err, ev_read, ev_write, tcp_stat_);
    if (ev_err)
    {
        int error = GetSockError();
        ErrorNotify(error, "handle err ev");
        return;
    }
    if (ev_write)
    {
        int error = 0;
        if (tcp_connecting == tcp_stat_)
        {
            if (ev_read)
            {
                error = GetSockError();
                if (error)
                {
                    ErrorNotify(error, "rw error");
                    return;
                }
            }
            tcp_stat_ = tcp_connected;
            tcp_event_handler_->OnConnected();
        }

        CCriticalAutoLock guard(send_buf_lock_);
        if (FlushSendBuf(error) < 0)
        {
            ErrorNotify(error, "flush error");
            return;
        }
        if (0 == cur_send_pos_)
        {
            tcp_event_handler_->OnSendOver();
            LogDEBUG("send over, fd:%d", GetSocketFd());
        }
    }
    if (ev_read)
    {
        RecvData();
    }
}

