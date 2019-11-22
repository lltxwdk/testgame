#ifndef __SRPROTOPARSER_H__
#define __SRPROTOPARSER_H__
#include "TypeDef.h"
#include "SRProtoHead.h"

class SRProtoParser
{
public:
    explicit SRProtoParser(char* data, uint32_t data_len);
    ~SRProtoParser();
public:
    //成功返回 pack指向的包尺寸, 如果没有足够数据则返回0, 小于0表示协议解析失败 
    int ExtractPack(char*& pack, uint8_t &reserve, uint16_t& cmd_type);

    //返回已经提取的数据总长度
    uint32_t ExtractTotalLen()
    {
        assert(begin_ptr_ >= original_ptr_);
        return begin_ptr_ - original_ptr_;
    }
    char* GetCurBeginPtr(){ return begin_ptr_; }
private:
    char* original_ptr_;
    char* begin_ptr_;
    char* end_ptr_;
};
#endif


