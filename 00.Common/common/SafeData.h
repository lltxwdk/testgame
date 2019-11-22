#ifndef __SAFE_DATA_H__
#define __SAFE_DATA_H__
#include <stdio.h>
#include <string>
#include <map>
#include "CriticalSection.h"
#include "Utils.h"

template<class KEY, class VALUE>
class SafeMap : public DelayRelease<VALUE>
{
public:
    typedef std::map<KEY, VALUE*> LocalMap;
    typedef VALUE* VAULE_PTR;
    SafeMap()
    {
        last_update_count_ = 0;
        last_get_count_ = 0;
    }
    ~SafeMap()
    {
        Clear();
        DelayRelease<VALUE>::RelayExpireObj(0);
    }

public:
    void Insert(KEY key, VALUE *vaule)
    {
        CCriticalAutoLock auto_lock(cs_);
        local_map_[key] = vaule;
        last_update_count_++;
        return;
    }
    void Delete(KEY key)
    {
        CCriticalAutoLock auto_lock(cs_);
        typename LocalMap::iterator itr = local_map_.find(key);
        if (itr == local_map_.end())
        {
            return;
        }

        //delay delete
        DelayRelease<VALUE>::Attach(itr->second);

        //erase
        local_map_.erase(itr);
        last_update_count_++; 
        return;
    }

    //but do not delete
    void Erase(KEY key)
    {
        CCriticalAutoLock auto_lock(cs_);
        typename LocalMap::iterator itr = local_map_.find(key);
        if (itr == local_map_.end())
        {
            return;
        }

        //erase
        local_map_.erase(itr);
        last_update_count_++;
        return;
    }

    VAULE_PTR Find(KEY key)
    {
        CCriticalAutoLock auto_lock(cs_);
        typename LocalMap::iterator itr = local_map_.find(key);
        if (itr == local_map_.end())
        {
            return NULL;
        }

        return itr->second;
    }

    uint32_t Size()
    {
        CCriticalAutoLock auto_lock(cs_);
        return (uint32_t)(local_map_.size());
    }
    void Clear()
    {
        CCriticalAutoLock auto_lock(cs_);
        typename LocalMap::iterator iter = local_map_.begin();
        for (; iter != local_map_.end(); iter++)
        {
            //delay delete
            DelayRelease<VALUE>::Attach(iter->second);
        }

        local_map_.clear();
        last_update_count_++;
    }
    //it's must get by one thread
    void GetLatestMap(LocalMap &result)
    {
        CCriticalAutoLock auto_lock(cs_);
        if (last_get_count_ >= last_update_count_)
        {
            return;
        }

        result = local_map_;
        last_get_count_++;
        return;
    }
    void CopyMap(LocalMap &result)
    {
        CCriticalAutoLock auto_lock(cs_);
        result = local_map_;
    }
private:
    SafeMap(const SafeMap& other);
    SafeMap& operator = (const SafeMap &other);
    CCriticalSection cs_;
    LocalMap local_map_;
    uint64_t last_update_count_;
    uint64_t last_get_count_;
};

#endif