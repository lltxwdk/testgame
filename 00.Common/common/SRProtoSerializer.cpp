#include <string.h>
#include "SRProtoSerializer.h"


SRProtoSerializer::SRProtoSerializer(char* data_buf, uint32_t buf_len)
    :original_ptr_(data_buf)
    , begin_ptr_(data_buf)
    , end_ptr_(data_buf + buf_len)
{
}

SRProtoSerializer::~SRProtoSerializer()
{
}

void SRProtoSerializer::Serialize(const char* sz)
{
    size_t size = strlen(sz);
    if (size > 1000)
    {
        throw -3;
    }
    uint16_t szlen = size;
    memcpy(begin_ptr_, &szlen, sizeof(uint16_t));
    begin_ptr_ += sizeof(uint16_t);
    if (0 == szlen)
    {
        return;
    }
    if (begin_ptr_ + szlen > end_ptr_)
    {
        throw -1;
    }
    memcpy(begin_ptr_, sz, szlen);
    begin_ptr_ += szlen;
}

SRProtoDeserializer::SRProtoDeserializer(char* data_buf, uint32_t buf_len)
    :begin_ptr_(data_buf)
    , end_ptr_(data_buf + buf_len)
{
}

SRProtoDeserializer::~SRProtoDeserializer()
{
}
void SRProtoDeserializer::Deserialize(char* str)
{
    if (begin_ptr_ + sizeof(uint16_t) > end_ptr_)
    {
        throw - 1;
    }
    uint16_t szlen = *((uint16_t*)begin_ptr_);
    begin_ptr_ += sizeof(uint16_t);
    if (0 == szlen)
    {
        return;
    }
    if (begin_ptr_ + szlen > end_ptr_)
    {
        throw - 1;
    }
    memcpy(str, begin_ptr_, szlen);
}

