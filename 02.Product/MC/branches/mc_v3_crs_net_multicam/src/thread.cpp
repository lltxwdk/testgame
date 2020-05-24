
#ifdef WIN32
//#include "stdafx.h"
#endif

#include "thread.h"

#if defined(WIN32) || defined(_WIN32_WCE)
#include <windows.h>
#else
#include <pthread.h>
#include<sched.h>
#endif

#include <cassert>

/////////////////////////////////////////////////////////////////////////////
////

CThreadBase::CThreadBase()
{
    m_nThreadCount	= 0;
    m_bRealTime		= false;
}

CThreadBase::~CThreadBase()
{
}

bool CThreadBase::Spawn(bool realtime)
{
    {
        CThreadMutexGuard mon(m_cLock);
        
#if defined(WIN32) || defined(_WIN32_WCE)
        const HANDLE handle =
            ::CreateThread(NULL, 0, &CThreadBase::SvcRun, this, 0, NULL);
        if (handle == NULL || handle == INVALID_HANDLE_VALUE)
        {
            return (false);
        }
        
        ::CloseHandle(handle);
#else
        pthread_attr_t* attr = NULL;
       
#ifndef LINUX	
        if (realtime)
        {
            attr = new pthread_attr_t;
            pthread_attr_init(attr);
            
            int ret = pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
            if (ret == 0)
            {
                ret = pthread_attr_setschedpolicy(attr, SCHED_RR);
                if (ret == 0)
                {
                    sched_param schedParam;
                    schedParam.sched_priority = sched_get_priority_max(SCHED_RR);
                    ret = pthread_attr_setschedparam(attr, &schedParam);
                }
            }
            
            if (ret != 0)
            {
                pthread_attr_destroy(attr);
                delete attr;
                attr = NULL;
            }
        }
#endif 
        pthread_t threadId = 0;
        const int ret = pthread_create(&threadId, attr, &SvcRun, this);
        
        if (attr != NULL)
        {
            pthread_attr_destroy(attr);
            delete attr;
            attr = NULL;
        }
        
        if (ret != 0)
        {
            return (false);
        }
#endif
        
        ++m_nThreadCount;
        m_bRealTime = realtime;
    }
    
    return (true);
}

void CThreadBase::Wait() const
{
    {
        CThreadMutexGuard mon(((CThreadBase*)this)->m_cLock);
        
        while (m_nThreadCount > 0)
        {
            ((CThreadBase*)this)->m_cCond.Wait(&((CThreadBase*)this)->m_cLock);
        }
    }
}

void CThreadBase::Svc()
{
    assert(0);
}

#if defined(WIN32) || defined(_WIN32_WCE)
unsigned long
__stdcall
#else
void*
#endif
CThreadBase::SvcRun(void* arg)
{
    assert(arg != NULL);
    if (arg == NULL)
    {
        return (0);
    }
    
    CThreadBase* const thread = (CThreadBase*)arg;
    
    {
        CThreadMutexGuard mon(thread->m_cLock);
        
#if defined(WIN32) || defined(_WIN32_WCE)
        if (thread->m_bRealTime)
        {
            ::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
        }
#endif
    }
    
    thread->Svc();
    
    {
        CThreadMutexGuard mon(thread->m_cLock);
        
        --thread->m_nThreadCount;
        thread->m_cCond.Signal();
    }
    
#if !defined(WIN32) && !defined(_WIN32_WCE)
    pthread_detach(pthread_self());
#endif
    
    return (0);
}
