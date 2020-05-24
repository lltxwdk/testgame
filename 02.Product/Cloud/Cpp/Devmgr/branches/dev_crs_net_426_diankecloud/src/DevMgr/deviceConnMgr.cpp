#include "deviceConnMgr.h"

//DeviceConnMgr::m_devmgrinfomanager = (DevmgrCoonManager*)0;

DeviceConnMgr::DeviceConnMgr(ITcpTransLib* tcp_trans_mod, CDevMgr* devMgr)
{
	tcp_trans_mod_ = tcp_trans_mod;
	dev_seq_num_ = 0;
	m_devMgr_ = devMgr;

	device_connect_map_.Clear();
	get_last_map_.clear();

	tcp_listener_ = NULL;
	
	m_devmgrinfomanager = new SRMC::DevmgrCoonManager();
}

DeviceConnMgr::~DeviceConnMgr()
{
	device_connect_map_.Clear();
	get_last_map_.clear();
}

bool DeviceConnMgr::Init(const char* listen_ip, uint16_t listen_port)
{
	tcp_listener_ = tcp_trans_mod_->CreateTcpListenner(this);
	if (NULL == tcp_listener_)
	{
		return false;
	}
	if (!tcp_listener_->Listen(listen_ip, listen_port))
	{
		return false;
	}
	return true;
}

void DeviceConnMgr::DeviceOnClose(DeviceConnect * pDeviceConnet)
{
	if (pDeviceConnet == NULL)
	{
		return;
	}
	
	sr_printf(SR_LOGLEVEL_NORMAL, " DeviceConnMgr::DeviceOnClose deviceid = %d, pDeviceConnet = %p\n", pDeviceConnet->GetDeviceID(), pDeviceConnet);

	DeviceConnect *device_acc = NULL;
	device_acc = device_connect_map_.Find(pDeviceConnet);

	printf(" DeviceConnMgr::DeviceOnClose Find device_acc = %p \n", device_acc);
	if (device_acc != NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, " DeviceConnMgr::DeviceOnClose device_connect_map_.Find OK!!!!!!!!!!!!\n");

		device_acc->Close();
		device_connect_map_.Delete(pDeviceConnet);
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, " DeviceConnMgr::DeviceOnClose device_connect_map_.Find Error!!!!!!!!!!!!\n");
	}

	//LogINFO("--device size = %d", device_connect_map_.Size());

	sr_printf(SR_LOGLEVEL_NORMAL, "--device size = %d\n", device_connect_map_.Size());
}

void DeviceConnMgr::GetLatestMap(std::map<DeviceConnect *, DeviceConnect *> &result)
{
	//printf(" 111  DeviceConnMgr::GetLatestMap Into SafeMap::GetLatestMap get_last_map_.size = %d\n", get_last_map_.size());
	device_connect_map_.RelayExpireObj(30 * 1000); // 配合device_connect_map_.Delete(devSocket)函数使用，此处5s调用一次，所有延迟删除时间30s不能小于该调用周期5s
	device_connect_map_.GetLatestMap(get_last_map_);
	//printf(" 2222 DeviceConnMgr::GetLatestMap Out SafeMap::GetLatestMap get_last_map_.size = %d\n", get_last_map_.size());
	result = get_last_map_;
	return;
}

void DeviceConnMgr::OnAccept(ITcpConn* new_tcpconn, char* remote_ip, unsigned short remote_port)
{
	INetAddr addr(remote_ip, remote_port);
	sr_printf(SR_LOGLEVEL_NORMAL, "Accept new tcp connection from device:%s, new_tcpconn=%p\n", addr.AsString().c_str(), new_tcpconn);
	DeviceConnect *device_cnct = new DeviceConnect(new_tcpconn, dev_seq_num_++, m_devMgr_);
	device_cnct->Init(remote_ip, remote_port);

	assert(device_connect_map_.Find(device_cnct) == NULL);
	device_connect_map_.Insert(device_cnct, device_cnct);
	sr_printf(SR_LOGLEVEL_NORMAL, "++device size = %d, device_cnct=%p\n", device_connect_map_.Size(), device_cnct);
}

void DeviceConnMgr::OnNetError(int err_code)
{
	assert(false);
}

