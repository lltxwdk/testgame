#ifndef _DEVMGRCONNECT_H_
#define _DEVMGRCONNECT_H_

#include "sr_queuemsgid.h"
#include "TraceLog.h"
#include "NetUtils.h"
#include "SRProtoConnBase.h"

#define DEVMGR_TCP_MAX_BUF_LEN 1024 * 100

class DeviceManager;
class DevmgrConnect : public SRStreamHandler //∫Õnetmpπ≤”√
{
public:
	DevmgrConnect(ITcpTransLib* tcp_trans_mod, DeviceManager* pMgr, INetAddr const& dev_addr);
    ~DevmgrConnect();
	bool Open(const char* remote_ip, unsigned short remote_port);
    bool Close();
    bool IsClosed();
    uint64_t GetSocketClosedTime();

	SRMC::SockRelateData  m_srd;
	bool SendPktToDevmgrOrNetmp(char* data, uint32_t tdLen, uint16_t cmd_type);
    
protected:
	//SRStreamHandler impl
	virtual void OnRecvSRPack(uint16_t cmd_type, char* raw_packet, uint16_t raw_packet_len);
	virtual void OnConnected();
	virtual void OnClosed();
private:
	ITcpConn*   m_devmgr_tcp_conn_;
	SRProtoConnBase* m_tcp_client_conn_;
    DeviceManager*  m_devMgr_;
	INetAddr dev_addr_;
};

#endif // #ifndef _DEVMGRCONNECT_H_