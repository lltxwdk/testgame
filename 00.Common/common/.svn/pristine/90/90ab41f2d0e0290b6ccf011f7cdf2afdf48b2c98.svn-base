#include <iostream>
#include <time.h>
#include "Timer.h"

using namespace std;
//////////////////////////public methods//////////////////////////
Timer::Timer(TimerCallback cb, void *parm)
  :second_(1),
  microsecond_(0),
  running_(false),
  timer_cb_(cb),
  cb_owner_(parm)
{
}

Timer::Timer(TimerCallback cb, void *parm, long second, long millisecond)
  :second_(second),
  microsecond_(millisecond*1000),
  running_(false),
  timer_cb_(cb),
  cb_owner_(parm)
{
}

Timer::~Timer()
{
}

void Timer::SetTimer(long second, long millisecond)
{
  second_ = second;
  microsecond_ = millisecond*1000;
}

void Timer::StartTimer()
{
  if (running_)
    return;

  running_ = true;
  unsigned int thd_id = 0;
  thread_.BeginThread(Timer::ThreadFunc, this, thd_id,"CommonTimer");
}

void Timer::StopTimer()
{
  running_ = false;
}

void Timer::OnTimer()
{
  struct timeval tempval;
#ifdef _WIN32
  fd_set read_set;
  SOCKET fd = socket(AF_INET, SOCK_DGRAM, 0);
#endif
  while (running_) {
    timer_cb_(cb_owner_);
    tempval.tv_sec = second_;
    tempval.tv_usec = microsecond_;
#ifdef _WIN32
    FD_ZERO(&read_set);
    FD_SET(fd, &read_set);
    select(-1, &read_set, NULL, NULL, &tempval);
#else
    select(0, NULL, NULL, NULL, &tempval);
#endif
  }
}
