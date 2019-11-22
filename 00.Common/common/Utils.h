#ifndef __UTILS_H__
#define __UTILS_H__
#include <iostream>
#include <map>
#include <string>
#include <list>
#include <set>
#include <vector>
#include <assert.h>
#include "TypeDef.h"
#include "CriticalSection.h"

namespace Utils
{
    //for time 
    static const int64_t kNumMillisecsPerSec = 1000;
    static const int64_t kNumMicrosecsPerSec = 1000 * 1000;
    static const int64_t kNumNanosecsPerSec = 1000 * 1000 * 1000;

    static const int64_t kNumMicrosecsPerMillisec = kNumMicrosecsPerSec / kNumMillisecsPerSec;
    static const int64_t kNumNanosecsPerMillisec =  kNumNanosecsPerSec / kNumMillisecsPerSec;

    // Returns the current time in milliseconds. ���ص�ǰ�ĺ���
    uint64_t Time();
    // Returns the current time in nanoseconds.
    uint64_t TimeNanos();

    void Sleep(uint32_t millisecond);

    typedef std::vector<std::string> StringArray;
    //zjg add string utils
    class String
    {
    public:
        // as sprintf format %s,%d,%u,%I64d,%lld ...
        static std::string  Format( const char *pszFormat, ... );
        static std::string  FormatV( const char *pszFormat, va_list pArgList );
        static std::string &AppendFormat( std::string &strOrg, const char *pszFormat, ... );
        static int Split( const std::string &strContent, Utils::StringArray &nValues, const std::string &strKey );
        static std::string Trim(std::string &s);
    };

    //get error code
    std::string GetErrorInfo();
	char* itoa(int value, char* string, int radix);
	void itoa(int num, char str[]);
	int atoi(char s[]);

    //path
    std::string GetModuleDirectoryPath();
}

template<class T>
class DelayRelease
{
private:
    typedef struct tagDelayNode 
    {
        T* obj_;
        uint64_t timestamp_;
        tagDelayNode(T* obj)
        {
            obj_ = obj;
            timestamp_ = Utils::Time(); 
        }
    }DelayNode;
public:
    ~DelayRelease()
    {
        node_lst_lock.Lock();
        while (!delay_node_lst_.empty())
        {
            delete delay_node_lst_.begin()->obj_;
            delay_node_lst_.pop_front();
        }
        node_lst_lock.UnLock();
    }
    void Attach(T* obj)
    {
        node_lst_lock.Lock();
        delay_node_lst_.push_back(DelayNode(obj));
        node_lst_lock.UnLock();
    }

    void RelayExpireObj(uint64_t delay_ms)
    {
        uint64_t now = Utils::Time();
        node_lst_lock.Lock();
        while(!delay_node_lst_.empty())
        {
            if (now - delay_node_lst_.begin()->timestamp_ < delay_ms)
            {
                break;
            }
            delete delay_node_lst_.begin()->obj_;
            delay_node_lst_.pop_front();
        }
        node_lst_lock.UnLock();
    }
    template<class Handler>
    void HandleDeletingObj(Handler* handler)
    {
        node_lst_lock.Lock();
        typename std::list<DelayNode>::iterator it = delay_node_lst_.begin();
        for (; it != delay_node_lst_.end(); it++)
        {
            handler->OnHandlDeletingObj(it->obj_);
        }
        node_lst_lock.UnLock();
    }
private:
    std::list<DelayNode> delay_node_lst_;
    CCriticalSection node_lst_lock;
};

template<class T>
class SelfDeleteObj
{
public:
    SelfDeleteObj(T *obj)
    {
        assert(NULL != obj);
        obj_ = obj;
    }
    ~SelfDeleteObj()
    {
        delete obj_;
        obj_ = NULL;
    }

    T* GetObj()
    {
        return obj_;
    }

private:
    T* obj_;
};

#endif

