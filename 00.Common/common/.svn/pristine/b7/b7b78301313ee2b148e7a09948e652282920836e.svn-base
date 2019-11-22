#if !defined(_TIMER_H_)
#define _TIMER_H_

#ifdef _WIN32
#include <WinSock2.h>
#include <windows.h>
#else
#include <pthread.h>
#include <sys/time.h>
#include <sys/select.h>
#endif

#include "BaseThread.h"

class Timer
{
public:
  typedef void(*TimerCallback)(void *parm);
  Timer(TimerCallback cb, void *parm);
  Timer(TimerCallback cb, void *parm, long second, long millisecond);
  ~Timer();
  void SetTimer(long second, long millisecond);
  void StartTimer();
  void StopTimer();

  bool WaitForTimer()
  {//等待timer线程结束
      return thread_.IsRunning();
  }

private:
  static uint32_t ThreadFunc(void *p)
  {
    (static_cast<Timer*>(p))->OnTimer();

    return 0;
  }

  void OnTimer();

private:
  CBaseThread thread_;
  long second_;
  long microsecond_;
  bool running_;
  TimerCallback timer_cb_;
  void *cb_owner_;
};

#endif // _TIMER_H_