void DeviceConnMgr::GetLatestMap(std::map<SR_uint32, AcitiveConnect*> &result)
{
	//printf(" 111  DeviceConnMgr::GetLatestMap Into SafeMap::GetLatestMap get_last_map_.size = %d\n", get_last_map_.size());
	svr_connect_map_.RelayExpireObj(30 * 1000); // 配合svr_connect_map_.Delete(devSocket)函数使用，此处5s调用一次，所有延迟删除时间30s不能小于该调用周期5s
	svr_connect_map_.GetLatestMap(get_last_svr_map_);
	//printf(" 2222 DeviceConnMgr::GetLatestMap Out SafeMap::GetLatestMap get_last_map_.size = %d\n", get_last_map_.size());
	result = get_last_svr_map_;
	return;
}

SR_void DeviceConnMgr::ConnectDevmgr()
{
	for (std::list<SRMC::DevmgrSockInfo*>::iterator devs_itor = m_devmgrinfomanager->m_devmgrInfos.begin();
		devs_itor != m_devmgrinfomanager->m_devmgrInfos.end(); ++devs_itor)
	{
		SRMC::DevmgrSockInfo* pdsinfo = *devs_itor;
		if ((NULL == pdsinfo->m_sockptr) && (SRMC::e_DevMgr_sock_init == pdsinfo->m_devsockstat))
		{
			pdsinfo->m_sockptr = ConnectDevmgr(pdsinfo->m_ip.c_str(), pdsinfo->m_port);
			//m_devmgrinfomanager->m_current_dev_connectcnt++;//每连接一次加1
			if (false == isConnecting())
			{
				sr_printf(SR_LOGLEVEL_ERROR, "connect devmgr(ip=%s, port=%d) is failed.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port);
				pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_error;
				pdsinfo->m_sockptr = NULL;
			}
			else
			{
				sr_printf(SR_LOGLEVEL_NORMAL, "connect devmgr(ip=%s, port=%d) is connecting, m_sockptr=%p.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port, pdsinfo->m_sockptr);
				pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_connecting;
				m_devmgrinfomanager->m_current_devmgr_sockptr = pdsinfo->m_sockptr;
				return;
			}
		}
	}

	//暂时没有找到可用的devmgr,定时去连之前连不上的
	for (std::list<SRMC::DevmgrSockInfo*>::iterator devs_itor = m_devmgrinfomanager->m_devmgrInfos.begin();
		devs_itor != m_devmgrinfomanager->m_devmgrInfos.end(); ++devs_itor)
	{
		SRMC::DevmgrSockInfo* pdsinfo = *devs_itor;
		pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_init;
		pdsinfo->m_sockptr = NULL;
	}

	sr_printf(SR_LOGLEVEL_WARNING, "startUp all devmgr is failed, so continue search other devmgr.\n");

	m_devMgr_->createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_connect_devmgr_timer, 0ull);
}

AcitiveConnect* DeviceConnMgr::ConnectDevmgr(const char* ip, int port/*, const SRMC::SockRelateData& prelatedate*/)
{
	//struct in_addr addr;
	//inet_pton(AF_INET, ip, &addr);
	//INetAddr devmgr_addr(ntohl(addr.s_addr), (uint16_t)port);

	AcitiveConnect* pdevmgr_connector = new AcitiveConnect(tcp_trans_mod_, m_devMgr_,this);
	pdevmgr_connector->m_srd.m_socktype = kdevmgr_sock_type;//static const SR_uint32 kdevmgr_sock_type
	SR_uint32 dev_number = m_devNumber++;
	m_is_connect_ok = pdevmgr_connector->Open(ip, (uint16_t)port);
	svr_connect_map_.Insert(dev_number, pdevmgr_connector);
	sr_printf(SR_LOGLEVEL_NORMAL, "ConnectDevmgr ip = %s port = %d [%d:%p]\n", ip, port, dev_number, pdevmgr_connector);
	return pdevmgr_connector;
}

void DeviceConnMgr::onActiveConnSockError(void *pActiveTcp)
{
	AcitiveConnect* pAcitiveConnect = (AcitiveConnect*)pActiveTcp;

	sr_printf(SR_LOGLEVEL_INFO, "onActiveConnSockError, pAcitiveConnect=%p, m_socktype=0x%x\n", pAcitiveConnect, pAcitiveConnect->m_srd.m_socktype);

	if (pAcitiveConnect
		&& pAcitiveConnect->m_srd.m_socktype == kdevmgr_sock_type)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "onActiveConnSockError socketptr=%p is error\n", pAcitiveConnect);

		if (m_devmgrinfomanager->m_current_devmgr_sockptr == pAcitiveConnect)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "onActiveConnSockError devmgr socketptr=%p is error,then connect other devmgr\n", pAcitiveConnect);
			m_devmgrinfomanager->m_current_devmgr_sockptr = NULL;

			for (std::list<SRMC::DevmgrSockInfo*>::iterator devs_itor = m_devmgrinfomanager->m_devmgrInfos.begin();
				devs_itor != m_devmgrinfomanager->m_devmgrInfos.end(); ++devs_itor)
			{
				SRMC::DevmgrSockInfo* pdsinfo = *devs_itor;
				if ((pAcitiveConnect == pdsinfo->m_sockptr) && (SRMC::e_DevMgr_sock_connect_ok == pdsinfo->m_devsockstat))
				{
					pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_error;
					pdsinfo->m_sockptr = NULL;
					break;
				}
			}

			//查找新的devmgr建立连接
			for (std::list<SRMC::DevmgrSockInfo*>::iterator devs_itor = m_devmgrinfomanager->m_devmgrInfos.begin();
				devs_itor != m_devmgrinfomanager->m_devmgrInfos.end(); ++devs_itor)
			{
				SRMC::DevmgrSockInfo* pdsinfo = *devs_itor;
				if ((NULL == pdsinfo->m_sockptr) && (SRMC::e_DevMgr_sock_init == pdsinfo->m_devsockstat))
				{
					pdsinfo->m_sockptr = ConnectDevmgr(pdsinfo->m_ip.c_str(), pdsinfo->m_port);

					if (false == isConnecting())
					{
						sr_printf(SR_LOGLEVEL_ERROR, "connect new devmgr(ip=%s, port=%d) is failed.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port);
						pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_error;
						pdsinfo->m_sockptr = NULL;
					}
					else
					{
						sr_printf(SR_LOGLEVEL_NORMAL, "connect new devmgr(ip=%s, port=%d) is connecting, m_sockptr=%p.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port, pdsinfo->m_sockptr);
						pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_connecting;
						m_devmgrinfomanager->m_current_devmgr_sockptr = pdsinfo->m_sockptr;
						return;
					}
				}
			}
			//暂时没有找到可用的devmgr,定时去连之前连不上的
			for (std::list<SRMC::DevmgrSockInfo*>::iterator devs_itor = m_devmgrinfomanager->m_devmgrInfos.begin();
				devs_itor != m_devmgrinfomanager->m_devmgrInfos.end(); ++devs_itor)
			{
				SRMC::DevmgrSockInfo* pdsinfo = *devs_itor;
				pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_init;
				pdsinfo->m_sockptr = NULL;
			}

			sr_printf(SR_LOGLEVEL_WARNING, "devmgr socketptr=%p is error,and connect all devmgr is failed, so continue search other devmgr.\n", pAcitiveConnect);
			
			m_devMgr_->createTimer(CONNECT_DEVMGR_TIME, SRMC::e_dispoable_timer, e_connect_devmgr_timer, 0ull);
			return;
		}
		else
		{
			sr_printf(SR_LOGLEVEL_WARNING, "onActiveConnSockError socketptr=%p is not equal m_current_devmgr_sockptr=%p\n", pAcitiveConnect, m_devmgrinfomanager->m_current_devmgr_sockptr);
		}
	}
}

