#ifndef __SRPROTOHEAD_V2_H__
#define __SRPROTOHEAD_V2_H__

#include <assert.h>
#include <string.h>
#include "TypeDef.h"
#define MIN_SRHEAD_LEN 6

const uint8_t cSRProto_V_BitMask = 0xc0;//ver 0
const uint8_t cSRProto_E_BitMask = 0x20;
const uint8_t cSRProto_R_BitMask = 0x10;
const uint8_t cSRProto_X_BitMask = 0x08;

enum SRPackErr
{
    SR_OK,
    SR_INVALIDED,
    SR_INSUFFICIENT
};

struct SRExtendData
{
    uint8_t x_reserve_;
    uint8_t x_len_;
    uint8_t* x_data_;
    SRPackErr Parse(uint8_t* x_begin, uint16_t remain_len);
    SRPackErr Pack(uint8_t* xbuf_begin, uint16_t remainbuf_len, uint16_t &out_pack_len);
};
union SRExtendUnion
{
    uint16_t x_info_;
    SRExtendData x_data_;
};
struct SRHeadExtend
{
    bool has_data_;
    SRExtendUnion extend_info_;
    uint16_t ExtendTotalLen()
    {
        if (!has_data_)
        {
            return 2; //0;
        }
        return extend_info_.x_data_.x_len_ + 2;
    }
    SRPackErr Parse(uint8_t* x_begin, uint16_t remain_len);
    SRPackErr Pack(uint8_t* xbuf_begin, uint16_t remainbuf_len, uint16_t &out_pack_len);
};

struct SRPacketBase
{
    bool E_;
    bool R_;
    bool X_;
    uint8_t* remain_buf_;
    uint16_t remain_len_;
    SRHeadExtend extend_;
    SRPacketBase();
    SRPackErr Parse(uint8_t* raw_pack, uint16_t raw_pack_len);
    SRPackErr Pack(uint8_t* pack_buf, uint16_t packbuf_len, uint16_t &out_pack_len);
};

struct SRSignal
{
    uint32_t PT_; //27 bits
    uint16_t signal_len_;
    uint8_t* signal_body_;

    SRPackErr Parse(uint8_t signal_info[4], uint8_t* pack_buf, uint16_t pack_len);
    SRPackErr Pack(uint8_t signal_info[4], uint8_t* pack_buf, uint16_t pack_len, uint16_t &out_pack_len);
};
#endif


