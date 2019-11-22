#ifndef __SRMEDIAHEAD_V2_H__
#define __SRMEDIAHEAD_V2_H__

/* SR packet define, R = 0(signal packet);
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| V |E|R|X|                  PT                                 |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         packet data len       |           X header            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
:                                                               :
:                             X Data                            :
:                               :                               :
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
:                         packet data                           :
:                               :                               :
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

#include <assert.h>
#include <string.h>
#include "TypeDef.h"
#include "SRProtoHeadV2.h"
#include "SRMediaCommon.h"

struct SRMedia
{
    SRMediaInfo media_info_;
    uint16_t rtp_len_;
    uint8_t* rtp_pack_;
    SRPackErr Parse(uint8_t media_fmt[4], uint8_t* pack_buf, uint16_t buf_len);
    SRPackErr Pack(uint8_t media_fmt[4], uint8_t* pack_buf, uint16_t buf_len, uint16_t &out_pack_len);

private:
    //audio
    SRPackErr ParseAudio(uint8_t media_fmt, uint8_t* pack_buf, uint16_t buf_len,
        uint8_t*& rtp_pack, uint16_t& rtp_len);
    SRPackErr PackAudio(uint8_t* media_fmt, uint8_t* pack_buf, uint16_t buf_len, uint16_t &out_pack_len,
        uint8_t* rtp_pack, uint16_t rtp_len);

    //video
    SRPackErr ParseVideo(uint8_t media_fmt, uint8_t* pack_buf, uint16_t buf_len,
        uint8_t*& rtp_pack, uint16_t& rtp_len);
    SRPackErr PackVideo(uint8_t* media_fmt, uint8_t* pack_buf, uint16_t buf_len, uint16_t &out_pack_len,
        uint8_t* rtp_pack, uint16_t rtp_len);
};

union SRPacketUnion
{
    SRSignal signal_pack_;
    SRMedia media_pack_;
};

struct SRPacket
{
	SRPacket();
   
    SRPacketBase base_head_;
    SRPacketUnion packet_;
    SRPackErr Parse(uint8_t* raw_pack, uint16_t raw_pack_len);
    SRPackErr Pack(uint8_t* pack_buf, uint16_t packbuf_len, uint16_t &out_pack_len);
};

#endif
