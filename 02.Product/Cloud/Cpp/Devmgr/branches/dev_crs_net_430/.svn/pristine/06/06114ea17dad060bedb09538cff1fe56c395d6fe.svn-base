#ifndef SRMCPROCESS_H
#define SRMCPROCESS_H

#include <set>
#include <map>
#include <string>
#include "device_ser.pb.h"
#include "asyncthread.h"
#include "sr_message.h"
#include "sr_queuemsgid.h"
//#include "sr_tcpheader.h"

#define MAX_BUFFER_MSG_SIZE (50u)
#define MAX_BUFFER_MSG_SIZE_DEVICE (200)

//class DeviceManager;
class TimerManager;
namespace SRMC{

	typedef enum{
		e_DevMgr_sock_init = 8,
		e_DevMgr_sock_connecting = 16,
		e_DevMgr_sock_connect_ok = 32,
		e_DevMgr_sock_error = 64,
		e_DevMgr_sock_closed = 128
	}E_DevSockStat;

class DevmgrSockInfo
{
public:
	DevmgrSockInfo(){
		m_ip.clear();
		m_port = (SR_uint16)0;
		m_devsockstat = e_DevMgr_sock_init;
		m_sockptr = (SR_void*)0;
	}
	SR_void* getSockPtr(){ return m_sockptr; }//TODO:mutex lock
	std::string   m_ip;//"192.168.1.1"左->右
	int     m_port;//主机序
	E_DevSockStat m_devsockstat;
	SR_void*	  m_sockptr;
};

class ThreadMessgeHandler
{
public:
	ThreadMessgeHandler(){}
	~ThreadMessgeHandler(){}
	virtual SR_void OnMessage(Message* msg) = 0;
	virtual CAsyncThread* GetSelfThread() = 0;
private:
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(ThreadMessgeHandler);
};

#define MAX_DEVMGR_NUMS (4)
#define DEVMGR_HEART_FLAGS (0xfeed)

//class BufferedProtoMsgPair{
//public:
//	explicit BufferedProtoMsgPair(SR_uint32 msguid,
//					google::protobuf::Message* pmsg):m_msguid(msguid),m_pmsg(pmsg){}
//	~BufferedProtoMsgPair(){
//		if(m_pmsg)
//			delete m_pmsg;
//		m_pmsg = (google::protobuf::Message*)0;
//	}
//	SR_uint32 m_msguid;
//	google::protobuf::Message* m_pmsg;
//private:
//	SUIRUI_DISALLOW_COPY_AND_ASSIGN(BufferedProtoMsgPair);
//	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(BufferedProtoMsgPair);
//};

class GlobalVars
{
public:
	static SR_int32 getAllTerNums(){
	#ifdef USE_LOCK_RW_ALL_TERSNUM	
		CThreadMutexGuard crit(m_lock);//此处锁无意义
	#endif
		return ters_in_mc;
	}
	static SR_void addOrSubTerNums(SR_int32 addOrSub){//多线程调用addOrSubTerNums时
	#ifdef USE_LOCK_RW_ALL_TERSNUM
		GlobalVars::m_lock.Lock();
	#endif	
		ters_in_mc += addOrSub;
	#ifdef USE_LOCK_RW_ALL_TERSNUM
		GlobalVars::m_lock.Unlock();
	#endif
	}
private:
	static volatile SR_int32 ters_in_mc;
	#ifdef USE_LOCK_RW_ALL_TERSNUM
	static CThreadMutex	m_lock;
	#endif
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(GlobalVars);
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(GlobalVars);
};

#define MAX_DEVMGR_NUMS (4)
#define DEVMGR_HEART_FLAGS (0xfeed)

class DevmgrCoonManager
{
public:
	DevmgrCoonManager()
	{
		m_devmgrInfos.clear();
		m_current_devmgr_sockptr = (void*)0;
		m_current_dev_stat = DEVMGR_HEART_FLAGS;
		m_current_dev_connectcnt = 0u;
	}
	//~DevmgrCoonManager()
	//{
	//	m_devmgrInfos.clear();
	//	m_current_devmgr_sockptr = (void*)0;
	//	m_current_dev_stat = DEVMGR_HEART_FLAGS;
	//	m_current_dev_connectcnt = 0u;
	//}
	//DevmgrSockInfo[MAX_DEVMGR_NUMS] m_devmgrInfos;//std::array 是c++11版本
	std::list<DevmgrSockInfo*> m_devmgrInfos;
	void*   m_current_devmgr_sockptr;
	SR_int32  m_current_dev_stat;
	SR_int32  m_current_dev_connectcnt; // 连接devmgr的次数
	//std::list<BufferedProtoMsgPair*> m_buffered_dev;
};

}// namespace SRMC
#endif //#ifndef SRMCPROCESS_H
