#include "timer_manager.h"

#if defined(WIN32) || defined(_WIN32)
#include "asyncthread.h"
TimerManager::TimerManager()
{
	//����ʹ��winAPI����һ�̳߳�
	m_hTimerQueue = CreateTimerQueue();
	if (NULL == m_hTimerQueue)
	{
		printf("CreateTimerQueue failed (%d)\n", GetLastError());
	}
}

TimerManager::~TimerManager()
{
	// Delete all timers in the timer queue.
	if (!DeleteTimerQueue(m_hTimerQueue))
		printf("DeleteTimerQueue failed (%d)\n", GetLastError());
}

class TimerCallBackData
{
public:
	explicit TimerCallBackData(SRMC::ThreadMessgeHandler* pmsgprochandle, SRMC::TimerData* pregister)
	{
		m_pmsgprochandle = pmsgprochandle;
		m_ptimerdata = pregister;
	}
	~TimerCallBackData(){}
	SRMC::ThreadMessgeHandler* m_pmsgprochandle;
	SRMC::TimerData* m_ptimerdata;
private:
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(TimerCallBackData);
};

//��֤�̰߳�ȫ
static VOID CALLBACK TimerCallBack(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	SUIRUI_UNUSE_VAR(TimerOrWaitFired);
	TimerCallBackData* ptcbd = static_cast<TimerCallBackData*>(lpParam);
	//printf("TimerCallBack \n");
	ptcbd->m_pmsgprochandle->GetSelfThread()->Post(ptcbd->m_pmsgprochandle,
				SRMC::E_MessageId::e_timerid,
				new SRMC::TimerData(*ptcbd->m_ptimerdata));
	//delete ptcbd;
}

//pregister Ϊ��������
TIMERID TimerManager::CreateTimer(SRMC::ThreadMessgeHandler* pmsgprochandle, 
									SRMC::TimerData* pregister,
									unsigned long int msecond,
									bool isPeroic)
{
	TIMERID hTimer;
	DWORD argPeriod = (isPeroic ? msecond : 0);
	// ptcbd �������ڵĹ���!
	TimerCallBackData* ptcbd = new TimerCallBackData(pmsgprochandle, pregister);
	//printf("timer time=%u ms\n", argPeriod);
	if (!CreateTimerQueueTimer(&hTimer, 
								m_hTimerQueue, 
								(WAITORTIMERCALLBACK)TimerCallBack, 
								(PVOID)ptcbd,
								msecond,//duetime �ӳ� duetime ���׳�ʱ���¼��ź�
								argPeriod, //peroc
								 0))
		printf("CreateTimerQueueTimer failed (%d)\n", GetLastError());
	return hTimer;
}

BOOL TimerManager::DeleteTimer(TIMERID timer)
{
	return DeleteTimerQueueTimer(m_hTimerQueue, timer, INVALID_HANDLE_VALUE);
}

#else 
#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h> // for malloc
#include <errno.h>
#include <sys/epoll.h>

#include "thread.h"
#include "sr_queuemsgid.h"
namespace SRMC{

//�����߼�¼�� EventPoller ������ EpollEngine
//EventPoller���¼�ע��ص��ӿ�,�����ӿ�
//EpollEngine�ṩ��·���õ�����(epoll_wait/epoll_ctl�ķ�װ)
//usage:
//  class MyEventPoller : public EventPoller{} -->ʵ�ֽӿں���,ָ����ص��¼�����
//  MyEventPoller* pcallback = new MyEventPoller(); -->�����ص�����
//  pengine->addPoller(pcallback); --ע�ᵽ����
//  pengine->dispatch_forever(); --�����ڵ����߳�һֱ���ע���¼�,����ʱʹ�ûص���������Ӧ����

//EPOLLIN,EPOLLOUT,EPOLLERR,EPOLLHUP,EPOLLRDHUP,EPOLLET
//EPOLLPRI:��������
//����ʹ��EPOLLETʱ,����ķ�����ģʽ�ɵ���������

class EventPoller
{
public:
	virtual ~EventPoller(){}
  	virtual SR_uint32 GetRegisteEvents() const = 0;//������Ҫע����¼�
  	virtual SR_void CallBackOnEvent(SR_uint32 ff) = 0;//ע��ľ�����¼�����ʱ�Ļص�
  	virtual SR_int32 GetEventHandle() const = 0;//����ע��ľ��
};
//������ ע��� EventPoller,�����߸����ͷ�
class EpollEngine
{
public:
	//maxfds ����epoll_wait֪ͨ�������¼���
	EpollEngine(SR_uint32 maxfds_for_one_notify = 4096u){
		m_epfdnums = maxfds_for_one_notify;
		m_epfd = epoll_create1(EPOLL_CLOEXEC);
		if(m_epfd < 0)
			return;
		m_epevents = (struct epoll_event*)malloc(m_epfdnums * sizeof(struct epoll_event));
		if(!m_epevents){
			::close(m_epfd);
			return;
		}
		memset(m_epevents,0,m_epfdnums * sizeof(struct epoll_event));
	}
	~EpollEngine(){
		if(m_epevents){
			free(m_epevents);
			m_epevents = (struct epoll_event*)0;
		}
		if(m_epfd){
			::close(m_epfd);
			m_epfd = 0;
		}
	}
	SR_void One_wait_dispatch(){
		int retfds = epoll_wait(m_epfd, m_epevents, m_epfdnums, -1);
		if(retfds <= 0) return;
		for(int i = 0;i < retfds;i++){
			EventPoller* pdispatcher = (EventPoller*)0;
			pdispatcher = (EventPoller*)(m_epevents[i].data.ptr);
			if (pdispatcher != 0 && pdispatcher != NULL)
			{
				pdispatcher->CallBackOnEvent(m_epevents[i].events);
			}
		}
	}
	
