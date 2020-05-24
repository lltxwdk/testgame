#ifndef _DEVICECONNECT_H_
#define _DEVICECONNECT_H_

#include "TraceLog.h"
#include "NetUtils.h"
#include "SRProtoConnBase.h"

#define DEVMGR_TCP_MAX_BUF_LEN 1024 * 100

class CDevMgr;
class DeviceConnect : public SRStreamHandler
{
public:
	DeviceConnect(ITcpConn* tcp_conn, uint64_t seq_num, CDevMgr* devMgr);

    ~DeviceConnect();
	bool Init(const char* remote_ip, uint16_t remote_port);
    void Close();
    bool IsClosed();
    uint64_t GetSocketClosedTime();


    void SetDeviceID(int id) { m_device_id = id; }
	int GetDeviceID() { return m_device_id; }
	bool SendPktToDevConnection(char* termData, uint32_t tdLen, uint16_t cmd_type);

	string m_remote_ip_;
	uint16_t m_remote_port_;
    

protected:
	//SRStreamHandler impl
	virtual void OnRecvSRPack(uint16_t cmd_type, char* raw_packet, uint16_t raw_packet_len);
	virtual void OnConnected();
	virtual void OnClosed();

private:
	SRProtoConnBase*   m_dev_tcp_conn_;
    CDevMgr * m_devMgr_;

    int m_device_id;

	INetAddr dev_addr_;

	//uint64_t    dev_seq_;

	INetAddr dev_remote_addr_;

};



#endif