#ifndef __SRPROTOHEAD_H__
#define __SRPROTOHEAD_H__

/* SR packet define(ver=0)
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      'S'      |       'R'     |    ver=0      |   reserve     |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|     packet data len           |         packet type           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
:                         packet data                           :
:                               :                               :
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/


#include <assert.h>
#include <string.h>
#include "TypeDef.h"
#define SRPROTO_HEAD_SIZE 8

inline void FillHeadField(char buf[SRPROTO_HEAD_SIZE], uint16_t cmd, uint8_t reserve)
{
    *buf = 'S';
    *(buf + 1) = 'R';
    *(buf + 2) = 0;
    *(buf + 3) = reserve;
    *((uint16_t*)(buf + 6)) = cmd;
}

inline void FillDataLenField(char buf[SRPROTO_HEAD_SIZE], uint16_t data_len)
{
    *((uint16_t*)(buf + 4)) = data_len;
}

inline uint16_t FillSRPayload(char* buf, uint16_t buf_len, char* payload, uint16_t payload_len)
{
    if (buf_len < payload_len + SRPROTO_HEAD_SIZE)
    {
        assert(false);
        return 0;
    }
    *((uint16_t*)(buf + 4)) = payload_len;
    memcpy(buf + SRPROTO_HEAD_SIZE, payload, payload_len);
    return payload_len + SRPROTO_HEAD_SIZE;
}

inline bool ParseHeadField(char buf[SRPROTO_HEAD_SIZE], uint8_t &reserve, uint16_t& cmd_type, uint16_t& pack_len)
{
    if ('S' != *buf)
    {
        return false;
    }
    if ('R' != *(buf + 1))
    {
        return false;
    }

    if (0 != *(buf + 2))
    { //ver
        return false;
    }
    reserve = *(uint8_t*)(buf + 3);
    pack_len = *((uint16_t *)(buf + 4));
    cmd_type = *((uint16_t *)(buf + 6));
    return true;
}

#endif


