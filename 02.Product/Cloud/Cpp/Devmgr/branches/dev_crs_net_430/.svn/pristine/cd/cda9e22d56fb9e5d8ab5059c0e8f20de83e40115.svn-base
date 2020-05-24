#ifndef _DEVICECONNMGR_H_
#define _DEVICECONNMGR_H_

#include "TcpNetInterface.h"
#include "deviceConnect.h"
#include "sr_type.h"
#include "SafeData.h"
#include "sr_queuemsgid.h"
#include "sr_msgprocess.h"
#include "ActiveConnect.h"
#include "DevMgr.h"

class CDevMgr;
//class DevmgrCoonManager;
class DeviceConnMgr : public ITcpListenEvent
{
public:
	DeviceConnMgr(ITcpTransLib* tcp_trans_mod, CDevMgr* devMgr);
    ~DeviceConnMgr();

	SRMC::DevmgrCoonManager* m_devmgrinfomanager;

	bool Init(const char* listen_ip, uint16_t listen_port);

	void DeviceOnClose(DeviceConnect * pDeviceConnet);
	void GetLatestMap(std::map<DeviceConnect *, DeviceConnect *> &result);
	void GetLatestMap(std::map<SR_uint32, AcitiveConnect*> &result);

	void ConnectDevmgr();
	AcitiveConnect* ConnectDevmgr(const char* ip, int port/*, const SRMC::SockRelateData& prelatedate*/);
	bool isConnecting() const { return m_is_connect_ok; }
	
	SR_uint32       m_devNumber;

	SR_void onActiveConnSockError(void *pActiveTcp);
	SR_void onActiveConnectSuccess(void *pActiveTcp);
	int CheckActiveConnTimer();

private:
	virtual void OnAccept(ITcpConn* new_tcpconn, char* remote_ip, unsigned short remote_port);
	virtual void OnNetError(int err_code);

private:
	ITcpTransLib* tcp_trans_mod_;
	ITcpListenner* tcp_listener_;
    CDevMgr * m_devMgr_;

	SafeMap<DeviceConnect*, DeviceConnect>	device_connect_map_;
	std::map<DeviceConnect*, DeviceConnect*> get_last_map_;
    uint64_t    dev_seq_num_;

	volatile bool   m_is_connect_ok;
	SafeMap<SR_uint32, AcitiveConnect>	svr_connect_map_;
	std::map<SR_uint32, AcitiveConnect*>  get_last_svr_map_;

};

#endif