void DeviceConnMgr::onActiveConnectSuccess(void *pActiveTcp)
{
	AcitiveConnect* pAcitiveConnect = (AcitiveConnect*)pActiveTcp;

	sr_printf(SR_LOGLEVEL_INFO, "onActiveConnectSuccess, pAcitiveConnect=%p, m_socktype=0x%x\n", pAcitiveConnect, pAcitiveConnect->m_srd.m_socktype);

	if (pAcitiveConnect
		&& pAcitiveConnect->m_srd.m_socktype == kdevmgr_sock_type)
	{
		SR_bool bFindSocket = false;
		for (std::list<SRMC::DevmgrSockInfo*>::iterator devs_itor = m_devmgrinfomanager->m_devmgrInfos.begin();
			devs_itor != m_devmgrinfomanager->m_devmgrInfos.end(); ++devs_itor)
		{
			SRMC::DevmgrSockInfo* pdsinfo = *devs_itor;
			sr_printf(SR_LOGLEVEL_INFO, "onActiveConnectSuccess, pAcitiveConnect=%p, pdsinfo->m_sockptr=%p\n", pAcitiveConnect, pdsinfo->m_sockptr);

			if ((pAcitiveConnect == pdsinfo->m_sockptr))
			{
				bFindSocket = true;
				m_devmgrinfomanager->m_current_dev_stat = DEVMGR_HEART_FLAGS;
				m_devmgrinfomanager->m_current_devmgr_sockptr = pdsinfo->m_sockptr;
				pdsinfo->m_devsockstat = SRMC::e_DevMgr_sock_connect_ok;
				m_devmgrinfomanager->m_current_dev_connectcnt = 0;
				//SRMsgs::ReqRegister mcregister;
				//struct in_addr addr;
				//if(0 == inet_pton(AF_INET,MCCfgInfo::instance()->get_listenip().c_str(), &addr))
				//{
				//	sr_printf(SR_PRINT_ERROR,"%s inet_pton error\n",__FUNCTION__);
				//	return;
				//}
				//mcregister.set_auth_password(MCCfgInfo::instance()->get_register_password());
				//mcregister.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
				//mcregister.set_ip(addr.s_addr);
				//mcregister.set_port(MCCfgInfo::instance()->get_listenport());
				//mcregister.set_cpunums(4);
				//SerialAndSendDevmgr_nobuf(getMsgIdByClassName(ReqRegister), &mcregister);

				//PostToTerThread(e_notify_to_register_devmgr, 0);

				sr_printf(SR_LOGLEVEL_INFO, "devmgr ip = %s port = %d  connnect ok,sockptr=%p,then to register devmgr.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port, pAcitiveConnect);

				m_devMgr_->RegisterToDevmgr();
				break;
			}
		}

		if (bFindSocket == false)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "devmgr sockptr=%p not in devmgr list.\n", pAcitiveConnect);
		}
	}
}

