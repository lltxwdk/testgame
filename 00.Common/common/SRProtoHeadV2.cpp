#include "SRProtoHeadV2.h"
#ifdef WIN32
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32") 
#else
#include <arpa/inet.h>
#endif

/* SR packet define, R = 0(signal packet);
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| V |E|R|X|                  PT                                 |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         X header fmt          |                               :
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               :
:                             X Data                            :
:                               :                               :
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         packet data len       |                               :
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               :
:                         packet data                           :
:                               :                               :
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

/* xheader fmt, D = 0
0                   1
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|D|           X info            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

/* xheader fmt, D = 1
0                   1
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|D|reserve      |   X len       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

SRPackErr SRExtendData::Parse(uint8_t* x_begin, uint16_t remain_len)
{
    x_len_ = x_begin[1];
    if (remain_len < x_len_ + 2)
    {
        return SR_INSUFFICIENT;
    }

    x_reserve_ = x_begin[0] & 0x7f;
    x_data_ = x_begin + 2;
    return SR_OK;
}

SRPackErr SRExtendData::Pack(uint8_t* xbuf_begin, uint16_t remainbuf_len, uint16_t &out_pack_len)
{
    assert(x_len_ && x_data_);
    if (remainbuf_len < x_len_ + 2)
    {
        return SR_INSUFFICIENT;
    }
    xbuf_begin[0] |= (x_reserve_ & 0x7f);
    xbuf_begin[1] = x_len_;
    memcpy(xbuf_begin + 2, x_data_, x_len_);

    out_pack_len += (2 + x_len_);
    return SR_OK;
}

SRPackErr SRHeadExtend::Parse(uint8_t* x_begin, uint16_t remain_len)
{
    has_data_ = (x_begin[0] & 0x80) != 0;
    if (has_data_)
    {
        SRPackErr resu = extend_info_.x_data_.Parse(x_begin, remain_len);
        if (SR_OK != resu)
        {
            return resu;
        }
        return SR_OK;
    }
    if (remain_len < 2)
    {
        return SR_INSUFFICIENT;
    }
    extend_info_.x_info_ = (*(uint16_t*)x_begin) & 0x7fff;

    return SR_OK;
}

SRPackErr SRHeadExtend::Pack(uint8_t* xbuf_begin, uint16_t remainbuf_len, uint16_t &out_pack_len)
{
    if (has_data_)
    {
        xbuf_begin[0] |= 0x80;
        return extend_info_.x_data_.Pack(xbuf_begin, remainbuf_len, out_pack_len);
    }
    if (remainbuf_len < 2)
    {
        return SR_INSUFFICIENT;
    }
    *(uint16_t*)xbuf_begin = extend_info_.x_info_ & 0x7fff;

    out_pack_len += 2;
    return SR_OK;
}

SRPacketBase::SRPacketBase()
{
    memset(this, 0, sizeof(SRPacketBase));
}

SRPackErr SRPacketBase::Parse(uint8_t* raw_pack, uint16_t raw_pack_len)
{
    if (MIN_SRHEAD_LEN > raw_pack_len)
    {
        return SR_INSUFFICIENT;
    }

    if ((raw_pack[0] & cSRProto_V_BitMask) != 0)//v = 0
    {
        return SR_INVALIDED;
    }

    X_ = (raw_pack[0] & cSRProto_X_BitMask) != 0;
    if (X_)
    {
        SRPackErr resu = extend_.Parse(raw_pack + 4, raw_pack_len - 4);
        if (SR_OK != resu)
        {
            return resu;
        }
    }

    E_ = (raw_pack[0] & cSRProto_E_BitMask) != 0;
    R_ = (raw_pack[0] & cSRProto_R_BitMask) != 0;
#if 0
    remain_buf_ = raw_pack + 4 + extend_.ExtendTotalLen();
    remain_len_ = raw_pack_len - 4 - extend_.ExtendTotalLen();
#else
    uint16_t extend_total_len = X_ ? extend_.ExtendTotalLen() : 0;
    remain_buf_ = raw_pack + 4 + extend_total_len;
    remain_len_ = raw_pack_len - 4 - extend_total_len;
#endif
    assert(remain_len_ > 0 && remain_buf_);
    return SR_OK;
}

SRPackErr SRPacketBase::Pack(uint8_t* pack_buf, uint16_t packbuf_len, uint16_t &out_pack_len)
{
    if (MIN_SRHEAD_LEN > packbuf_len)
    {
        assert(false);
        return SR_INSUFFICIENT;
    }

    pack_buf[0] = 0;//ver 0

    if (X_)
    {
        pack_buf[0] |= cSRProto_X_BitMask;
        SRPackErr resu = extend_.Pack(pack_buf + 4, packbuf_len - 4, out_pack_len);
        if (SR_OK != resu)
        {
            return resu;
        }
    }

    if (E_)
    {
        pack_buf[0] |= cSRProto_E_BitMask;
    }
    if (R_)
    {
        pack_buf[0] |= cSRProto_R_BitMask;
    }

#if 0
    remain_buf_ = pack_buf + 4 + extend_.ExtendTotalLen();
    remain_len_ = packbuf_len - 4 - extend_.ExtendTotalLen();
#else
    uint16_t extend_total_len = X_ ? extend_.ExtendTotalLen() : 0;
    remain_buf_ = pack_buf + 4 + extend_total_len;
    remain_len_ = packbuf_len - 4 - extend_total_len;
#endif
    assert(remain_len_ >= 2 && remain_buf_);

    out_pack_len += 4;
    return SR_OK;
}

SRPackErr SRSignal::Parse(uint8_t signal_info[4], uint8_t* pack_buf, uint16_t packbuf_len)
{
    uint16_t signal_len = *(uint16_t*)(pack_buf);
    if (0 == signal_len)
    {
        return SR_OK;
    }
    if (packbuf_len != signal_len + 2)
    {
        return SR_INSUFFICIENT;
    }
    PT_ = ntohl(*(uint32_t*)signal_info) & 0x07ffffff;
    signal_len_ = signal_len;
    signal_body_ = pack_buf+2;
    return SR_OK;
}

SRPackErr SRSignal::Pack(uint8_t signal_info[4], uint8_t* pack_buf, uint16_t packbuf_len, uint16_t &out_pack_len)
{
    if (packbuf_len < signal_len_ + 2)
    {
        return SR_INSUFFICIENT;
    }

    *(uint32_t*)signal_info &= htonl(0xf8000000);
    *(uint32_t*)signal_info |= htonl(PT_ & 0x07ffffff);
    *(uint16_t*)(pack_buf) = signal_len_;
    memcpy(pack_buf + 2, signal_body_, signal_len_);

    out_pack_len += (signal_len_ + 2);
    return SR_OK;
}


