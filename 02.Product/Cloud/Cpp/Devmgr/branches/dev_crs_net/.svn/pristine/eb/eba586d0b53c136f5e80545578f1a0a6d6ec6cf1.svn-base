#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

#include "Queue.h"

typedef struct msgStruct
{
	unsigned int uMsgId;
	long long msgParam1;
	long long msgParam2;
}MSG_STRUCT_PKT;

typedef Queue<MSG_STRUCT_PKT> MsgQueue;
typedef Queue<MSG_STRUCT_PKT>* PMsgQueue;
const static int  MAX_MSG_QUEUE_SIZE = 32000;

bool CreateMsgQueue(MsgQueue* &queue, int size = MAX_MSG_QUEUE_SIZE);
bool SendMsgQueue(MsgQueue* queue, unsigned int msgId);
bool SendMsgQueue(MsgQueue* queue, unsigned int msgid, long long param1, long long param2);
bool SendMsgQueue(MsgQueue* queue, MSG_STRUCT_PKT msg);
bool RecvMsgQueue(MsgQueue* queue, MSG_STRUCT_PKT &msg);
void DeleteMsgQueue(MsgQueue* &queue);

typedef unsigned int timer_sec;				/*定时器参数秒*/
typedef unsigned int timer_msec;		        /*定时器参数纳秒*/
typedef void (*TIMER_FUNC)(union sigval v);

#define THREAD_STACK_SIZE  	(2*1024*1024)
typedef void* (*pThreadFunc)(void* param);

/*线程创建函数*/
bool CreateThread(pthread_t& threadId, char* name, pThreadFunc pfunc, void* param, size_t stackSize = THREAD_STACK_SIZE);
/*线程创建函数*/
bool CreateThread(pthread_t& threadId, const char* name, pThreadFunc pfunc, void* param, size_t stackSize = THREAD_STACK_SIZE);

/*线程退出*/
bool ExitThread(pthread_t& threadId);

/*定时器创建函数*/
bool CreateTimer(timer_t &timerId, TIMER_FUNC timerFunc, void* paraTimerFunc, size_t stackSize = THREAD_STACK_SIZE/50);
/*设置定时器函数*/
bool SetTimer(timer_t timerId, timer_sec seconds, timer_msec mSeconds);
/*暂停定时器*/
bool PauseTimer(timer_t timerId);
/*删除定时器*/
bool DeleteTimer(timer_t& timerId);

/*微秒级定时器等待*/
void msleep(unsigned long millisec);
void __nsleep(const struct timespec *req, struct timespec *rem);

#endif//__MSG_QUEUE_H__

