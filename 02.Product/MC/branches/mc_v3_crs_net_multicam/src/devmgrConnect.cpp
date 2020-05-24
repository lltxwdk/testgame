#include "proto_msgid_define.h"
#include "TraceLog.h"
#include "device_ser.pb.h"
#include "devmgrConnect.h"
#include "deviceManager.h"
#include "sr_tcpheader.h"
#include "ini_configuration.h"

DevmgrConnect::DevmgrConnect(ITcpTransLib* tcp_trans_mod, DeviceManager* pdevMgr, INetAddr const& dev_addr)
{
	m_devmgr_tcp_conn_ = NULL;
	m_devmgr_tcp_conn_ = tcp_trans_mod->CreateTcpConn(); // 创建tcp主动连接
	
	m_tcp_client_conn_ = new SRProtoConnBase(m_devmgr_tcp_conn_, this); // 创建事件对象
	//m_tcp_client_conn_->Init(1024 * 100, 1024 * 10);
	m_tcp_client_conn_->Init(DEVMGR_TCP_MAX_BUF_LEN, DEVMGR_TCP_MAX_BUF_LEN);
	m_devMgr_ = pdevMgr;
	dev_addr_ = dev_addr;
}

//// override PduHandler::OnSendPdu()
//// CallStack --by TcpNetClient::OnConnected() override INetTcpEvent::OnConnected()
////                   TcpEpollEvent::HandleConnectedEvent()
////                       Connector::handleSend()
////                          Channel::handleEvent()
//void DevmgrConnect::OnSendPdu()
//{
//	m_srd.m_pSocket = (void *)this;
//	m_devMgr_->m_DevmgrThread->Post(m_devMgr_->m_devmpprocess, SRMC::e_sock_connect_ok,
//		new SRMC::SockRelateData(m_srd));
//}

void DevmgrConnect::OnConnected()
{
	m_srd.m_pSocket = (void *)this;
	m_devMgr_->m_DevmgrThread->Post(m_devMgr_->m_devmpprocess, SRMC::e_sock_connect_ok,
		new SRMC::SockRelateData(m_srd));
}

DevmgrConnect::~DevmgrConnect()
{
	SAFE_DELETE(m_tcp_client_conn_);
	m_devmgr_tcp_conn_ = NULL;
}

bool DevmgrConnect::Open(const char* remote_ip, unsigned short remote_port)
{
	if (!m_tcp_client_conn_->Connect(remote_ip, remote_port))
	{
		sr_printf(SR_PRINT_ERROR, "DevmgrConnect open connection failed:%s[%d]\n", remote_ip, remote_port);
		return false;
	}
	sr_printf(SR_PRINT_DEBUG, "DevmgrConnect open connection success:%s[%d]\n", remote_ip, remote_port);
	return true;
}

bool DevmgrConnect::Close()
{
	//if (!m_tcp_client_conn_->IsClosed())
	//{
	//	m_tcp_client_conn_->Close();
	//}

	m_tcp_client_conn_->Close();

	return true;
}

bool DevmgrConnect::IsClosed()
{
	return m_tcp_client_conn_->IsClosed();
}

uint64_t DevmgrConnect::GetSocketClosedTime()
{
	return m_tcp_client_conn_->GetSocketClosedTime();
}

void DevmgrConnect::OnRecvSRPack(uint16_t cmd_type, char* raw_packet, uint16_t raw_packet_len)
{
	m_devMgr_->OnReciveDevmgrData((void*)this, raw_packet, raw_packet_len);
}

void DevmgrConnect::OnClosed()
{
	sr_printf(SR_PRINT_ERROR, "DevmgrConnect::OnClosed this(%p).\n", this);

	m_devMgr_->m_DevmgrThread->Post(m_devMgr_->m_devmpprocess, SRMC::e_sock_rw_error,
		new SRMC::SockErrorData((void *)this));
}

bool DevmgrConnect::SendPktToDevmgrOrNetmp(char* data, uint32_t tdLen, uint16_t cmd_type)
{
	if (!m_tcp_client_conn_)
	{
		sr_printf(SR_PRINT_ERROR, "DevmgrConnect(%p)'s m_tcp_client_conn_ is null.\n", this);
		return false;
	}
	if (m_tcp_client_conn_->IsClosed())
	{
		sr_printf(SR_PRINT_ERROR, "DevmgrConnect(%p)'s m_tcp_client_conn_(%p) is closed.\n", this, m_tcp_client_conn_);
		return false;
	}
	bool send_rslt = m_tcp_client_conn_->SendPduData(cmd_type, data + 8, tdLen - 8);

	return send_rslt;
}
