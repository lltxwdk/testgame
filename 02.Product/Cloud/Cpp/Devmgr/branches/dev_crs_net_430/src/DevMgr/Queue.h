#ifndef __DE_QUEUE__
#define __DE_QUEUE__

#include <pthread.h>
#include <deque>
#include <map>

/************************************************************************
// 此类专为push, pop函数为两个不同线程调用而设计，
// 避免了一个读写锁的开销
// 内部实现了阻塞与唤醒机制
************************************************************************/

template <class A>
class Queue
{
public:		
	Queue(int uQueSize = 0);	
	~Queue();
	bool Push(const A &item);    //为生产者线程调用
	bool Pop(A &item);           //为消费者线程调用
	int  GetSize();
	int  QuitWait();

private:		
	std::deque<A> m_que;   
	pthread_mutex_t mutex;
	pthread_cond_t cond;	
	unsigned int m_uQueSize;   
	int m_bQuitFlag;
};

template <class A>
Queue<A>::Queue(int uQueSize)
{
	m_uQueSize =  uQueSize;
	m_bQuitFlag = false;
	pthread_mutex_init(&mutex,NULL);	
	pthread_cond_init(&cond,NULL);	
}

template <class A>
Queue<A>::~Queue()
{
	pthread_cond_destroy(&cond);	
	pthread_mutex_destroy(&mutex);
	m_que.clear();
}

template <class A>
int Queue<A>::GetSize()
{
	pthread_mutex_lock(&mutex);	
	int size = m_que.size();
	pthread_mutex_unlock(&mutex);
	return size;
}


template <class A>
bool Queue<A>::Push(const A &item)
{
	pthread_mutex_lock(&mutex);	
	if(m_uQueSize && (m_que.size() == m_uQueSize))  
	{	
		//printf("FILE %s,LINE %d %s(): return FALSE\n",__FILE__,__LINE__,__func__);
		pthread_mutex_unlock(&mutex);
		return false;
	}

	m_que.push_back(item);	

	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);    //唤醒一个线程
	return true;
}

template <class A>
bool Queue<A>::Pop(A &item)
{	
	pthread_mutex_lock(&mutex);	
	while (m_que.empty()) 
	{		
		if(m_bQuitFlag)
		{
			//printf("FILE %s,LINE %d %s(): return FALSE\n",__FILE__,__LINE__,__func__);
			pthread_mutex_unlock(&mutex);
			return false; 
		}		
		pthread_cond_wait(&cond,&mutex);  //线程睡眠，等待唤醒
	}

	if(m_que.size() == 0)
	{	
		//printf("FILE %s,LINE %d %s(): return FALSE\n",__FILE__,__LINE__,__func__);
		pthread_mutex_unlock(&mutex);
		return false;
	}

	item = m_que.front();
	m_que.pop_front();	
	pthread_mutex_unlock(&mutex);
	return true;
}

//强制线程从POP的阻塞中退出
template <class A>
int  Queue<A>::QuitWait()
{
	m_bQuitFlag = true;
	pthread_cond_signal(&cond);    //唤醒一个线程
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//此类专为Insert, Erase函数为两个不同线程调用而设计，
//避免了一个读写锁的开销
//内部实现了阻塞与唤醒机制
//////////////////////////////////////////////////////////////////////////
template <class key, class A>
class MapKey
{
public:		
	MapKey(int uMapSize = 0);	
	~MapKey();
	bool Insert(const key &k, const A &item);    //为生产者线程调用
	bool Erase(key &k, A &item);           //为消费者线程调用
	int  GetSize();
	int  QuitWait();

private:		
	std::map<key, A> m_map;   
	pthread_mutex_t mutex;
	pthread_cond_t cond;	
	unsigned int m_uMapSize;   
	int m_bQuitFlag;
};

template <class key, class A>
MapKey<key, A>::MapKey(int uMapSize)
{
	m_uMapSize =  uMapSize;
	m_bQuitFlag = false;
	pthread_mutex_init(&mutex,NULL);	
	pthread_cond_init(&cond,NULL);	
}

template <class key, class A>
MapKey<key, A>::~MapKey()
{
	pthread_cond_destroy(&cond);	
	pthread_mutex_destroy(&mutex);
	m_map.clear();
}

template <class key, class A>
int MapKey<key, A>::GetSize()
{
	pthread_mutex_lock(&mutex);	
	int size = m_map.size();
	pthread_mutex_unlock(&mutex);
	return size;
}


template <class key, class A>
bool MapKey<key, A>::Insert(const key &k, const A &item)
{
	pthread_mutex_lock(&mutex);	
	if(m_uMapSize && (m_map.size() == m_uMapSize))  
	{	
		//printf("FILE %s,LINE %d %s(): return FALSE\n",__FILE__,__LINE__,__func__);
		pthread_mutex_unlock(&mutex);
		return false;
	}

	m_map.insert( typename std::map<key, A>::value_type(k, item));

	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);    //唤醒一个线程
	return true;
}

template <class key, class A>
bool MapKey<key, A>::Erase(key &k, A &item)
{	
	pthread_mutex_lock(&mutex);	

	while (m_map.empty()) 
	{		
		if(m_bQuitFlag)
		{
			//printf("FILE %s,LINE %d %s(): return FALSE\n",__FILE__,__LINE__,__func__);
			pthread_mutex_unlock(&mutex);
			return false; 
		}		
		pthread_cond_wait(&cond, &mutex);  //线程睡眠，等待唤醒
	}

	if(m_map.size() == 0)
	{	
		//printf("FILE %s,LINE %d %s(): return FALSE\n",__FILE__,__LINE__,__func__);
		pthread_mutex_unlock(&mutex);
		return false;
	}

	typename std::map<key, A>::iterator mapIter = m_map.begin();
	item = mapIter->second;
	k    = mapIter->first;
	m_map.erase(mapIter);

	pthread_mutex_unlock(&mutex);
	return true;
}

//强制线程从Erase的阻塞中退出
template <class key, class A>
int  MapKey<key, A>::QuitWait()
{
	m_bQuitFlag = true;
	pthread_cond_signal(&cond);    //唤醒一个线程
	return 0;
}

#endif//__DE_QUEUE__


