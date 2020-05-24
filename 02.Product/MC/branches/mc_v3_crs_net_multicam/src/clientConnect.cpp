#include "clientConnect.h"
#include "terminalMgr.h"

ClientConnect::ClientConnect(ITcpConn* tcp_conn, uint64_t seq_num, TerminalMgr* ptermMgr)
{
	m_client_tcp_conn_ = NULL;
	m_client_tcp_conn_ = new SRProtoConnBase(tcp_conn, this);
	m_termMgr_ = ptermMgr;
	m_remote_ip_.clear();
	m_remote_port_ = 0;
}

ClientConnect::~ClientConnect()
{
	m_remote_ip_.clear();
	m_remote_port_ = 0;
    SAFE_DELETE(m_client_tcp_conn_);
}

bool ClientConnect::Init(const char* remote_ip, uint16_t remote_port)
{
	client_remote_addr_ = INetAddr(remote_ip, remote_port);
	m_remote_ip_ = remote_ip;
	m_remote_port_ = remote_port;
	return m_client_tcp_conn_->Init(CLIENT_TCP_MAX_BUF_LEN, CLIENT_TCP_MAX_BUF_LEN);
}

void ClientConnect::Close()
{
    if (!m_client_tcp_conn_->IsClosed())
    {
        m_client_tcp_conn_->Close();
    }
}

bool ClientConnect::IsClosed()
{
    return m_client_tcp_conn_->IsClosed();
}

uint64_t ClientConnect::GetSocketClosedTime()
{
    return m_client_tcp_conn_->GetSocketClosedTime();
}

void ClientConnect::OnRecvSRPack(uint16_t cmd_type, char* raw_packet, uint16_t raw_packet_len)
{
	m_termMgr_->OnReciveTermData((void*)this, raw_packet, raw_packet_len);
}

void ClientConnect::OnConnected()
{
}

void ClientConnect::OnClosed()
{
	sr_printf(SR_PRINT_INFO, "relaymc ClientConnect::OnClosed this(%p)\n", this);
	m_termMgr_->OnClosed((void*)this);
}

bool ClientConnect::SendPktToClient(char* termData, uint32_t tdLen, uint16_t cmd_type)
{
	if((!m_client_tcp_conn_)||(true == m_client_tcp_conn_->IsClosed()))
		return false;
    bool send_rslt = m_client_tcp_conn_->SendPduData(cmd_type, termData + 8, tdLen - 8);
	return send_rslt;
}
