#include "MsgQueue.h"
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>

bool CreateMsgQueue(MsgQueue* &queue, int size)
{
	if(queue)
	{
		return false;
	}

	queue = new MsgQueue(size);

	return queue != NULL ? true : false;
}

bool SendMsgQueue(MsgQueue* queue, unsigned int msgId)
{
	if(queue)
	{
		MSG_STRUCT_PKT msg;
		msg.uMsgId = msgId;
		msg.msgParam1 = 0;
		msg.msgParam2 = 0;

		if(queue->Push(msg))
		{
			return true;	
		}
	}

	return false;
}

bool SendMsgQueue(MsgQueue* queue, unsigned int msgid, long long param1, long long param2)
{
	if(queue)
	{
		MSG_STRUCT_PKT msg;
                msg.uMsgId = msgid;
		msg.msgParam1 = param1;
		msg.msgParam2 = param2;

		if(queue->Push(msg))
		{
			return true;	
		}
	}

	return false;	
}

bool SendMsgQueue(MsgQueue* queue, MSG_STRUCT_PKT msg)
{
	if(queue && queue->Push(msg))
	{
		return true;
	}

	return false;	
}

bool RecvMsgQueue(MsgQueue* queue, MSG_STRUCT_PKT &msg)
{
	if(queue && queue->Pop(msg))
	{
		return true;
	}

	return false;
}

void DeleteMsgQueue(MsgQueue* &queue)
{
	if(queue != NULL)
	{
		queue->QuitWait();
		usleep(100);

		delete queue;
		queue = NULL;
	}
}


/*创建定时器*/
bool CreateTimer(timer_t &timerId, TIMER_FUNC timerFunc, void* paraTimerFunc, size_t stackSize)
{
	if(NULL == paraTimerFunc)
	{
		printf("para is wrong\n");
		return false;
	}
	
	pthread_attr_t thr_attr;
    pthread_attr_init(&thr_attr);
    pthread_attr_setstacksize(&thr_attr, stackSize);   //stacksize
    pthread_attr_setdetachstate(&thr_attr, PTHREAD_CREATE_DETACHED);

	struct sigevent se;
	memset(&se, 0, sizeof(se));
	se.sigev_notify = SIGEV_THREAD;
	se.sigev_notify_function = timerFunc;   //定时处理函数
    se.sigev_notify_attributes = &thr_attr;
	se.sigev_value.sival_ptr = paraTimerFunc;

	if(timer_create(CLOCK_REALTIME, &se, &timerId) != 0)//创建定时器
	{
		perror("CreateTimer failed:");
		pthread_attr_destroy(&thr_attr);
		return false;
	}

	pthread_attr_destroy(&thr_attr);
	return true;
}

/*设置启动定时器*/
bool SetTimer(timer_t timerId, timer_sec seconds, timer_msec mSeconds)
{
	if(timerId <= 0)
	{
		perror("settimer:");
		return false;
	}
	else
	{
		struct itimerspec ts, ots;
		ts.it_value.tv_sec = seconds;
		ts.it_value.tv_nsec = mSeconds*1000000;//转换为纳秒
		ts.it_interval.tv_sec = seconds;
		ts.it_interval.tv_nsec = mSeconds*1000000;
		ots = ts;

		if (timer_settime(timerId, 0, &ts, &ots) < 0)//设置定时器时间间隔
		{
			perror("SetTimer:");
			return false;
		}
	}
	return true;

}

/*暂停定时器*/
bool PauseTimer(timer_t timerId)
{
	if(timerId <= 0)
	{
		return false;
	}
	else
	{
		struct itimerspec its,oits;
		memset( &its, 0, sizeof(its));
		if(timer_settime(timerId, 0, &its, &oits) < 0)
		{
			perror("pause timer fail:");
			return false;
		}
	}
	return true;
}

/*删除定时器*/
bool DeleteTimer(timer_t &timerId)
{
	if(timerId <= 0)
	{
		return false;
	}
	if(timer_delete(timerId) == 0)
	{
		timerId = 0;
		return true;
	}
	else
	{
		perror("timer_delete:");
		return false;
	}
}

bool CreateThread(pthread_t& threadId, char* name, pThreadFunc pfunc, void* param, size_t stackSize)
{
    pthread_attr_t thr_attr;
    pthread_attr_init(&thr_attr);
    pthread_attr_setstacksize(&thr_attr, stackSize);   //stacksize
    pthread_attr_setdetachstate(&thr_attr, PTHREAD_CREATE_DETACHED);

    if(pthread_create(&threadId, &thr_attr, pfunc, param)!= 0)
    {
        pthread_attr_destroy(&thr_attr);
        if(name != NULL)
        {
            printf("Thread %s create fail!\n", name);
        }
        else
        {
            printf("Thread create fail!\n");
        }

        return false;
    }

    pthread_attr_destroy(&thr_attr);
    return true;
}

bool CreateThread(pthread_t& threadId, const char* name, pThreadFunc pfunc, void* param, size_t stackSize)
{
	return CreateThread(threadId, (char*)name, pfunc, param, stackSize);
}
/*线程退出*/
bool ExitThread(pthread_t& threadId)
{
	if(threadId == 0)
	{
		return false;
	}
	else
	{
        pthread_cancel(threadId);
		threadId = 0;
        //pthread_exit(NULL);
		return true;
	}
}


/******************************************************************
* 函数名称: msleep()
* 描    述: 毫秒级定时器，1秒 = 10E3毫秒 = 10E6 微秒
* 输入参数: millisec(I)毫秒数
* 输出参数: 无
* 返    回: 无
******************************************************************/
void __nsleep(const struct timespec *req, struct timespec *rem)
{
	struct timespec temp_rem;
	if(nanosleep(req, rem) == -1)
	{
		__nsleep(rem, &temp_rem);
	}
}

void msleep(unsigned long millisec)
{
	struct timespec req={0}, rem={0};
	time_t sec = (int)(millisec / 1000);

	millisec = millisec - (sec * 1000);

	req.tv_sec = sec;
	req.tv_nsec = millisec * 1000000L;
	__nsleep(&req, &rem);
}

