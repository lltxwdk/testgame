// DevMgr.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <string>
#include "proto_msgid_define.h"
#include "mc_ter.pb.h"
#include "terminalMgr.h"
#include "cbuffer.h"
#include "asyncthread.h"
#include "ini_configuration.h"
#include "sr_msgprocess.h"

TerminalMgr::TerminalMgr(ITcpTransLib* tcp_trans_mod)
{
	tcp_trans_mod_ = tcp_trans_mod;
	m_ter_process = new SRMC::TerMsgProcessThread();
}

TerminalMgr::~TerminalMgr()
{

}

void TerminalMgr::OnAccept(ITcpConn* new_tcpconn, char* remote_ip, unsigned short remote_port)
{
	INetAddr addr(remote_ip, remote_port);
	sr_printf(SR_PRINT_DEBUG, "Accept new tcp connection from relaymc:%s,(%p)\n", addr.AsString().c_str(), new_tcpconn);
	ClientConnect *client_cnct_ptr = new ClientConnect(new_tcpconn, client_seq_num_++, this);
	client_cnct_ptr->Init(remote_ip, remote_port);

	assert(client_connect_map_.Find(client_cnct_ptr) == NULL);
	client_connect_map_.Insert(client_cnct_ptr, client_cnct_ptr);
	sr_printf(SR_PRINT_DEBUG, "++device size = %d, client_cnct_ptr=%p\n", client_connect_map_.Size(), client_cnct_ptr);
}
void TerminalMgr::OnNetError(int err_code)
{
	assert(false);
}

int TerminalMgr::OnTimerPerSecond()
{
    client_connect_map_.GetLatestMap(get_last_map_);
	for (std::map<ClientConnect*, ClientConnect*>::iterator itr = get_last_map_.begin(); itr != get_last_map_.end(); ++itr)
    {
        ClientConnect * term = (ClientConnect *)(itr->second);
        if (!term->IsClosed())
        {
            continue;
        }

        if ((Utils::Time() - term->GetSocketClosedTime()) <= 10 * Utils::kNumMillisecsPerSec)
        {
            continue;
        }

        client_connect_map_.Delete(itr->first);
    }
	
    client_connect_map_.RelayExpireObj(30 * 1000);
    return 0;
}

void TerminalMgr::SaveLicence(char* expringdata, uint32_t maxsrcount, uint32_t maxstdcount, char* cstdexpiringdate, uint32_t maxreccount, char* crecexpiringdate, uint32_t maxlivecount, char* cliveexpiringdate, uint32_t maxternumperconf, uint32_t maxconfnum, uint32_t maxmonitorcount, char* cmonitorexpiringdate, uint32_t maxvoicecount, char* cvoiceexpiringdate, char* myversion)
{
	if (NULL == m_pMainThread)
		return;

	m_pMainThread->Post(m_ter_process, SRMC::e_notify_to_save_licenceinfo, new SRMC::LicenceInfoData(expringdata, maxsrcount, maxstdcount, cstdexpiringdate, maxreccount, crecexpiringdate, maxlivecount, cliveexpiringdate, maxternumperconf, maxconfnum, maxmonitorcount, cmonitorexpiringdate, maxvoicecount, cvoiceexpiringdate, myversion));
}

//void* pclient <-> ClientContent
int TerminalMgr::OnReciveTermData(void* pclient, char* data, uint32_t data_len)//must return how many data Has been copied(recive)
{
	if (m_pMainThread == NULL)
		return 0;

	if (data == NULL || data_len <= 0)
		return 0;

	//if (data_len < sizeof(SRMC::S_SRMsgHeader))
	if (data_len < 8) // sizeof(SRMC::S_SRMsgHeader)
	{
		sr_printf(SR_PRINT_ERROR, " OnReciveTermData data len is too small !!!!\n");
		return 0;
	}

	////////////////////////////////////////////////////////////////////////////
	//char *ss = new char[data_len + 1];
	//memcpy(ss, data, data_len + 1);
	//char lpTemp[1024] = { 0 };
	//for (int i = 0; i < data_len; i++)
	//{
	//	char temp[6] = { 0 };
	//	//sprintf(temp, "0x%02x ", (unsigned char)ss[i]);
	//	sprintf(temp, "%02x ", (unsigned char)ss[i]);
	//	strcat(lpTemp, temp);
	//}
	//sr_printf(SR_PRINT_DEBUG, "----TerminalMgr::OnReciveTermData--->>>%s\n", lpTemp);
	//if (ss != NULL)
	//{
	//	delete ss;
	//}
	////////////////////////////////////////////////////////////////////////////

//	int sockfd = *((int*)(((ClientConnect*)pclient)->GetSocketFd()));
	SRMC::S_SRMsgHeader header;
	header.m_cmdtype = 200;
	//header.m_data_len = data_len;
	//m_pMainThread->Post(m_ter_process, SRMC::e_sock_recv_data, new SRMC::SockTcpData(&header, /*sockfd,*/ pclient, data));
	//return data_len;
	header.m_data_len = data_len-8;
	m_pMainThread->Post(m_ter_process, SRMC::e_sock_recv_data, new SRMC::SockTcpData(&header, /*sockfd,*/ pclient, data+8));
	return data_len-8;
}

void TerminalMgr::OnClosed(void* pclient)
{
	if (NULL == m_pMainThread)
		return;
	if (NULL == pclient)
		return;

	m_pMainThread->Post(m_ter_process,SRMC::e_sock_rw_error,new SRMC::SockErrorData(pclient));
}
bool TerminalMgr::Init()
{
	//�û������߳�
	m_pMainThread = new CAsyncThread();
	m_ter_process->m_selfThread = m_pMainThread;
	m_pMainThread->Start(false);

	m_ter_process->Init();

	//���������߳�
	bool bret = ServerTcpStart(SRMC::MCCfgInfo::instance()->get_listenport());//hening read config

	if (bret)
	{
		sr_printf(SR_PRINT_DEBUG, "TerminalMgr::Init() success\n");
		return true;
	} 
	else
	{
		sr_printf(SR_PRINT_DEBUG, "TerminalMgr::Init() success\n");
		return false;
	}
}

void TerminalMgr::Fini()
{
	printf("CDevMgr::Fini() success\n");

	//�߳��ͷ�
	if (NULL != m_pMainThread)
	{
		m_pMainThread->Stop();
		delete m_pMainThread;
		m_pMainThread = NULL;
	}
	
	return;
}

bool TerminalMgr::ServerTcpStart(const int listenport)
{
	tcp_listener_ = tcp_trans_mod_->CreateTcpListenner(this);
	if (NULL == tcp_listener_)
	{
		return false;
	}
	if (!tcp_listener_->Listen(NULL, listenport))
	{
		return false;
	}
	return true;
}