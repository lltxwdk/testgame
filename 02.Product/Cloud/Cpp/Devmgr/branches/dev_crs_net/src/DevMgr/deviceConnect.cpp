#include "deviceConnect.h"
#include "DevMgr.h"

DeviceConnect::DeviceConnect(ITcpConn* tcp_conn, uint64_t seq_num, CDevMgr* devMgr)
{
	m_device_id = 0;
	//SAFE_DELETE(m_dev_tcp_conn_);
	m_dev_tcp_conn_ = NULL;
	m_dev_tcp_conn_ = new SRProtoConnBase(tcp_conn, this);
    m_devMgr_ = devMgr;

	//dev_seq_ = seq_num;

	m_remote_ip_.clear();
	m_remote_port_ = 0;
}

DeviceConnect::~DeviceConnect()
{
	m_device_id = 0;

	m_remote_ip_.clear();
	m_remote_port_ = 0;

    SAFE_DELETE(m_dev_tcp_conn_);
}

bool DeviceConnect::Init(const char* remote_ip, uint16_t remote_port)
{
	dev_remote_addr_ = INetAddr(remote_ip, remote_port);

	m_remote_ip_ = remote_ip;
	m_remote_port_ = remote_port;

	return m_dev_tcp_conn_->Init(DEVMGR_TCP_MAX_BUF_LEN, DEVMGR_TCP_MAX_BUF_LEN);
}

void DeviceConnect::Close()
{
	if (m_dev_tcp_conn_ == NULL)
	{
		return;
	}

    if (!m_dev_tcp_conn_->IsClosed())
    {
        m_dev_tcp_conn_->Close();
    }
}

bool DeviceConnect::IsClosed()
{
	if (m_dev_tcp_conn_ == NULL)
	{
		return false;
	}

    return m_dev_tcp_conn_->IsClosed();
}

uint64_t DeviceConnect::GetSocketClosedTime()
{
	if (m_dev_tcp_conn_ == NULL)
	{
		return false;
	}
    return m_dev_tcp_conn_->GetSocketClosedTime();
}

void DeviceConnect::OnRecvSRPack(uint16_t cmd_type, char* raw_packet, uint16_t raw_packet_len)
{

	m_devMgr_->OnServerReciveData((void*)this, raw_packet, raw_packet_len);

}

void DeviceConnect::OnConnected()
{
}

void DeviceConnect::OnClosed()
{
	m_devMgr_->OnClosed((void*)this);
}

bool DeviceConnect::SendPktToDevConnection(char* termData, uint32_t tdLen, uint16_t cmd_type)
{
	if (!m_dev_tcp_conn_)
	{
		LogERR("DeviceConnect(%p)'s m_dev_tcp_conn_ is null", this);
		return false;
	}
	if (m_dev_tcp_conn_->IsClosed())
	{
		LogERR("DeviceConnect(%p)'s m_dev_tcp_conn_=%p is closed", this, m_dev_tcp_conn_);
		return false;
	}

    bool send_ret = m_dev_tcp_conn_->SendPduData(cmd_type, termData + 8, tdLen - 8);
	return send_ret;
}
