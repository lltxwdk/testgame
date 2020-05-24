
#if !defined(____FUNCTOR_COMMAND_TASK_H____)
#define ____FUNCTOR_COMMAND_TASK_H____

#include "thread.h"
#include "thread_mutex.h"
#include "functor.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif

#include <list>
#include <set>
#include <vector>
#include "sr_message.h"

/////////////////////////////////////////////////////////////////////////////
////

class IFunctorCommand;

/////////////////////////////////////////////////////////////////////////////
////

class CAsyncThread : public CThreadBase
{
public:
    
    CAsyncThread();
    
    virtual ~CAsyncThread();
    
    bool Start(bool realtime, unsigned long threadCount = 1);
    
    void Stop();
    
    bool Put(IFunctorCommand* pCmd);
    
    unsigned long GetSize() const;
    
    unsigned long GetSize(unsigned long userCode) const;
    
    const std::vector<unsigned long> GetThreadIds() const;  
	virtual void OnMessage(SRMC::Message* msg){}
	
	//类 T 中必须包含OnMessage(Message*)
	template<typename T>
	void Post(T* objOfthis,unsigned int id, SRMC::MessageData *pdata = NULL){
		SRMC::Message* pmsg = new SRMC::Message(); //在Destroy里销毁
		pmsg->message_id = id;
		pmsg->pdata = pdata;
		typedef void (T::* ACTION)(SRMC::Message*);
		this->Put(CFunctorCommand_Arg<T, ACTION, SRMC::Message>::CreateInstance(
			objOfthis, &T::OnMessage, pmsg));
	}
protected:
    
    virtual void Svc();

private:
    
	unsigned long					m_threadCountSum;
    unsigned long					m_threadCountNow;
    bool							m_wantExit;
    std::set<unsigned long>			m_threadIds;
    std::list<IFunctorCommand*>		m_commands;
    CThreadMutex					m_lock;
    CThreadMutexCondition			m_countCond;
    CThreadMutexCondition			m_commandCond;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif //// COMMAND_TASK_H
