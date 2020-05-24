#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "proto_msgid_define.h"
#include "deviceManager.h"
#include "cbuffer.h"
#include "asyncthread.h"
#include "sr_msgprocess.h"

DeviceManager::DeviceManager(ITcpTransLib* tcp_trans_mod)
{
	m_pTcp_trans_mod = tcp_trans_mod;
	m_DevmgrThread = new CAsyncThread();
	m_devmpprocess = new SRMC::DevMgrNetMPProcessThread();
	m_devmpprocess->m_selfThread = m_DevmgrThread;
	m_devNumber = 1;
	m_is_connect_ok = false;
}

DeviceManager::~DeviceManager()
{

}

bool DeviceManager::Init()
{
    m_DevmgrThread->Start(false);
	sr_printf(SR_PRINT_NORMAL, "DeviceManager::Init() success\n");

    return true;
}
void DeviceManager::ConnectNetmp(const char* ip, int port, const SRMC::SockRelateData& prelatedate)
{
	struct in_addr addr;
	inet_pton(AF_INET, ip, &addr);
	INetAddr netmp_addr(ntohl(addr.s_addr), (uint16_t)port);

	DevmgrConnect* pconnector = new DevmgrConnect(m_pTcp_trans_mod, this, netmp_addr);
	pconnector->m_srd = prelatedate;
	/*m_is_connect_ok = */pconnector->Open(ip, (uint16_t)port);
	SR_uint32 dev_number = m_devNumber++;
    device_connect_map_.Insert(dev_number, pconnector);
	sr_printf(SR_PRINT_NORMAL, "ConnectNetmp ip = %s port = %d [%d:%p]\n", ip, port, dev_number, pconnector);
    
}

void DeviceManager::ConnectMP(const char* ip, int port, const SRMC::SockRelateData& prelatedate)
{
	struct in_addr addr;
	inet_pton(AF_INET, ip, &addr);
	INetAddr mp_addr(ntohl(addr.s_addr), (uint16_t)port);

	DevmgrConnect* pMpconnector = new DevmgrConnect(m_pTcp_trans_mod, this, mp_addr);
	pMpconnector->m_srd = prelatedate;
	SR_uint32 dev_number = m_devNumber++;
	/*m_is_connect_ok = */pMpconnector->Open(ip, (uint16_t)port);
	device_connect_map_.Insert(dev_number, pMpconnector);
	sr_printf(SR_PRINT_NORMAL, "ConnectMP ip = %s port = %d [%d:%p]\n", ip, port, dev_number, pMpconnector);

}

void DeviceManager::ConnectGW(const char* ip, int port, const SRMC::SockRelateData& prelatedate)
{
	struct in_addr addr;
	inet_pton(AF_INET, ip, &addr);
	INetAddr gw_addr(ntohl(addr.s_addr), (uint16_t)port);

	DevmgrConnect* pGWconnector = new DevmgrConnect(m_pTcp_trans_mod, this, gw_addr);
	pGWconnector->m_srd = prelatedate;
	SR_uint32 dev_number = m_devNumber++;
	/*m_is_connect_ok = */pGWconnector->Open(ip, (uint16_t)port);
	device_connect_map_.Insert(dev_number, pGWconnector);
	sr_printf(SR_PRINT_NORMAL, "ConnectGW ip = %s port = %d [%d:%p]\n", ip, port, dev_number, pGWconnector);

}

void DeviceManager::ConnectCRS(const char* ip, int port, const SRMC::SockRelateData& prelatedate)
{
	struct in_addr addr;
	inet_pton(AF_INET, ip, &addr);
	INetAddr mp_addr(ntohl(addr.s_addr), (uint16_t)port);

	DevmgrConnect* pCRSconnector = new DevmgrConnect(m_pTcp_trans_mod, this, mp_addr);
	pCRSconnector->m_srd = prelatedate;
	SR_uint32 dev_number = m_devNumber++;
	/*m_is_connect_ok = */pCRSconnector->Open(ip, (uint16_t)port);
	device_connect_map_.Insert(dev_number, pCRSconnector);
	sr_printf(SR_PRINT_NORMAL, "ConnectCRS ip = %s port = %d [%d:%p]\n", ip, port, dev_number, pCRSconnector);

}

void DeviceManager::ConnectRelayserver(const char* ip, int port, const SRMC::SockRelateData& prelatedate)
{
	struct in_addr addr;
	inet_pton(AF_INET, ip, &addr);
	INetAddr relayserver_addr(ntohl(addr.s_addr), (uint16_t)port);

	DevmgrConnect* pRSconnector = new DevmgrConnect(m_pTcp_trans_mod, this, relayserver_addr);
	pRSconnector->m_srd = prelatedate;
	SR_uint32 dev_number = m_devNumber++;
	/*m_is_connect_ok = */pRSconnector->Open(ip, (uint16_t)port);
	device_connect_map_.Insert(dev_number, pRSconnector);
	sr_printf(SR_PRINT_NORMAL, "ConnectRelayserver ip = %s port = %d [%d:%p]\n", ip, port, dev_number, pRSconnector);

}

