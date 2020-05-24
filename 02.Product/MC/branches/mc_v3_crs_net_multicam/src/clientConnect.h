#ifndef _CLIENTCONNECT_H_
#define _CLIENTCONNECT_H_

#include "TraceLog.h"
#include "NetUtils.h"
#include "SRProtoConnBase.h"

#define CLIENT_TCP_MAX_BUF_LEN 1024*1024*10

class TerminalMgr;
class ClientConnect : public SRStreamHandler
{
public:
	ClientConnect(ITcpConn* tcp_conn, uint64_t seq_num, TerminalMgr* termMgr);
    ~ClientConnect();

	bool Init(const char* remote_ip, uint16_t remote_port);

    void Close();
    bool IsClosed();
    uint64_t GetSocketClosedTime();
    bool SendPktToClient(char* termData, uint32_t tdLen, uint16_t cmd_type);

	inline bool SendPduData(char* pdata, uint32_t data_len,uint16_t cmd_type){ // 调用者不包含SR头的数据,由网络层打包
		if(!m_client_tcp_conn_)
			return 0;
		if (m_client_tcp_conn_->IsClosed())
			return 0;
		return m_client_tcp_conn_->SendPduData(cmd_type, pdata, data_len);
	}    

	string m_remote_ip_;
	uint16_t m_remote_port_;
protected:
	//SRStreamHandler impl
	virtual void OnRecvSRPack(uint16_t cmd_type, char* raw_packet, uint16_t raw_packet_len);
	virtual void OnConnected();
	virtual void OnClosed();

private:
	SRProtoConnBase*   m_client_tcp_conn_;
    TerminalMgr *   m_termMgr_;

	INetAddr client_remote_addr_;
};



#endif