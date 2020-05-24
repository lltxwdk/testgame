#ifndef _DEVICEMANAGER_H_
#define _DEVICEMANAGER_H_

#include <vector>
#include "devmgrConnect.h"
#include "sr_msgprocess.h"
#include "sr_queuemsgid.h"
#include "SafeData.h"

#ifdef LINUX
#define __stdcall
#endif
class TerminalMgr;
class CAsyncThread;
class DeviceManager
{
public:
	DeviceManager(ITcpTransLib* tcp_trans_mod);
    ~DeviceManager();

    bool Init();
    void Fini();
	DevmgrConnect* ConnectDevmgr(const char* ip, int port);
	bool isConnecting() const {return m_is_connect_ok;}
	void ConnectNetmp(const char* ip, int port, const SRMC::SockRelateData& prelatedate);
	void ConnectMP(const char* ip, int port, const SRMC::SockRelateData& prelatedate);
	void ConnectGW(const char* ip, int port, const SRMC::SockRelateData& prelatedate);
	void ConnectCRS(const char* ip, int port, const SRMC::SockRelateData& prelatedate);
	void ConnectRelayserver(const char* ip, int port, const SRMC::SockRelateData& prelatedate);
    int OnTimerPerSecond();

	void startUp();
public:
    virtual int OnReciveDevmgrData(void* pclient, char* data, uint32_t data_len);//must return how many data Has been copied(recive)
	TerminalMgr* m_terminalMgr_;
	SRMC::DevMgrNetMPProcessThread* m_devmpprocess;
	CAsyncThread*	m_DevmgrThread;
	SR_uint32       m_devNumber;
private:
	ITcpTransLib* m_pTcp_trans_mod;
    volatile bool   m_is_connect_ok;
    SafeMap<SR_uint32, DevmgrConnect>	device_connect_map_;
	std::map<SR_uint32, DevmgrConnect*>  get_last_device_map_;
};

#endif