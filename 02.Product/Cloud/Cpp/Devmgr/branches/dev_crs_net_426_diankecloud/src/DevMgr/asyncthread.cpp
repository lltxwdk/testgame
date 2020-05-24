#ifdef WIN32
#include "stdafx.h"
#endif
#include "asyncthread.h"
#include "functor.h"

#include "thread.h"
#include "thread_mutex.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#if defined(WIN32) || defined(_WIN32_WCE)
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <cassert>
#include <list>
#include <set>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
////

CAsyncThread::CAsyncThread()
{
    m_threadCountSum = 0;
    m_threadCountNow = 0;
    m_wantExit       = false;
}

CAsyncThread::~CAsyncThread()
{
    Stop();
}

bool CAsyncThread::Start(bool realtime,
						 unsigned long threadCount) /* = 1 */
{
    assert(threadCount > 0);
    if (threadCount == 0)
    {
        return (false);
    }
    
    do
    {
        {
            CThreadMutexGuard mon(m_lock);
            
            assert(m_threadCountSum == 0);
            if (m_threadCountSum != 0)
            {
                return (false);
            }
            
            m_threadCountSum = threadCount; //// Stop()!!!
            
            int i = 0;
            
            for (; i < (int)threadCount; ++i)
            {
                if (!Spawn(realtime))
                {
                    break;
                }
            }
            
            assert(i == (int)threadCount);
            if (i != (int)threadCount)
            {
                break;
            }
            
            while (m_threadCountNow < m_threadCountSum)
            {
                m_countCond.Wait(&m_lock);
            }
        }
        
        return (true);
    }
    while (0);
    
    Stop();
    
    return (false);
}

void CAsyncThread::Stop()
{
#if defined(WIN32) || defined(_WIN32_WCE)
    const unsigned long threadId = ::GetCurrentThreadId();
#else
    const unsigned long threadId = (unsigned long)pthread_self();
#endif
    
    {
        CThreadMutexGuard mon(m_lock);
        
        if (m_threadCountSum == 0 || m_wantExit) //// m_wantExit!!!
        {
            return;
        }
        
        assert(m_threadIds.find(threadId) == m_threadIds.end()); //// deadlock!!!
        
        m_wantExit = true;
        m_commandCond.Signal();
    }
    
    Wait();
    
    {
        CThreadMutexGuard mon(m_lock);
        
        m_threadCountSum = 0;
        m_threadCountNow = 0;
        m_wantExit       = false;
    }
}

bool CAsyncThread::Put(IFunctorCommand* command)
{
    assert(command != NULL);
    if (command == NULL)
    {
        return (false);
    }
    
    {
        CThreadMutexGuard mon(m_lock);
        
        assert(m_threadCountSum > 0);
        assert(m_threadCountNow == m_threadCountSum);
        if (m_threadCountSum == 0 || m_threadCountNow != m_threadCountSum)
        {
            return (false);
        }
        
        if (m_wantExit)
        {
            return (false);
        }
        
        m_commands.push_back(command);
        m_commandCond.Signal();
    }
    
    return (true);
}

unsigned long CAsyncThread::GetSize() const
{
    unsigned long size = 0;
    
    {
        CThreadMutexGuard mon(((CAsyncThread*)this)->m_lock);
        
        size = m_commands.size();
    }
    
    return (size);
}

unsigned long CAsyncThread::GetSize(unsigned long userCode) const
{
    unsigned long size = 0;
    
    {
        CThreadMutexGuard mon(((CAsyncThread*)this)->m_lock);
        
        for (std::list<IFunctorCommand*>::const_iterator itr = m_commands.begin();
        itr != m_commands.end(); ++itr)
        {
            if ((*itr)->GetUserCode() == userCode)
            {
                ++size;
            }
        }
    }
    
    return (size);
}

const std::vector<unsigned long> CAsyncThread::GetThreadIds() const
{
    std::vector<unsigned long> threadIds;
    
    {
        CThreadMutexGuard mon(((CAsyncThread*)this)->m_lock);
        
        for (std::set<unsigned long>::const_iterator itr = m_threadIds.begin();
        itr != m_threadIds.end(); ++itr)
        {
            threadIds.push_back(*itr);
        }
    }
    
    return (threadIds);
}

void CAsyncThread::Svc()
{
#if defined(WIN32) || defined(_WIN32_WCE)
    const unsigned long threadId = ::GetCurrentThreadId();
#else
    const unsigned long threadId = (unsigned long)pthread_self();
#endif
    
    {
        CThreadMutexGuard mon(m_lock);
        
        ++m_threadCountNow;
        m_threadIds.insert(threadId);
        m_countCond.Signal();
    }
    
    std::list<IFunctorCommand*> commands;
    
    while (1)
    {
        IFunctorCommand* command = NULL;
        
        {
            CThreadMutexGuard mon(m_lock);
            
            while (1)
            {
                if (m_wantExit || m_commands.size() > 0)
                {
                    break;
                }
                
                m_commandCond.Wait(&m_lock);
            }
            
            if (m_wantExit)
            {
                commands = m_commands;
                m_commands.clear();
                break;
            }
            
            command = m_commands.front();
            m_commands.pop_front();
        }
        
        command->Execute();
        command->Destroy();
    }
    
    while (commands.size() > 0)
    {
        IFunctorCommand* const command = commands.front();
        commands.pop_front();
        command->Execute();
        command->Destroy();
    }
    
    {
        CThreadMutexGuard mon(m_lock);
        
        m_threadIds.erase(threadId);
    }
}