	#define SR_RUN_FOREVER for(;;)
	
	SR_void Dispatch_forever(){//ռ�õ����߳�,����ִ��
		int retfds = 0;
		SR_RUN_FOREVER {
			retfds = epoll_wait(m_epfd, m_epevents, m_epfdnums, -1);
			if((retfds < 0)&& (errno != EINTR)){
				//printf("epoll_wait error.epollengine(%p) stopped\n",this);
				break;
			}
			for(int i = 0;i < retfds;i++){
				EventPoller* pdispatcher = (EventPoller*)0;
				pdispatcher = (EventPoller*)(m_epevents[i].data.ptr);
				if (pdispatcher != 0 && pdispatcher != NULL)
				{
					pdispatcher->CallBackOnEvent(m_epevents[i].events);
				}
			}
		}	
	}
	inline SR_bool AddPoller(EventPoller *pdispathcer) const{
		if (pdispathcer != 0 && pdispathcer != NULL)
		{
			struct epoll_event event;
			memset(&event,0,sizeof(struct epoll_event));
			event.data.ptr = pdispathcer;
			event.events = pdispathcer->GetRegisteEvents();
			int ret = epoll_ctl(m_epfd,EPOLL_CTL_ADD,pdispathcer->GetEventHandle(),&event);
			return ((ret < 0)?false:true);
		}
		else
		{
			return false;
		}
	}
	inline SR_bool RemovePoller(EventPoller *pdispathcer) const{
		if (pdispathcer != 0 && pdispathcer != NULL)
		{
			if (pdispathcer->GetEventHandle() > 0)
			{
				struct epoll_event event;
				int ret = epoll_ctl(m_epfd,EPOLL_CTL_DEL,pdispathcer->GetEventHandle(),&event);
				return ((ret < 0)?false:true);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
private:
	SR_int32            			 m_epfd;
	struct epoll_event*	 		 	 m_epevents;
	SR_uint32 		  			     m_epfdnums; //����epoll_wait ���ص�����¼���
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(EpollEngine);
};

class TimerThread : public CThreadBase
{
public:
	explicit TimerThread(EpollEngine* epengine = NULL){
		m_epengine = epengine;
		if(!m_epengine)
			m_epengine = new EpollEngine();

		CThreadBase::Spawn(true);
	}
	~TimerThread(){ delete m_epengine; }
	virtual void Svc(){
		m_epengine->Dispatch_forever();
	}
	EpollEngine* GetEngine(){return m_epengine;}
private:
	EpollEngine* m_epengine;
};


class TimerPoller : public EventPoller
{
public:
	//�ڹ��캯����ע�ᶨʱ���¼��ͻص�(������ע�ᵽepoll_engine)
	explicit TimerPoller(EpollEngine* epoll_engine,//�����ظö�ʱ��������
						 ThreadMessgeHandler* processThread,//��ʱ�����첽�ص�
						 SR_uint32 millisecond,
						 E_TimerType timertype,
						 TimerData* ptd)//ע��Ķ�ʱ������
	{
		m_epoll_engine = epoll_engine;
		m_processThread = processThread;
		m_ptd = ptd;
		m_counts = 0;
		m_timerfd = ::timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK);
		if(m_timerfd <= 0){
			sr_printf(SR_PRINT_ERROR,"timerfd_create error %s\n",strerror(errno));
			return;
		}
		m_timertype = timertype;
		struct timespec now;   
		if (-1 == ::clock_gettime(CLOCK_MONOTONIC, &now)){
			sr_printf(SR_PRINT_ERROR,"clock_gettime error\n");
			::close(m_timerfd);
			return;
		}
		
		struct itimerspec new_value;   
		new_value.it_value.tv_sec = now.tv_sec;  
		new_value.it_value.tv_nsec = now.tv_nsec;	
		new_value.it_interval.tv_sec = (millisecond/1000ULL);   
		new_value.it_interval.tv_nsec = ((millisecond%1000ULL)*1000000ULL); 
		//0��ʾ����Զ�ʱ����TFD_TIMER_ABSTIME��ʾ�Ǿ��Զ�ʱ��
		if (-1 == ::timerfd_settime(m_timerfd, TFD_TIMER_ABSTIME, &new_value, NULL)){
			sr_printf(SR_PRINT_ERROR,"timerfd_settime error\n");
			::close(m_timerfd);
			return;
		}
		SR_bool isRmOK = false; 
		isRmOK = m_epoll_engine->AddPoller(this);
		sr_printf(SR_PRINT_CRIT,"this=%p TimerPoller destructor--add this timer isRmOK=%d.\n", this, isRmOK);
	}
	
	virtual ~TimerPoller(){
		//if(e_dispoable_timer == m_timertype)
		//	return;
		//::close(m_timerfd);
		//m_epoll_engine->RemovePoller(this);

		if (m_timerfd > 0)
		{			
			SR_bool isRmOK = false; 
			isRmOK = m_epoll_engine->RemovePoller(this);
			::close(m_timerfd);
			m_timerfd = 0;

			sr_printf(SR_PRINT_CRIT,"this=%p TimerPoller destructor--remove and close this timer isRmOK=%d.\n", this, isRmOK);
		}
		else
		{
			sr_printf(SR_PRINT_CRIT,"this=%p TimerPoller destructor--m_timerfd already remove.\n", this);
		}
	}
	
  	virtual SR_uint32 GetRegisteEvents() const {return (EPOLLIN|EPOLLHUP|EPOLLERR|EPOLLET);}
  	virtual SR_void CallBackOnEvent(SR_uint32 ff){
		if(EPOLLIN == (ff&EPOLLIN))
		{
			if (m_timerfd > 0)
			{
				SR_uint64 exp = 0;
				SR_int32 readret;
				readret = read(m_timerfd, &exp, sizeof(SR_uint64));
				m_counts++;
				if(m_timertype == e_dispoable_timer)
				{
					sr_printf(SR_PRINT_CRIT," this=%p e_dispoable_timer in CallBackOnEvent[confid=%"SR_PRIu64", timerid=0x%x].\n", this, m_ptd->m_timerdata, m_ptd->m_timerflag);

					if(1 == m_counts)//�������һ�η��ص��¼�,��Ϊ��һ�����������ص�
					{
						sr_printf(SR_PRINT_CRIT," this=%p e_dispoable_timer in CallBackOnEvent frist time, do nothing.\n", this);
						return;
					}
					else
					{
						SR_bool isRmOK = false; 
						isRmOK = m_epoll_engine->RemovePoller(this);
						::close(m_timerfd);//������sock,��ʹ��CloseSocket()
						m_timerfd = 0;

						sr_printf(SR_PRINT_CRIT," this=%p e_dispoable_timer in CallBackOnEvent, remove and close this timer isRmOK=%d.\n", this, isRmOK);
					}
				}

				if(m_timertype == e_dispoable_timer) 
				{
					sr_printf(SR_PRINT_CRIT,"this=%p e_dispoable_timer in CallBackOnEvent, post event msg[timerdata/confid=%"SR_PRIu64", timerid=0x%x] to controller.\n", this, m_ptd->m_timerdata, m_ptd->m_timerflag);
				}

				//֪ͨ��ʱ���¼�
				m_processThread->GetSelfThread()->Post(m_processThread,
					SRMC::e_timerid,
					new SRMC::TimerData(*m_ptd));
				//if(m_timertype == e_dispoable_timer) 
				//{
				//	sr_printf(SR_PRINT_CRIT,"delete this=%p e_dispoable_timer in CallBackOnEvent.\n", this);
				//	delete this;// WTF,excuse me?
				//}
			} 
			else
			{
				sr_printf(SR_PRINT_ERROR,"this=%p CallBackOnEvent m_timerfd error\n", this);
			}
		}
	}
	
  	virtual SR_socket GetEventHandle() const {return m_timerfd;}
private:
	EpollEngine* m_epoll_engine;
	ThreadMessgeHandler* m_processThread;
	volatile SR_int32 m_counts;//���ش���
	SR_int32 m_timerfd;
	TimerData* m_ptd;
	SR_int32 m_timertype;
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(TimerPoller);
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(TimerPoller);
};

}// namespace SRMC

TimerManager::TimerManager(){
	m_timerengine = new SRMC::TimerThread();
	//((SRMC::TimerThread*)m_timerengine)->Spawn(true);
}
TimerManager::~TimerManager(){
}

TIMERID TimerManager::CreateTimer(SRMC::ThreadMessgeHandler* ptmh,
				SRMC::TimerData* pregister,
				unsigned long int msecond,
				bool isPeroic)
{
	SRMC::E_TimerType ett = (isPeroic ? SRMC::e_periodicity_timer : SRMC::e_dispoable_timer);
	SRMC::TimerPoller* ptp = new SRMC::TimerPoller(((SRMC::TimerThread*)m_timerengine)->GetEngine(),
												ptmh,msecond,ett,pregister);
	return (TIMERID)ptp;
}
BOOL TimerManager::DeleteTimer(TIMERID timer)
{
	//((SRMC::TimerThread*)m_timerengine)->GetEngine()->RemovePoller((SRMC::TimerPoller*)timer);
	//return (BOOL)0;

	SRMC::TimerPoller* ptimer = NULL;
	ptimer = (SRMC::TimerPoller*)timer;
	if (ptimer != NULL)
	{
		delete ptimer;
	}

	return (BOOL)0;
}


#endif