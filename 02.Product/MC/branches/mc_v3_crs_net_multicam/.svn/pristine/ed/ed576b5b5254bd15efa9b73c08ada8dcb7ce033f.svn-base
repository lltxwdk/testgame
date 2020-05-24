
#if !defined(____THREAD_MUTEX_H____)
#define ____THREAD_MUTEX_H____
#pragma pack(push, 8)

#include "disabled.h"

#if !defined(WIN32) && !defined(_WIN32_WCE)
#include <pthread.h>
#endif

/////////////////////////////////////////////////////////////////////////////
////

#if defined(WIN32) || defined(_WIN32_WCE)
class CThreadMutexImpl;
#endif

/////////////////////////////////////////////////////////////////////////////
////

class CThreadMutex : public CDisabled
{
public:
    
    CThreadMutex();
    
    virtual ~CThreadMutex();
    
    void Lock();
    
    void Unlock();
    
#if !defined(WIN32) && !defined(_WIN32_WCE)
    pthread_mutex_t* GetImpl();
#endif
    
private:
    
#if defined(WIN32) || defined(_WIN32_WCE)
    CThreadMutexImpl* m_impl;
#else
    pthread_mutex_t      m_mutex;
#endif
};

/////////////////////////////////////////////////////////////////////////////
////

class CThreadMutexGuard : public CDisabled
{
public:
    
    CThreadMutexGuard(CThreadMutex& mutex);
    
    virtual ~CThreadMutexGuard();
    
private:
    
    CThreadMutex& m_mutex;
};

/////////////////////////////////////////////////////////////////////////////
////

class CThreadMutexCondition : public CDisabled
{
public:
    
    CThreadMutexCondition();
    
    virtual ~CThreadMutexCondition();
    
    ////
    //// mutexø…“‘Œ™NULL
    ////
    void Wait(CThreadMutex* mutex);
    
    void Signal();
    
private:
    
#if defined(WIN32) || defined(_WIN32_WCE)
    long           m_autoEvent;
#else
    pthread_cond_t m_cond;
#endif
};

/////////////////////////////////////////////////////////////////////////////
////

class CNullMutex
{
public:
    
    void Lock()
    {
    }
    
    void Unlock()
    {
    }
};

/////////////////////////////////////////////////////////////////////////////
////

#pragma pack(pop)
#endif //// ____THREAD_MUTEX_H____