DevmgrConnect* DeviceManager::ConnectDevmgr(const char* ip, int port)
{
	struct in_addr addr;
	inet_pton(AF_INET, ip, &addr);
	INetAddr devmgr_addr(ntohl(addr.s_addr), (uint16_t)port);

	DevmgrConnect* pdevmgr_connector = new DevmgrConnect(m_pTcp_trans_mod, this, devmgr_addr);
	pdevmgr_connector->m_srd.m_socktype = 0xbeafu;//static const SR_uint32 kdevmgr_sock_type
	SR_uint32 dev_number = m_devNumber++;
	m_is_connect_ok = pdevmgr_connector->Open(ip, (uint16_t)port);
    device_connect_map_.Insert(dev_number, pdevmgr_connector);
	sr_printf(SR_PRINT_NORMAL, "ConnectDevmgr ip = %s port = %d [%d:%p]\n", ip, port, dev_number, pdevmgr_connector);
	return pdevmgr_connector;
}

void DeviceManager::Fini()
{
    printf("DeviceManager::Fini() success\n");

    //Ïß³ÌÊÍ·Å
    if (NULL != m_DevmgrThread)
    {
        m_DevmgrThread->Stop();
        delete m_DevmgrThread;
        m_DevmgrThread = NULL;
    }

    return;
}

int DeviceManager::OnReciveDevmgrData(void* pclient, char* data, uint32_t data_len)//must return how many data Has been copied(recive)
{
    if (m_DevmgrThread == NULL)
        return 0;

    if (data == NULL || data_len <= 0)
        return 0;
	
	DevmgrConnect* pDevcon = (DevmgrConnect*)(pclient);

	if (pDevcon == NULL)
	{
		return 0;
	}
	//if (data_len < sizeof(SRMC::S_SRMsgHeader))
	if (data_len < 8) // sizeof(SRMC::S_SRMsgHeader)
	{
		sr_printf(SR_PRINT_ERROR, " OnReciveDevmgrData data len is too small !!!!\n");
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
	//sr_printf(SR_PRINT_DEBUG, "====DeviceManager::OnReciveDevmgrData--->>>%s\n", lpTemp);
	//if (ss != NULL)
	//{
	//	delete ss;
	//}
	////////////////////////////////////////////////////////////////////////////

	SRMC::S_SRMsgHeader header;
	header.m_cmdtype = 202;
	//header.m_data_len = data_len;
	//m_DevmgrThread->Post(m_devmpprocess, SRMC::e_sock_recv_data, new SRMC::SockTcpData(&header, /*sockfd,*/ pclient, data));
	//return data_len;
	header.m_data_len = data_len - 8; //sizeof(SRMC::S_SRMsgHeader)
	m_DevmgrThread->Post(m_devmpprocess, SRMC::e_sock_recv_data, new SRMC::SockTcpData(&header, /*sockfd,*/ pclient, data+8));
    return data_len-8;
}

int DeviceManager::OnTimerPerSecond()
{
    //sr_printf(SR_PRINT_DEBUG, "device_connect_map_.size = %d\n", device_connect_map_.Size());
    device_connect_map_.GetLatestMap(get_last_device_map_);
    uint64_t timenow = Utils::Time();
	/*
	for (std::map<SR_uint32, DevmgrConnect*>::iterator itr = get_last_device_map_.begin(); itr != get_last_device_map_.end(); ++itr)
    {
        DevmgrConnect * device = (DevmgrConnect *)(itr->second);
        if (!device->IsClosed())
        {
            //sr_printf(SR_PRINT_DEBUG, "!IsClosed %x:%x\n", itr->first, itr->second);
            continue;
        }

        if ((timenow - device->GetSocketClosedTime()) <= 10 * Utils::kNumMillisecsPerSec)
        {
#ifdef WIN32
            sr_printf(SR_PRINT_DEBUG, "<= 10s %d:%x %I64d:%I64d\n", itr->first, itr->second, timenow, device->GetSocketClosedTime());
#elif defined LINUX
            sr_printf(SR_PRINT_DEBUG, "<= 10s %d:%x %lld:%lld\n", itr->first, itr->second, timenow, device->GetSocketClosedTime());
#endif
            continue;
        }
        //sr_printf(SR_PRINT_DEBUG, "device_connect_map_.Delete %d:%x\n", itr->first, itr->second);
        device_connect_map_.Delete(itr->first);
    }
	*/

	for (std::map<SR_uint32, DevmgrConnect*>::iterator itr = get_last_device_map_.begin(); itr != get_last_device_map_.end(); ++itr)
	{
		DevmgrConnect * device = (DevmgrConnect *)(itr->second);
		uint64_t close_time = device->GetSocketClosedTime();
		if (close_time > 0 && (timenow - close_time) > 5 * Utils::kNumMillisecsPerSec)
		{
			device->Close();
			m_DevmgrThread->Post(m_devmpprocess, SRMC::e_sock_rw_error, new SRMC::SockErrorData((void *)device));

#ifdef WIN32
			sr_printf(SR_PRINT_DEBUG, "<= 5s %d:%p %I64d:%I64d\n", itr->first, itr->second, timenow, close_time);
#elif defined LINUX
			sr_printf(SR_PRINT_DEBUG, "<= 5s %d:%p %lld:%lld\n", itr->first, itr->second, timenow, close_time);
#endif
			sr_printf(SR_PRINT_DEBUG, "device_connect_map_.Delete %d:%p\n", itr->first, itr->second);
			device_connect_map_.Delete(itr->first);
			continue;
		}
	}

    device_connect_map_.RelayExpireObj(30 * 1000);
    return 0;
}

void DeviceManager::startUp()
{
	if (NULL == m_devmpprocess)
		return;

	m_DevmgrThread->Post(m_devmpprocess, SRMC::e_notify_to_startup);
	sr_printf(SR_PRINT_NORMAL, "DeviceManager::startUp() Post startup msg to DevMgrNetMPProcessThread success\n");
}