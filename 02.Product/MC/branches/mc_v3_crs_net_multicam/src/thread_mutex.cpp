#ifdef WIN32
//#include "stdafx.h"
#endif
#include "thread_mutex.h"

#if defined(WIN32) || defined(_WIN32_WCE)
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <cassert>

/////////////////////////////////////////////////////////////////////////////
////

#if !defined(WIN32) && !defined(_WIN32_WCE)
static CThreadMutex g_s_mutex;
#endif

/////////////////////////////////////////////////////////////////////////////
////

#if defined(WIN32) || defined(_WIN32_WCE)

class CThreadMutexImpl : public CDisabled
{
public:
    
    CThreadMutexImpl()
    {
        ::InitializeCriticalSection(&m_mutex);
    }
    
    virtual ~CThreadMutexImpl()
    {
        ::DeleteCriticalSection(&m_mutex);
    }
    
    void Lock()
    {
        ::EnterCriticalSection(&m_mutex);
    }
    
    void Unlock()
    {
        ::LeaveCriticalSection(&m_mutex);
    }
    
private:
    
    CRITICAL_SECTION m_mutex;
};

#endif //// WIN32, _WIN32_WCE

/////////////////////////////////////////////////////////////////////////////
////

CThreadMutex::CThreadMutex()
{
#if defined(WIN32) || defined(_WIN32_WCE)
    m_impl = new CThreadMutexImpl;
#else
    pthread_mutex_init(&m_mutex, NULL);
#endif
}

CThreadMutex::~CThreadMutex()
{
#if defined(WIN32) || defined(_WIN32_WCE)
    delete m_impl;
    m_impl = NULL;
#else
    pthread_mutex_destroy(&m_mutex);
#endif
}

void CThreadMutex::Lock()
{
#if defined(WIN32) || defined(_WIN32_WCE)
    assert(m_impl != NULL);
    if (m_impl == NULL)
    {
        return;
    }
    
    m_impl->Lock();
#else
    pthread_mutex_lock(&m_mutex);
#endif
}

void CThreadMutex::Unlock()
{
#if defined(WIN32) || defined(_WIN32_WCE)
    assert(m_impl != NULL);
    if (m_impl == NULL)
    {
        return;
    }
    
    m_impl->Unlock();
#else
    pthread_mutex_unlock(&m_mutex);
#endif
}

#if !defined(WIN32) && !defined(_WIN32_WCE)

pthread_mutex_t* CThreadMutex::GetImpl()
{
    return (&m_mutex);
}

#endif //// WIN32, _WIN32_WCE

/////////////////////////////////////////////////////////////////////////////
////

CThreadMutexGuard::CThreadMutexGuard(CThreadMutex& mutex)
: m_mutex(mutex)
{
    m_mutex.Lock();
}

CThreadMutexGuard::~CThreadMutexGuard()
{
    m_mutex.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
////

CThreadMutexCondition::CThreadMutexCondition()
{
#if defined(WIN32) || defined(_WIN32_WCE)
    m_autoEvent = (long)::CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(m_autoEvent != (long)NULL);
    assert(m_autoEvent != (long)INVALID_HANDLE_VALUE);
#else
    pthread_cond_init(&m_cond, NULL);
#endif
}

CThreadMutexCondition::~CThreadMutexCondition()
{
#if defined(WIN32) || defined(_WIN32_WCE)
    if (m_autoEvent != (long)NULL &&
        m_autoEvent != (long)INVALID_HANDLE_VALUE)
    {
        ::CloseHandle((HANDLE)m_autoEvent);
        m_autoEvent = (long)NULL;
    }
#else
    pthread_cond_destroy(&m_cond);
#endif
}

void CThreadMutexCondition::Wait(CThreadMutex* mutex)
{
#if defined(WIN32) || defined(_WIN32_WCE)
    
    assert(m_autoEvent != (long)NULL);
    assert(m_autoEvent != (long)INVALID_HANDLE_VALUE);
    if (m_autoEvent == (long)NULL ||
        m_autoEvent == (long)INVALID_HANDLE_VALUE)
    {
        return;
    }
    
    if (mutex != NULL)
    {
        mutex->Unlock();
    }
    
    ::WaitForSingleObject((HANDLE)m_autoEvent, INFINITE);
    
    if (mutex != NULL)
    {
        mutex->Lock();
    }
    
#else //// WIN32, _WIN32_WCE
    
    CThreadMutex* theMutex = mutex;
    if (mutex == NULL)
    {
        theMutex = &g_s_mutex;
        theMutex->Lock();
    }
    
    pthread_cond_wait(&m_cond, theMutex->GetImpl());
    
    if (mutex == NULL)
    {
        theMutex->Unlock();
    }
    
#endif //// WIN32, _WIN32_WCE
}

void CThreadMutexCondition::Signal()
{
#if defined(WIN32) || defined(_WIN32_WCE)
    assert(m_autoEvent != (long)NULL);
    assert(m_autoEvent != (long)INVALID_HANDLE_VALUE);
    if (m_autoEvent == (long)NULL ||
        m_autoEvent == (long)INVALID_HANDLE_VALUE)
    {
        return;
    }
    
    ::SetEvent((HANDLE)m_autoEvent);
#else
    pthread_cond_signal(&m_cond);
#endif
}
