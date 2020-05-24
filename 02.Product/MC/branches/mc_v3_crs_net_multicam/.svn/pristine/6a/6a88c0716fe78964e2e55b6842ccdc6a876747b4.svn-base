#ifndef _TERMINALMGR_H_
#define _TERMINALMGR_H_
#include <vector>

#include "clientConnect.h"
#include "SafeData.h"
#include "asyncthread.h"
#include "deviceManager.h"
#include "sr_msgprocess.h"

#ifdef LINUX
#define __stdcall
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

//class SRMC::TerMsgProcessThread;
class CAsyncThread;
class TerminalMgr : public ITcpListenEvent
{
public:
	TerminalMgr(ITcpTransLib* tcp_trans_mod);
    ~TerminalMgr();

	bool Init();
	void Fini();
    int OnTimerPerSecond();

	void SaveLicence(char* expringdata, uint32_t srcount, uint32_t stdcount, char* cstdexpiringdate, uint32_t reccount, char* crecexpiringdate, uint32_t livecount, char* cliveexpiringdate, uint32_t maxternumperconf, uint32_t maxconfnum, uint32_t maxmonitorcount, char* cmonitorexpiringdate, uint32_t maxvoicecount, char* cvoiceexpiringdate, char* myversion);

public:
	void OnClosed(void* pclient);
	virtual int OnReciveTermData(void* pclient, char* data, uint32_t data_len);//must return how many data Has been copied(recive)
	SRMC::TerMsgProcessThread* GetThreadProcess(){ return m_ter_process; }
	CAsyncThread* GetProcessThread() { return m_pMainThread; }
protected:
	bool ServerTcpStart(const int listenport);
	
private:
	//ITcpListenEvent impl
	virtual void OnAccept(ITcpConn* new_tcpconn, char* remote_ip, unsigned short remote_port);
	virtual void OnNetError(int err_code);

	CAsyncThread*	m_pMainThread;			// 用户机主线程
	DeviceManager*  m_devmgr;
	SRMC::TerMsgProcessThread* m_ter_process;
	
	ITcpTransLib* tcp_trans_mod_;
	ITcpListenner* tcp_listener_;

	SafeMap<ClientConnect*, ClientConnect>	client_connect_map_;
	std::map<ClientConnect*, ClientConnect*>  get_last_map_;
    uint64_t    client_seq_num_;
};

#endif
