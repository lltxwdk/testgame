#ifndef __SRPROTO_SERIALIZER_H__
#define __SRPROTO_SERIALIZER_H__
#include "TypeDef.h"
#include "SRProtoHead.h"
class SRProtoSerializer
{
public:
    explicit SRProtoSerializer(char* data_buf, uint32_t buf_len);
    ~SRProtoSerializer();
public:
    template<class T>
    uint16_t SerialPack(T& pack, uint8_t reserve = 0);
public:
    template<typename T>
    void Serialize(const T& item);

    template<typename T>
    void Serialize(const T* item);

    template<typename T>
    void Serialize(T items[], uint32_t cnt);

    void Serialize(const char* str);

private:
    char* original_ptr_;
    char* begin_ptr_;
    char* end_ptr_;
};

template<class T>
uint16_t SRProtoSerializer::SerialPack(T& pack, uint8_t reserve)
{
    if (begin_ptr_ + SRPROTO_HEAD_SIZE > end_ptr_)
    {
        return 0;
    }

    FillHeadField(begin_ptr_, pack.cmd_type_, reserve);
    begin_ptr_ += SRPROTO_HEAD_SIZE;
    if (!pack.Serialize(*this))
    {
        return 0;
    }
    uint16_t data_len = begin_ptr_ - original_ptr_ - SRPROTO_HEAD_SIZE;
    FillDataLenField(original_ptr_, data_len);
    return data_len + SRPROTO_HEAD_SIZE;
}

template<typename T>
void SRProtoSerializer::Serialize(const T* item)
{
#ifdef WEBRTC_MAC
#else
    throw - 2;
#endif
}
template<typename T>
void SRProtoSerializer::Serialize(const T& item)
{
    assert(begin_ptr_);
    if (begin_ptr_ + sizeof(T) > end_ptr_)
    {
#ifdef WEBRTC_MAC
#else
        throw -1;
#endif
    }
    memcpy(begin_ptr_, &item, sizeof(T));
    begin_ptr_ += sizeof(T);
}

template<typename T>
void SRProtoSerializer::Serialize(T items[], uint32_t cnt)
{
    assert(begin_ptr_);
    if (0 == cnt)
    {
        return;
    }
    if (begin_ptr_ + sizeof(T) * cnt > end_ptr_)
    {
#ifdef WEBRTC_MAC
#else
        throw -1;
#endif
    }
    memcpy(begin_ptr_, items, sizeof(T) * cnt);
    begin_ptr_ += sizeof(T) * cnt;
}

class SRProtoDeserializer
{
public:
    explicit SRProtoDeserializer(char* data_buf, uint32_t buf_len);
    ~SRProtoDeserializer();
public:
    template<class T>
    bool DeserialPack(T& pack);
public:
    template<typename T>
    void Deserialize(T& item);

    template<typename T>
    void Deserialize(const T* item);

    template<typename T>
    void Deserialize(T items[], uint32_t cnt);

    void Deserialize(char* str);
private:
    char* begin_ptr_;
    char* end_ptr_;
};

template<class T>
bool SRProtoDeserializer::DeserialPack(T& pack)
{
    return pack.Deserialize(*this);
}

template<typename T>
void SRProtoDeserializer::Deserialize(T& item)
{
    assert(begin_ptr_);
    if (begin_ptr_ + sizeof(T) > end_ptr_)
    {
#ifdef WEBRTC_MAC
#else
        throw - 1;
#endif
    }

    memcpy(&item, begin_ptr_, sizeof(T));
}

template<typename T>
void SRProtoDeserializer::Deserialize(const T* item)
{
#ifdef WEBRTC_MAC
#else
    throw - 2;
#endif
}

template<typename T>
void SRProtoDeserializer::Deserialize(T items[], uint32_t cnt)
{
    assert(begin_ptr_);
    assert(cnt > 0);
    if (begin_ptr_ + sizeof(T) * cnt > end_ptr_)
    {
#ifdef WEBRTC_MAC
#else
        throw - 1;
#endif
    }
    memcpy(items, begin_ptr_, sizeof(T) * cnt);
    begin_ptr_ += sizeof(T) * cnt;
}

#define SRPACK_SERIAL_HELPER(STRU_PACK) \
    STRU_PACK(); \
    bool Serialize(SRProtoSerializer& serial);\
    bool Deserialize(SRProtoDeserializer& deserial);\
    static uint16_t cmd_type_;

#define SRPACK_CONSTRUCT_IMPL(STRU_PACK, PACK_CMD_ID) \
uint16_t STRU_PACK::cmd_type_ = PACK_CMD_ID; \
STRU_PACK::STRU_PACK(){ memset(this, 0, sizeof(STRU_PACK));}


#endif


