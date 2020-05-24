#if !defined(THREAD_H)
#define THREAD_H
#pragma pack(push, 8)

#include "disabled.h"
#include "thread_mutex.h"

class CThreadBase : public CDisabled
{
protected:
    
    CThreadBase();
    
    virtual ~CThreadBase();
    
    bool Spawn(bool realtime);
    
    void Wait() const;
    
    unsigned long GetThreadCount() const;
    
    virtual void Svc();
    
private:

#if defined(WIN32) || defined(_WIN32_WCE)
    static unsigned long __stdcall SvcRun(void* arg);
#else
    static void* SvcRun(void* arg);
#endif
        
    unsigned long           m_nThreadCount;
    bool                    m_bRealTime;
    CThreadMutex			m_cLock;
    CThreadMutexCondition	m_cCond;
};

#pragma pack(pop)
#endif //// THREAD_H
