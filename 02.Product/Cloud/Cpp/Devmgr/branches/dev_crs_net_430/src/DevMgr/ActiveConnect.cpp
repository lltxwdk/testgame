
//#include "device_ser.pb.h"
#include "ActiveConnect.h"
//#include "deviceManager.h"
//#include "sr_tcpheader.h"
#include "DevMgr.h"

AcitiveConnect::AcitiveConnect(ITcpTransLib* tcp_trans_mod, CDevMgr* pdevMgr, DeviceConnMgr* pDevconnMgr)
{
	m_svr_tcp_conn_ = NULL;
	m_svr_tcp_conn_ = tcp_trans_mod->CreateTcpConn(); // 创建tcp主动连接

	m_svr_tcp_conn_obj = new SRProtoConnBase(m_svr_tcp_conn_, this); // 创建事件对象
	m_svr_tcp_conn_obj->Init(DEVMGR_TCP_MAX_BUF_LEN, DEVMGR_TCP_MAX_BUF_LEN);
	m_devMgr_ = pdevMgr;
	m_devconnMgr_ = pDevconnMgr;
}

AcitiveConnect::~AcitiveConnect()
{
	SAFE_DELETE(m_svr_tcp_conn_obj);
	m_svr_tcp_conn_ = NULL;
}

bool AcitiveConnect::Open(const char* remote_ip, unsigned short remote_port)
{
	if (!m_svr_tcp_conn_obj->Connect(remote_ip, remote_port))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "AcitiveConnect open connection failed:%s[%d]\n", remote_ip, remote_port);
		return false;
	}
	sr_printf(SR_LOGLEVEL_DEBUG, "AcitiveConnect open connection success:%s[%d]\n", remote_ip, remote_port);
	return true;
}

bool AcitiveConnect::Close()
{
	//if (!m_svr_tcp_conn_obj->IsClosed())
	//{
	//	m_svr_tcp_conn_obj->Close();
	//}

	m_svr_tcp_conn_obj->Close();

	return true;
}

bool AcitiveConnect::IsClosed()
{
	return m_svr_tcp_conn_obj->IsClosed();
}

uint64_t AcitiveConnect::GetSocketClosedTime()
{
	return m_svr_tcp_conn_obj->GetSocketClosedTime();
}

void AcitiveConnect::OnRecvSRPack(uint16_t cmd_type, char* raw_packet, uint16_t raw_packet_len)
{
	m_devMgr_->OnClientReciveData((void*)this, raw_packet, raw_packet_len);
}

void AcitiveConnect::OnConnected()
{
	//m_srd.m_pSocket = (void *)this;
	//m_devMgr_->m_DevmgrThread->Post(m_devMgr_->m_devmpprocess, SRMC::e_sock_connect_ok,
	//	new SRMC::SockRelateData(m_srd));

	sr_printf(SR_LOGLEVEL_DEBUG, "AcitiveConnect OnConnected this:%p\n", this);

	m_devMgr_->ActiveConnOnConnected((void*)this);
}

void AcitiveConnect::OnClosed()
{
	sr_printf(SR_LOGLEVEL_ERROR, "AcitiveConnect::OnClosed this(%p).\n", this);

	//m_devMgr_->m_DevmgrThread->Post(m_devMgr_->m_devmpprocess, SRMC::e_sock_rw_error,
	//	new SRMC::SockErrorData((void *)this));

	m_devMgr_->ActiveConnOnClosed((void*)this);
}

bool AcitiveConnect::SendPktToDevmgr(char* data, uint32_t tdLen, uint16_t cmd_type)
{
	if (!m_svr_tcp_conn_obj)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "AcitiveConnect(%p)'s m_svr_tcp_conn_obj is null.\n", this);
		return false;
	}
	if (m_svr_tcp_conn_obj->IsClosed())
	{
		sr_printf(SR_LOGLEVEL_ERROR, "AcitiveConnect(%p)'s m_svr_tcp_conn_obj(%p) is closed.\n", this, m_svr_tcp_conn_obj);
		return false;
	}
	bool send_rslt = m_svr_tcp_conn_obj->SendPduData(cmd_type, data + 8, tdLen - 8);

	return send_rslt;
}