int DeviceConnMgr::CheckActiveConnTimer()
{
	sr_printf(SR_LOGLEVEL_DEBUG, "svr_connect_map_.size = %d\n", svr_connect_map_.Size());

	svr_connect_map_.GetLatestMap(get_last_svr_map_);

	uint64_t timenow = Utils::Time();

	for (std::map<SR_uint32, AcitiveConnect*>::iterator itr = get_last_svr_map_.begin(); itr != get_last_svr_map_.end(); ++itr)
	{
		AcitiveConnect * pSvr = (AcitiveConnect *)(itr->second);
		uint64_t close_time = pSvr->GetSocketClosedTime();
		if (close_time > 0 && (timenow - close_time) > 5 * Utils::kNumMillisecsPerSec)
		{
			pSvr->Close();
			//m_DevmgrThread->Post(m_devmpprocess, SRMC::e_sock_rw_error, new SRMC::SockErrorData((void *)pSvr));

#ifdef WIN32
			sr_printf(SR_LOGLEVEL_DEBUG, "<= 5s %d:%p %I64d:%I64d\n", itr->first, itr->second, timenow, close_time);
#elif defined LINUX
			sr_printf(SR_LOGLEVEL_DEBUG, "<= 5s %d:%p %lld:%lld\n", itr->first, itr->second, timenow, close_time);
#endif
			sr_printf(SR_LOGLEVEL_DEBUG, "svr_connect_map_.Delete %d:%p\n", itr->first, itr->second);
			svr_connect_map_.Delete(itr->first);
			continue;
		}
	}

	svr_connect_map_.RelayExpireObj(30 * 1000);
	return 0;
}
