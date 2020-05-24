#ifndef SR_QUEUEMSGID_H
#define SR_QUEUEMSGID_H
#include <set>
#include <string>
#include <string.h>
#include <map>
#include "sr_message.h"
//#include "sr_tcpheader.h"

namespace SRMC{


typedef enum //定时器种类
{	
	e_periodicity_timer = 0xcad,//默认的周期性定时器	
	e_dispoable_timer = 0x19880824,//一次性定时器
}E_TimerType;//定义和解释都归调用者定义

//创建和析构也由调用者负责
class TimerData : public MessageData
{
public:	
	explicit TimerData(SR_uint32 timerflag,SR_uint64 timerdata)	
	{		
		m_timerdata = timerdata;		
		m_timerflag = timerflag;	
	}	
	~TimerData(){}// do nothing	
	SR_uint32 m_timerflag;//区分同一类型的不同作用的定时器	
	SR_uint64 m_timerdata;
};

class SockRelateData : public MessageData
{
public:
	//enum tagMsgId{
	//	e_connect_error = e_sock_connect_error,
	//	e_connect_ing = e_sock_connect_ing,
	//	e_connect_ok = e_sock_connect_ok
	//};
	explicit SockRelateData(SR_uint32 socktype = 0U, SR_uint32 relatedata = 0U)
	{
		m_socktype = socktype;
		m_relatedata = relatedata;
	}
	const SockRelateData& operator=(const SockRelateData& sd)
	{
		if (this == &sd)
			return *this;
		this->m_socktype = sd.m_socktype;
		this->m_relatedata = sd.m_relatedata;
		//this->m_sock = sd.m_sock;	 // del by csh
		this->m_pSocket = sd.m_pSocket;
		return *this;
	}
	SockRelateData(const SockRelateData& sd)
	{
		this->operator=(sd);
	}
	//SR_socket m_sock;//sock句柄 --> dispatcher发送消息时填充	// del by csh
	SR_void* m_pSocket;
	SR_uint32 m_socktype;//socket 类型	
	SR_uint32 m_relatedata;//socket 相关联的数据
};


//自动删除 MessageData的智能指针
class Simple_ScopedPtr
{
public:
	explicit Simple_ScopedPtr(MessageData* ptr){m_ptr = ptr;}
	~Simple_ScopedPtr(){delete m_ptr;}
private:
	MessageData* m_ptr;
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(Simple_ScopedPtr);
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(Simple_ScopedPtr);
};

} //namespace SRMC

#endif //#ifndef SR_QUEUEMSGID_H
