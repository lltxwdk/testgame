#include "Utils.h"
#include "TraceLog.h"
#include "SRProtoConnBase.h"

#define SEND_PACK_MAX_LEN 1024*100

SRProtoConnBase::SRProtoConnBase(ITcpConn* tcp_conn, SRStreamHandler* srpack_handler)
    :tcp_conn_(tcp_conn), srpack_handler_(srpack_handler)
{
    last_disconnect_time_ = 0;
}

SRProtoConnBase::~SRProtoConnBase()
{
    tcp_conn_->Release();
}

bool SRProtoConnBase::Init(uint32_t max_sendbuf_len, uint32_t max_recvbuf_len)
{
    return tcp_conn_->Init(this, max_sendbuf_len, max_recvbuf_len);
}

bool SRProtoConnBase::Connect(const char* remote_ip, unsigned short remote_port)
{
    last_disconnect_time_ = Utils::Time();
    return tcp_conn_->Connect((char*)remote_ip, remote_port);
}

void SRProtoConnBase::Close()
{
    last_disconnect_time_ = Utils::Time();
    tcp_conn_->Close();
}

bool SRProtoConnBase::IsClosed()
{
    return last_disconnect_time_ != 0;
}

bool SRProtoConnBase::SendPduData(uint16_t cmd_id, char* data, uint32_t data_len)
{
	char buf[SEND_PACK_MAX_LEN] = { 0 };
    FillHeadField(buf, cmd_id, 0);
	uint16_t len = FillSRPayload(buf, SEND_PACK_MAX_LEN, data, data_len);
    if (0 == len)
    {
        return false;
    }
    int result = tcp_conn_->SendData(buf, len);
    if (0 == result)
    {
        LogERR("send buf overflow!!! len:%d", len);
        assert(false);//ÁÙÊ±²âÊÔ¶ÏÑÔ
        return false;
    }
    return result > 0;
}

bool SRProtoConnBase::SendSRPack(char* pack, uint32_t pack_len)
{
    int result = tcp_conn_->SendData(pack, pack_len);
    if (0 == result)
    {
        LogERR("send packet failed!! buf overflow!!! len:%d", pack_len);
        assert(false);//ÁÙÊ±²âÊÔ¶ÏÑÔ
        return false;
    }
    return result > 0;
}

//ITcpEvent interface impl
void SRProtoConnBase::OnConnected()
{
    last_disconnect_time_ = 0;
	srpack_handler_->OnConnected();
}

void SRProtoConnBase::OnDisconnect(int err_code)
{
    last_disconnect_time_ = Utils::Time();
    srpack_handler_->OnClosed();
}
uint64_t SRProtoConnBase::GetSocketClosedTime()
{
    return last_disconnect_time_;
}

int SRProtoConnBase::OnRecvData(char* data, int data_len)
{
    char* pack = NULL;
    uint8_t reserve = 0;
    uint16_t cmd_type = 0;
    SRProtoParser sr_parser(data, data_len);
    int len = 0;
    do
    {
        len = sr_parser.ExtractPack(pack, reserve, cmd_type);
        if (len < 0)
        {
            char* bptr = sr_parser.GetCurBeginPtr();
            LogERR("bptr:%p,DATA[%x, %x, %x, %x, %x, %x, %x, %x], len:%d", bptr, bptr[0], bptr[1]
                , bptr[2], bptr[3], bptr[4], bptr[5], bptr[6], bptr[7], data_len);
            if (bptr != data)
            {
                char* bptr = data;
                LogERR("optr:%p,DATA[%x, %x, %x, %x, %x, %x, %x, %x]", bptr, bptr[0], bptr[1]
                    , bptr[2], bptr[3], bptr[4], bptr[5], bptr[6], bptr[7]);
            }

            //ÐÅÁîÁ÷Òì³££¬¹Ø±ÕÁ´Â·
            Close();
            return data_len;
        }
        if (0 == len)
        {
            break;
        }
        srpack_handler_->OnRecvSRPack(cmd_type, pack, len);
    } while (true);

    //if (0 > len)
    //{
    //    assert(false);
    //}
    return sr_parser.ExtractTotalLen();
}


