#include <assert.h>
#include "SRProtoParser.h"

SRProtoParser::SRProtoParser(char* data, uint32_t pack_len)
{
    original_ptr_ = data;
    begin_ptr_ = data;
    end_ptr_ = data + pack_len;
}

SRProtoParser::~SRProtoParser()
{
}

int SRProtoParser::ExtractPack(char*& pack, uint8_t &reserve, uint16_t& cmd_type)
{
    assert(begin_ptr_);
    pack = NULL;
    if (begin_ptr_ + SRPROTO_HEAD_SIZE > end_ptr_)
    {
        //data's size is not enough
        return 0;
    }

    uint16_t data_len = 0;
    if (!ParseHeadField(begin_ptr_, reserve, cmd_type, data_len))
    {
        //proto head is invalid
        return -1;
    }
    if (begin_ptr_ + SRPROTO_HEAD_SIZE + data_len > end_ptr_)
    {
        //data's size is not enough
        return 0;
    }

    //pack = begin_ptr_ + SRPROTO_HEAD_SIZE;
    pack = begin_ptr_;
    begin_ptr_ += SRPROTO_HEAD_SIZE;
    begin_ptr_ += data_len;
    assert(begin_ptr_ <= end_ptr_);
    return data_len + SRPROTO_HEAD_SIZE;
}


