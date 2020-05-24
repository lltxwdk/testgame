#ifndef _ACTIVECONNECT_H_
#define _ACTIVECONNECT_H_
#include "TraceLog.h"
#include "sr_queuemsgid.h"
#include "SRProtoConnBase.h"

#define DEVMGR_TCP_MAX_BUF_LEN 1024 * 100

class CDevMgr;
class DeviceConnMgr;
class AcitiveConnect : public SRStreamHandler
{
public:
	AcitiveConnect(ITcpTransLib* tcp_trans_mod, CDevMgr* pdevMgr, DeviceConnMgr* pDevconnMgr);
	~AcitiveConnect();
	bool Open(const char* remote_ip, unsigned short remote_port);
	bool Close();
	bool IsClosed();
	uint64_t GetSocketClosedTime();

	SRMC::SockRelateData  m_srd;
	//void SetDeviceID(int id) { m_device_id = id; }
	//int GetDeviceID() { return m_device_id; }
	bool SendPktToDevmgr(char* data, uint32_t tdLen, uint16_t cmd_type);

protected:
	//SRStreamHandler impl
	virtual void OnRecvSRPack(uint16_t cmd_type, char* raw_packet, uint16_t raw_packet_len);
	virtual void OnConnected();
	virtual void OnClosed();
private:
	ITcpConn*   m_svr_tcp_conn_;
	SRProtoConnBase* m_svr_tcp_conn_obj;
	DeviceConnMgr*  m_devconnMgr_;
	CDevMgr * m_devMgr_;
};

#endif // #ifndef _ACTIVECONNECT_H_