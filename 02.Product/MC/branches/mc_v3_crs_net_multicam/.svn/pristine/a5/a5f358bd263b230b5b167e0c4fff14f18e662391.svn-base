#ifndef __NETUTILS_H__
#define __NETUTILS_H__
#include <stdint.h>
#include <assert.h>
#include <string>
#include <vector>
#include <set>
#include "NetLibCommon.h"
using namespace std;
class INetAddr
{
public:
    INetAddr(string& ip, uint16_t port)
        :ip_(ip), port_(port)
    {
        init();
    }
    INetAddr(const char* ip, uint16_t port)
        :ip_(ip), port_(port)
    {
        init();
    }
    INetAddr(uint32_t ip, uint16_t port) 
        :port_(port)
    {
        char ip_str[100] = { 0 };
        IPV4IntToStr(ip, ip_str);
        ip_ = ip_str;
        init();
    }
    INetAddr() :port_(0)
    {
    }
    const string& IpStr() const { return ip_; }
    int IpInt() const { return IPV4StrToInt(ip_.c_str()); }
    uint16_t Port() const { return port_; }

    const string& AsString() const
    {
        return addr_;
    }
private:
    friend bool operator<(const INetAddr& lhs, const INetAddr& rhs)
    {
        return lhs.addr_ < rhs.addr_;
    }
    void init()
    {
        char addr_str_[100] = { 0 };
        sprintf(addr_str_, "%s:%d", ip_.c_str(), port_);
        addr_ = addr_str_;
    }
    string ip_;
    uint16_t port_;
    string addr_;
};

typedef vector<INetAddr> INetAddrArr;
typedef set<INetAddr> INetAddrSet;

class NetAddrMgr
{
public:
    NetAddrMgr() :addr_index_(0){}
    void SetAddrArray(INetAddrArr& addr_array){ addr_array_ = addr_array; }
    const INetAddr& GetNextDevAddr()
    {
        assert(!addr_array_.empty());
        if (addr_index_ >= addr_array_.size())
        {
            addr_index_ = 0;
        }
        return addr_array_[addr_index_++];
    }
private:
    INetAddrArr addr_array_;
    uint32_t addr_index_;
};
#endif

