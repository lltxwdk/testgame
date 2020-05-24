#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H
#include "sr_msgprocess.h"
#include "sr_queuemsgid.h" // TimerData

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
//CreateTimerQueue
//CreateTimerQueueTimer
//CreateEvent
//WaitForSingleObject WaitForMultipleObjects
//WSAWaitForMultipleEvents

//Waitable Timer Objects 
//--A waitable timer object is a synchronization object whose state is set to signaled 
//   when the specified due time arrives. There are two types of waitable timers that 
//   can be created: manual-reset and synchronization. A timer of either type can also be a periodic timer.

typedef HANDLE TIMERID;
#else 
typedef void* TIMERID;
typedef bool BOOL;
#endif
class TimerManager
{
public:
	TimerManager();
	~TimerManager();
	TIMERID CreateTimer(SRMC::ThreadMessgeHandler*,
				SRMC::TimerData* pregister,
				unsigned long int msecond,
				bool isPeroic);
	BOOL DeleteTimer(TIMERID timer);
private:
	TIMERID m_hTimerQueue;
#if defined(WIN32) || defined(_WIN32)
#else
	void* m_timerengine;//linux 使用设置的线程运行，监听调度定时器
#endif
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(TimerManager);
};



#endif // #ifndef TIMER_MANAGER_H