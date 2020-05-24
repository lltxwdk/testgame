#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include "sr_msgprocess.h"
#include "sr_queuemsgid.h"
#include "proto_msgid_define.h"

#include "device_ser.pb.h"
#include "mc_netmp.pb.h"
#include "mc_ter.pb.h"
#include "mc_mp.pb.h"
#include "mc_gw.pb.h"
#include "mc_crs.pb.h"
#include "mc_relayserver.pb.h"

#include "deviceManager.h"
#include "timer_manager.h"

#ifdef _WIN32
#define snprintf _snprintf
#endif
namespace SRMC{

static const SR_uint32 kdevmgr_sock_type = 0xbeafu;
static const SR_uint32 knetmp_sock_type = 0xabcdu;
static const SR_uint32 kmp_sock_type = 0xcdeau;
static const SR_uint32 kgw_sock_type = 0xecadu;
static const SR_uint32 kcrs_sock_type = 0xdabcu;
static const SR_uint32 krelayserver_sock_type = 0xdacbu;

#define TOLERANCE_TIME 20

SR_void SerialAndSend(SR_void* sockptr,SR_uint32 proto_msguid,const google::protobuf::Message* msg)
{
	if (NULL == sockptr)
	{
		sr_printf(SR_PRINT_ERROR, "%s,%d,  sockptr == NULL, msg: %s(0x%x)\n", __FUNCTION__, __LINE__, msg->GetTypeName().c_str(), proto_msguid);
		//sr_printf(SR_PRINT_ERROR, "%s,%d,  sockptr == NULL \n", __FUNCTION__, __LINE__);
		return;
	}

	SR_uint8 buf[1024*300] = { 0 };//���ڴ����Ҫ�ȷ��͵ײ�socket����Ŀ���ڴ�ҪС,�����Ǹ��ڴ��п���Ҫ���ע����ϢЯ�����л��鼰�ն���Ϣ,���Բ��ܷ���̫С
	msg->SerializePartialToArray(buf + sizeof(S_SRMsgHeader) + sizeof(S_ProtoMsgHeader), sizeof(buf));

	S_ProtoMsgHeader protoheader;
	protoheader.m_msguid = proto_msguid;
	protoheader.m_msglen = msg->ByteSize();
	
	//assert(1024*300 < (protoheader.m_msglen + sizeof(S_ProtoMsgHeader) + sizeof(S_SRMsgHeader)));

	packProtoMsgHeader(&protoheader, buf + sizeof(S_SRMsgHeader));
	SR_int32 sendlen = protoheader.m_msglen + sizeof(S_ProtoMsgHeader) + sizeof(S_SRMsgHeader);

	uint16_t cmd_type = SRMSG_CMDTYPE_NETMP_MC;
	if (Msg_ID_DEVICE_SERVER_BASE_1 == (Msg_ID_DEVICE_SERVER_BASE_1 & proto_msguid))
		cmd_type = SRMSG_CMDTYPE_DevMgr_MC;
	SR_bool sendrslt = ((DevmgrConnect*)sockptr)->SendPktToDevmgrOrNetmp((char*)buf, sendlen, cmd_type); // buf����S_SRMsgHeader
	//if((SRMsgId_IndsertodevHeart == proto_msguid)||
	//			(SRMsgId_ReqGetDeviceInfo == proto_msguid)||
	//			(SRMsgId_IndNetMPHeartBeatOfMC == proto_msguid))
	//	return;

	if (Msg_ID_MC_RELAYSERVER_BASE_1 == (Msg_ID_MC_RELAYSERVER_BASE_1 & proto_msguid))
	{
		if (SRMsgId_IndMCHeartBeatToRelayserver == proto_msguid)
		{
			sr_printf(SR_PRINT_DEBUG, "mc->relayserver,sockptr=%p,send rslt=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
		else
		{
			sr_printf(SR_PRINT_INFO, "mc->relayserver,sockptr=%p,send rslt=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
	}
	else if (Msg_ID_DEVICE_SERVER_BASE_1 == (Msg_ID_DEVICE_SERVER_BASE_1 & proto_msguid))
	{
		if ((SRMsgId_IndsertodevHeart == proto_msguid) 
			||(SRMsgId_ReqGetDeviceInfo == proto_msguid))
		{
			sr_printf(SR_PRINT_DEBUG, "mc->devmgr,sockptr=%p,send rslt=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
		else
		{
			sr_printf(SR_PRINT_INFO, "mc->devmgr,sockptr=%p,send rslt=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
	}
	else if (Msg_ID_MC_CRS_BASE_1 == (Msg_ID_MC_CRS_BASE_1 & proto_msguid)) // �ŵ�else if (Msg_ID_MC_NETMP_BASE_1 == (Msg_ID_MC_NETMP_BASE_1 & proto_msguid))��֧ǰ,��Ȼ�ᱻnetmp��Ϣ���˵�,��ӡ����netmp����Ϣ
	{
		if (SRMsgId_IndMCHeartBeatToCRS == proto_msguid)
		{
			sr_printf(SR_PRINT_DEBUG, "mc->crs,sockptr=%p,send buflen=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
		else
		{
			sr_printf(SR_PRINT_INFO, "mc->crs,sockptr=%p,send buflen=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
	}
	else if (Msg_ID_MC_NETMP_BASE_1 == (Msg_ID_MC_NETMP_BASE_1 & proto_msguid))
	{
		if (SRMsgId_IndNetMPHeartBeatOfMC == proto_msguid)
		{
			sr_printf(SR_PRINT_DEBUG, "mc->netmp,sockptr=%p,send rslt=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		} 
		else
		{
			sr_printf(SR_PRINT_INFO, "mc->netmp,sockptr=%p,send rslt=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
	}
	else if (Msg_ID_MC_GW_BASE_1 == (Msg_ID_MC_GW_BASE_1 & proto_msguid)) // �ŵ�else if (Msg_ID_MC_MP_BASE_1 == (Msg_ID_MC_MP_BASE_1 & proto_msguid))��֧ǰ,��Ȼ�ᱻmp��Ϣ���˵�,��ӡ����mp����Ϣ
	{
		if (SRMsgId_IndMCHeartBeatToGW == proto_msguid)
		{
			sr_printf(SR_PRINT_DEBUG, "mc->gw,sockptr=%p,send buflen=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
		else
		{
			sr_printf(SR_PRINT_INFO, "mc->gw,sockptr=%p,send buflen=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
	}
	else if (Msg_ID_MC_MP_BASE_1 == (Msg_ID_MC_MP_BASE_1 & proto_msguid))
	{
		if (SRMsgId_IndMCHeartBeatToMP == proto_msguid)
		{
			sr_printf(SR_PRINT_DEBUG, "mc->mp,sockptr=%p,send buflen=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		} 
		else
		{
			sr_printf(SR_PRINT_INFO, "mc->mp,sockptr=%p,send buflen=%d(sendlen=%d),%s--\n%s\n", sockptr, sendrslt, sendlen, msg->GetTypeName().c_str(), msg->Utf8DebugString().c_str());
		}
	}
}

SR_void SerialAndSendDevmgr_nobuf(SR_uint32 proto_msguid,const google::protobuf::Message* msg)
{
	SerialAndSend(DevMgrNetMPProcessThread::m_netmpinfomanager->m_current_devmgr_sockptr,proto_msguid,msg);
}

SR_void SerialAndSendDevmgr_buffered(SR_uint32 proto_msguid,const google::protobuf::Message* msg)
{
	if(IS_DEVMGR_UNREGISTER())
	{
		if(DevMgrNetMPProcessThread::m_netmpinfomanager->m_buffered_dev.size() > MAX_BUFFER_MSG_SIZE_DEVICE)
			sr_printf(SR_PRINT_ERROR,"devmgr sendbuffer overflow\n");
		else{
			google::protobuf::Message* pbufferedmsg = msg->New();
			pbufferedmsg->CopyFrom(*msg);
			BufferedProtoMsgPair* pbuf__ = new BufferedProtoMsgPair(proto_msguid,pbufferedmsg);
			DevMgrNetMPProcessThread::m_netmpinfomanager->m_buffered_dev.push_back(pbuf__);
		}
	}
	else
	{
		SerialAndSend(GET_CURRENT_DEVMGR(),proto_msguid,msg);
	}
}

//SR_void SerialAndSendToCCS_nobuf(SR_uint32 proto_msguid, const google::protobuf::Message* msg)
//{
//	parseAndSendTerMsgEx(DevMgrNetMPProcessThread::m_terprocess->m_current_ccs_sockptr, DevMgrNetMPProcessThread::m_terprocess->m_ccsheadcontex, proto_msguid, msg);
//}
//
//SR_void SerialAndSendToCCS_buffered(SR_uint32 proto_msguid, const google::protobuf::Message* msg)
//{
//}

NetMPInfoManager* DevMgrNetMPProcessThread::m_netmpinfomanager = (NetMPInfoManager*)0;
MPInfoManager* DevMgrNetMPProcessThread::m_mpinfomanager = (MPInfoManager*)0;
GWInfoManager* DevMgrNetMPProcessThread::m_gwinfomanager = (GWInfoManager*)0;
CRSInfoManager* DevMgrNetMPProcessThread::m_crsinfomanager = (CRSInfoManager*)0;
SRSInfoManager* DevMgrNetMPProcessThread::m_srsinfomanager = (SRSInfoManager*)0;
RelayServerInfoManager* DevMgrNetMPProcessThread::m_relaysvrinfomgr = (RelayServerInfoManager*)0;
RelayMcInfoManager* DevMgrNetMPProcessThread::m_relaymcinfomgr = (RelayMcInfoManager*)0;
DevMgrNetMPProcessThread::DevMgrNetMPProcessThread()
{
	m_deviceMan = (DeviceManager*)0;
	if((NetMPInfoManager*)0 == m_netmpinfomanager)
		m_netmpinfomanager = new NetMPInfoManager();

	if ((MPInfoManager*)0 == m_mpinfomanager)
		m_mpinfomanager = new MPInfoManager();

	if ((GWInfoManager*)0 == m_gwinfomanager)
		m_gwinfomanager = new GWInfoManager();

	if ((CRSInfoManager*)0 == m_crsinfomanager)
		m_crsinfomanager = new CRSInfoManager();

	if ((SRSInfoManager*)0 == m_srsinfomanager)
		m_srsinfomanager = new SRSInfoManager();

	if ((RelayServerInfoManager*)0 == m_relaysvrinfomgr)
		m_relaysvrinfomgr = new RelayServerInfoManager();

	if ((RelayMcInfoManager*)0 == m_relaymcinfomgr)
		m_relaymcinfomgr = new RelayMcInfoManager();

	m_periodtimer_updatenetmp = (void*)0;
	m_periodtimer_hearttodevmgr = (void*)0;
	m_periodtimer_hearttonetmp = (void*)0;
	m_terthreadhandle = (CAsyncThread*)0;
	m_terprocess = (TerMsgProcessThread*)0;
	m_check_devmgr_heart_timer = (void*)0;

	m_periodtimer_updatemp = (void*)0;
	m_periodtimer_hearttomp = (void*)0;

	m_periodtimer_updategw = (void*)0;
	m_periodtimer_hearttogw= (void*)0;

	m_periodtimer_updatecrs = (void*)0;
	m_periodtimer_hearttocrs = (void*)0;

	m_periodtimer_updatesrs = (void*)0;
	m_periodtimer_updatesyscurload = (void*)0;

	m_periodtimer_updaterelayserver = (void*)0; 
	m_periodtimer_hearttorelayserver = (void*)0;

	m_periodtimer_updaterelaymc = (void*)0;
	m_periodtimer_checkrelaymctimeout = (void*)0;
	
	m_re_register_timer_list.clear();
	m_connect_devmgr_timer_list.clear();

	//m_isRegisterOK = false;

	//��ȡdevmgr�����ļ�
	DevSockInfo* pdsinfo = new DevSockInfo();
	pdsinfo->m_ip = MCCfgInfo::instance()->get_devmgrip();
	pdsinfo->m_port = MCCfgInfo::instance()->get_devmgrport();
	m_netmpinfomanager->m_devmgrInfos.insert(pdsinfo);

	SRMC::Configuration mc_cfg(MCCfgInfo::instance()->get_fileName().c_str());
	for(int i = 1;i <= MAX_DEVMGR_NUMS;++i)
	{
		char dev_key[48] = {0};
		snprintf(dev_key,sizeof(dev_key)-1,"device_%d",i);
		std::string ipstr = mc_cfg.getStrValue(dev_key,"deviceip","null");
		if(ipstr == "null")
		{
			break;
		}
		else
		{
			DevSockInfo* pdsinfo = new DevSockInfo();
			pdsinfo->m_ip = ipstr;
			pdsinfo->m_port = (int)mc_cfg.getInt64Value(dev_key,"deviceport",0);
			m_netmpinfomanager->m_devmgrInfos.insert(pdsinfo);			
		}
	}
	
	if(0u == m_netmpinfomanager->m_devmgrInfos.size())
	{
		sr_printf(SR_PRINT_ERROR,"plz config DevMgr ip & port in %s,donot support cfg at runtime now\n",
			MCCfgInfo::instance()->get_fileName().c_str());
		exit(1);
	}
}
SR_void DevMgrNetMPProcessThread::updateNetmpInfoPeric()
{
	//TODO: ����ʹ��update ���
	if (MCCfgInfo::instance()->get_mcdeviceid() != 0) // mcע��ɹ��ŷ��ͣ����ⷢ������deviceidΪ0������devmgr��������
	{
		SRMsgs::ReqGetDeviceInfo reqgdi;
		reqgdi.set_token(MCCfgInfo::instance()->get_mctoken());
		reqgdi.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
		reqgdi.set_devnums(10);
		reqgdi.set_get_svr_type(DEVICE_SERVER::DEVICE_NETMP);
		reqgdi.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
		SerialAndSendDevmgr_nobuf(getMsgIdByClassName(ReqGetDeviceInfo), &reqgdi);
	}
	
	if(0u == m_netmpinfomanager->m_netmpinfo.size())
	{
		//sr_printf(SR_PRINT_INFO,"updateNetmpInfoPeric have no netmp.\n");
		return;
	}
	else
	{
		//�Ѿ��ﵽ�������
		if(m_netmpinfomanager->m_connected_nemtps.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp())
		{
			sr_printf(SR_PRINT_INFO,"updateNetmpInfoPeric have max netmp_tcp_connect[cur:%d, cfg:%d],so do nothing.\n",
				m_netmpinfomanager->m_connected_nemtps.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
			return;
		}

		if(m_netmpinfomanager->m_idle_netmps.size() >= MCCfgInfo::instance()->get_idle_netmp_nums())
		{
			sr_printf(SR_PRINT_INFO,"updateNetmpInfoPeric m_idle_netmps=%u >= cfg_idle_nums=%u,so do nothing.\n",
				m_netmpinfomanager->m_idle_netmps.size(),MCCfgInfo::instance()->get_idle_netmp_nums());
			return;
		}	
		//����Ƿ���Ҫ�½�����
		SR_uint32 connect_nums = 1u;
		for(std::map<SR_uint32,NetMPInfo*>::iterator netmpitor = m_netmpinfomanager->m_netmpinfo.begin();
			netmpitor != m_netmpinfomanager->m_netmpinfo.end();netmpitor++)
		{
			NetMPInfo* pnetinfo = netmpitor->second;
			if(pnetinfo->m_connectionstate != e_Netmp_sock_inite)
			{
				sr_printf(SR_PRINT_INFO, "updateNetmpInfoPeric pnetinfo->m_netmpdeviceid=%u,m_connectionstate=%u(not inite), continue search netmp.\n", pnetinfo->m_netmpdeviceid, pnetinfo->m_connectionstate);
				continue;
			}	
			if(connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums())
			{
				sr_printf(SR_PRINT_WARN,"updateNetmpInfoPeric connect_nums=%u > cfg_idle_nums=%u,so do nothing.\n",
					connect_nums,MCCfgInfo::instance()->get_idle_netmp_nums());
				break;
			}	
			sr_printf(SR_PRINT_INFO, "updateNetmpInfoPeric idle_netmps=%lu,cfg_idle_netmps=%u,connected_netmps=%lu\n",
				m_netmpinfomanager->m_idle_netmps.size(),MCCfgInfo::instance()->get_idle_netmp_nums(),
				m_netmpinfomanager->m_connected_nemtps.size());
			connect_nums++;
			
			SR_char IPdotdec[40] = {0}; //��ŵ��ʮ����IP��ַ
			struct in_addr s; // IPv4��ַ�ṹ��
			s.s_addr = pnetinfo->m_ip;
			inet_ntop(AF_INET,(void *)&s, IPdotdec, sizeof(IPdotdec));
			sr_printf(SR_PRINT_INFO,"updateNetmpInfoPeric check connect netmp(%u)(%s,%u,%u)\n",pnetinfo->m_ip,IPdotdec,pnetinfo->m_port,pnetinfo->m_netmpdeviceid);
			SockRelateData srd(knetmp_sock_type,pnetinfo->m_netmpdeviceid);
			pnetinfo->m_connectionstate = e_Netmp_sock_connecting;
			if (m_deviceMan)
				m_deviceMan->ConnectNetmp(IPdotdec,pnetinfo->m_port,srd);

			sr_printf(SR_PRINT_INFO, "updateNetmpInfoPeric check connect netmp(%u)(%s,%u,%u) is connecting.....\n", pnetinfo->m_ip, IPdotdec, pnetinfo->m_port, pnetinfo->m_netmpdeviceid);
		}
	}
	
}

SR_void DevMgrNetMPProcessThread::updateMPInfoPeric()
{
	//TODO: ����ʹ��update ���
	if (MCCfgInfo::instance()->get_mcdeviceid() != 0) // mcע��ɹ��ŷ��ͣ����ⷢ������deviceidΪ0������devmgr��������
	{
		SRMsgs::ReqGetDeviceInfo reqgdi;
		reqgdi.set_token(MCCfgInfo::instance()->get_mctoken());
		reqgdi.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
		reqgdi.set_devnums(10);
		reqgdi.set_get_svr_type(DEVICE_SERVER::DEVICE_MP);//��ȡMP�豸��Ϣ
		reqgdi.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
		SerialAndSendDevmgr_nobuf(getMsgIdByClassName(ReqGetDeviceInfo), &reqgdi);
	}
	
	if (0u == m_mpinfomanager->m_mpinfo.size())
	{
		//sr_printf(SR_PRINT_INFO,"updateMPInfoPeric have no netmp\n");
		return;
	}
	else
	{
		//�Ѿ��ﵽ�������
		if (m_mpinfomanager->m_connected_mps.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp()) // ������chen songhua������MP��ʱ������netmp��������
		{
			sr_printf(SR_PRINT_INFO, "updateMPInfoPeric have max mp_tcp_connect[cur:%d, cfg:%d],so do nothing.\n",
				m_mpinfomanager->m_connected_mps.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
			return;
		}

		if (m_mpinfomanager->m_idle_mps.size() >= MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
		{
			sr_printf(SR_PRINT_INFO,"updateMPInfoPeric m_idle_mps=%u >= cfg_idle_nums=%u,so do nothing.\n",
				m_mpinfomanager->m_idle_mps.size(),MCCfgInfo::instance()->get_idle_netmp_nums());
			return;
		}
		//����Ƿ���Ҫ�½�����
		SR_uint32 connect_nums = 1u;
		for (std::map<SR_uint32, MPInfo*>::iterator mpitor = m_mpinfomanager->m_mpinfo.begin();
			mpitor != m_mpinfomanager->m_mpinfo.end(); mpitor++)
		{
			MPInfo* pMpinfo = mpitor->second;
			if (pMpinfo->m_connectionstate != e_Mp_sock_inite)
			{
				sr_printf(SR_PRINT_INFO, "updateMPInfoPeric pMpinfo->m_mpdeviceid=%u,m_connectionstate=%u, not inite state continue search.\n", pMpinfo->m_mpdeviceid, pMpinfo->m_connectionstate);
				continue;
			}
			if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
			{
				sr_printf(SR_PRINT_WARN, "updateMPInfoPeric connect_nums=%u > cfg_idle_nums=%u,so do nothing.\n", connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
				break;
			}
			sr_printf(SR_PRINT_INFO, "updateMPInfoPeric idle_mps=%lu,cfg_idle_mps=%u,connected_mps=%lu\n",
				m_mpinfomanager->m_idle_mps.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
				m_mpinfomanager->m_connected_mps.size());
			connect_nums++;

			SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
			struct in_addr s; // IPv4��ַ�ṹ��
			s.s_addr = pMpinfo->m_ip;
			inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
			sr_printf(SR_PRINT_INFO, "updateMPInfoPeric next connect mp(%u)(%s,%u,%u)\n", pMpinfo->m_ip, IPdotdec, pMpinfo->m_port, pMpinfo->m_mpdeviceid);
			SockRelateData srd(kmp_sock_type, pMpinfo->m_mpdeviceid);
			pMpinfo->m_connectionstate = e_Mp_sock_connecting;
			if (m_deviceMan)
				m_deviceMan->ConnectMP(IPdotdec, pMpinfo->m_port, srd);

			sr_printf(SR_PRINT_INFO, "updateMPInfoPeric new mp(%u)(%s,%u,%u) is connecting.....\n", pMpinfo->m_ip, IPdotdec, pMpinfo->m_port, pMpinfo->m_mpdeviceid);
		}
	}

}

SR_void DevMgrNetMPProcessThread::updateGWInfoPeric()
{
	//TODO: ����ʹ��update ���
	if (MCCfgInfo::instance()->get_mcdeviceid() != 0) // mcע��ɹ��ŷ��ͣ����ⷢ������deviceidΪ0������devmgr��������
	{
		SRMsgs::ReqGetDeviceInfo reqgdi;
		reqgdi.set_token(MCCfgInfo::instance()->get_mctoken());
		reqgdi.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
		reqgdi.set_devnums(10);
		reqgdi.set_get_svr_type(DEVICE_SERVER::DEVICE_GW);//��ȡGW�豸��Ϣ
		reqgdi.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
		SerialAndSendDevmgr_nobuf(getMsgIdByClassName(ReqGetDeviceInfo), &reqgdi);
	}

	if (0u == m_gwinfomanager->m_gwinfo.size())
	{
		//sr_printf(SR_PRINT_INFO,"updateMPInfoPeric have no netmp\n");
		return;
	}
	else
	{
		//�Ѿ��ﵽ�������
		if (m_gwinfomanager->m_connected_gws.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp()) // ������chen songhua������MP��ʱ������netmp��������
		{
			sr_printf(SR_PRINT_INFO,"updateGWInfoPeric have max gw_tcp_connect[cur:%d, cfg:%d],so do nothing.\n",
				m_gwinfomanager->m_connected_gws.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
			return;
		}

		if (m_gwinfomanager->m_idle_gws.size() >= MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
		{
			sr_printf(SR_PRINT_INFO,"updateGWInfoPeric m_idle_mps=%u >= cfg_idle_nums=%u,so do nothing.\n",
				m_mpinfomanager->m_idle_mps.size(),MCCfgInfo::instance()->get_idle_netmp_nums());
			return;
		}
		//����Ƿ���Ҫ�½�����
		SR_uint32 connect_nums = 1u;
		for (std::map<SR_uint32, GWInfo*>::iterator gwitor = m_gwinfomanager->m_gwinfo.begin();
			gwitor != m_gwinfomanager->m_gwinfo.end(); gwitor++)
		{
			GWInfo* pGWinfo = gwitor->second;
			if (pGWinfo->m_connectionstate != e_GW_sock_inite)
			{
				sr_printf(SR_PRINT_INFO, "updateGWInfoPeric pGWinfo->m_gwdeviceid=%u,m_connectionstate=%u, not inite state continue search.\n", pGWinfo->m_gwdeviceid, pGWinfo->m_connectionstate);
				continue;
			}
			if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
			{
				sr_printf(SR_PRINT_WARN, "updateGWInfoPeric connect_nums=%u > cfg_idle_nums=%u,so do nothing.\n", 
					connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
				break;
			}
			sr_printf(SR_PRINT_INFO, "updateGWInfoPeric idle_gws=%lu,cfg_idle_gws=%u,connected_gws=%lu\n",
				m_gwinfomanager->m_idle_gws.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
				m_gwinfomanager->m_connected_gws.size());
			connect_nums++;

			SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
			struct in_addr s; // IPv4��ַ�ṹ��
			s.s_addr = pGWinfo->m_ip;
			inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
			sr_printf(SR_PRINT_INFO, "updateGWInfoPeric check connect gw(%u)(%s,%u,%u)\n", pGWinfo->m_ip, IPdotdec, pGWinfo->m_port, pGWinfo->m_gwdeviceid);
			SockRelateData srd(kgw_sock_type, pGWinfo->m_gwdeviceid);
			pGWinfo->m_connectionstate = e_Mp_sock_connecting;
			if (m_deviceMan)
				m_deviceMan->ConnectGW(IPdotdec, pGWinfo->m_port, srd);

			sr_printf(SR_PRINT_INFO, "updateGWInfoPeric check connect gw(%u)(%s,%u,%u) is connecting.....\n", pGWinfo->m_ip, IPdotdec, pGWinfo->m_port, pGWinfo->m_gwdeviceid);
		}
	}

}

SR_void DevMgrNetMPProcessThread::updateCRSInfoPeric()
{
	if (MCCfgInfo::instance()->get_mcdeviceid() != 0) // mcע��ɹ��ŷ��ͣ����ⷢ������deviceidΪ0������devmgr��������
	{
		SRMsgs::ReqGetDeviceInfo reqgdi;
		reqgdi.set_token(MCCfgInfo::instance()->get_mctoken());
		reqgdi.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
		reqgdi.set_devnums(10);
		reqgdi.set_get_svr_type(DEVICE_SERVER::DEVICE_CRS);//��ȡCRS�豸��Ϣ
		reqgdi.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
		SerialAndSendDevmgr_nobuf(getMsgIdByClassName(ReqGetDeviceInfo), &reqgdi);
	}

	if (0u == m_crsinfomanager->m_crsinfo.size())
	{
		//sr_printf(SR_PRINT_INFO,"updateCRSInfoPeric have no crs\n");
		return;
	}
	else
	{
		//�Ѿ��ﵽ�������
		if (m_crsinfomanager->m_connected_crss.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp()) // ������chen songhua������CRS��ʱ������netmp��������
		{
			sr_printf(SR_PRINT_INFO, "updateCRSInfoPeric have max crs_tcp_connect[cur:%d, cfg:%d],so do nothing.\n",
				m_crsinfomanager->m_connected_crss.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
			return;
		}

		if (m_crsinfomanager->m_idle_crss.size() >= MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
		{
			sr_printf(SR_PRINT_INFO, "updateCRSInfoPeric m_idle_crss=%u >= cfg_idle_nums=%u,so do nothing.\n",
				m_crsinfomanager->m_idle_crss.size(), MCCfgInfo::instance()->get_idle_netmp_nums());
			return;
		}
		//����Ƿ���Ҫ�½�����
		SR_uint32 connect_nums = 1u;
		for (std::map<SR_uint32, CRSInfo*>::iterator crsitor = m_crsinfomanager->m_crsinfo.begin();
			crsitor != m_crsinfomanager->m_crsinfo.end(); crsitor++)
		{
			CRSInfo* pCRSinfo = crsitor->second;
			if (pCRSinfo->m_connectionstate != e_dev_sock_inite)
			{
				sr_printf(SR_PRINT_INFO, "updateCRSInfoPeric pCRSinfo->m_crsdeviceid=%u,m_connectionstate=%u, not inite state continue search.\n", pCRSinfo->m_crsdeviceid, pCRSinfo->m_connectionstate);
				continue;
			}
			if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������CRS��ʱ������netmp��������
			{
				sr_printf(SR_PRINT_WARN, "updateCRSInfoPeric connect_nums=%u > cfg_idle_nums=%u,so do nothing.\n", connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
				break;
			}
			sr_printf(SR_PRINT_INFO, "updateCRSInfoPeric idle_crss=%lu,cfg_idle_crss=%u,connected_crss=%lu\n",
				m_crsinfomanager->m_idle_crss.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),	m_crsinfomanager->m_connected_crss.size());
			connect_nums++;

			SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
			struct in_addr s; // IPv4��ַ�ṹ��
			s.s_addr = pCRSinfo->m_ip;
			inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
			sr_printf(SR_PRINT_INFO, "updateCRSInfoPeric next connect crs(%u)(%s,%u,%u)\n", pCRSinfo->m_ip, IPdotdec, pCRSinfo->m_port, pCRSinfo->m_crsdeviceid);
			SockRelateData srd(kcrs_sock_type, pCRSinfo->m_crsdeviceid);
			pCRSinfo->m_connectionstate = e_dev_sock_connecting;
			if (m_deviceMan)
				m_deviceMan->ConnectCRS(IPdotdec, pCRSinfo->m_port, srd);

			sr_printf(SR_PRINT_INFO, "updateCRSInfoPeric new crs(%u)(%s,%u,%u) is connecting.....\n", pCRSinfo->m_ip, IPdotdec, pCRSinfo->m_port, pCRSinfo->m_crsdeviceid);
		}
	}
}

SR_void DevMgrNetMPProcessThread::updateSRSInfoPeric()
{
	if (MCCfgInfo::instance()->get_mcdeviceid() != 0) // mcע��ɹ��ŷ��ͣ����ⷢ������deviceidΪ0������devmgr��������
	{
		SRMsgs::ReqGetDeviceInfo reqgdi;
		reqgdi.set_token(MCCfgInfo::instance()->get_mctoken());
		reqgdi.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
		reqgdi.set_devnums(10);
		reqgdi.set_get_svr_type(DEVICE_SERVER::DEVICE_SRS);//��ȡSRS�豸��Ϣ
		reqgdi.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
		SerialAndSendDevmgr_nobuf(getMsgIdByClassName(ReqGetDeviceInfo), &reqgdi);
	}

	if (0u == m_srsinfomanager->m_srsinfo.size())
	{
		//sr_printf(SR_PRINT_INFO,"updateCRSInfoPeric have no crs\n");
		return;
	}
	else
	{
		time_t timeNow;
		timeNow = time(NULL);

		std::map<SR_int32, SR_int32> offlinesrsset;// srsid<-->isroot
		offlinesrsset.clear();

		// ��ʱ���srs�ı仯
		for (std::map<SR_uint32, SRSInfo*>::iterator srs_itor = m_srsinfomanager->m_srsinfo.begin();
			srs_itor != m_srsinfomanager->m_srsinfo.end(); srs_itor++)
		{
			SRSInfo* psrsinfo = NULL;
			psrsinfo = srs_itor->second;
			if (psrsinfo
				&& timeNow - psrsinfo->m_srsdevtime > TOLERANCE_TIME)
			{
				offlinesrsset.insert(std::make_pair(srs_itor->first, psrsinfo->m_load));
			}
		}

		for (std::map<SR_int32, SR_int32>::iterator offsrs_itor = offlinesrsset.begin();
			offsrs_itor != offlinesrsset.end(); offsrs_itor++)
		{
			std::map<SR_uint32, SRSInfo*>::iterator srs_itor = m_srsinfomanager->m_srsinfo.find(offsrs_itor->first);
			if (srs_itor != m_srsinfomanager->m_srsinfo.end())
			{
				SRSInfo* poffsrsinfo = NULL;
				poffsrsinfo = srs_itor->second;
				if (poffsrsinfo)
				{
					sr_printf(SR_PRINT_INFO, "updateSRSInfoPeric delete outtime srs[ip=%s,port=%u,srsid=%u,isroot=%u]\n", poffsrsinfo->m_strip.c_str(), poffsrsinfo->m_port, poffsrsinfo->m_srsdeviceid, poffsrsinfo->m_load);

					SRSInfoData* pSRSinfodata = new SRSInfoData();
					pSRSinfodata->m_srsid = poffsrsinfo->m_srsdeviceid;
					pSRSinfodata->m_isroot = poffsrsinfo->m_load;
					pSRSinfodata->m_addordel = 2;//ɾ��
					PostToTerThread(e_update_srsinfo_inconf, pSRSinfodata);

					delete poffsrsinfo;
					poffsrsinfo = NULL;
				}
				m_srsinfomanager->m_srsinfo.erase(srs_itor);
			}
		}
	}
}

SR_void DevMgrNetMPProcessThread::updateRelayServerInfoPeric()
{
	if (MCCfgInfo::instance()->get_mcdeviceid() != 0) // mcע��ɹ��ŷ��ͣ����ⷢ������deviceidΪ0������devmgr��������
	{
		SRMsgs::ReqGetDeviceInfo reqgdi;
		reqgdi.set_token(MCCfgInfo::instance()->get_mctoken());
		reqgdi.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
		reqgdi.set_devnums(10);
		reqgdi.set_get_svr_type(DEVICE_SERVER::DEVICE_RELAYSERVER);//��ȡrelayserver�豸��Ϣ
		reqgdi.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
		SerialAndSendDevmgr_nobuf(getMsgIdByClassName(ReqGetDeviceInfo), &reqgdi);
	}

	if (0u == m_relaysvrinfomgr->m_relaysvrinfo.size())
	{
		//sr_printf(SR_PRINT_INFO,"updateRelayServerInfoPeric have no relayserver\n");
		return;
	}
	else
	{
		//�Ѿ��ﵽ�������
		if (m_relaysvrinfomgr->m_connected_relaysvrs.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp()) // ����netmp��������
		{
			sr_printf(SR_PRINT_INFO, "updateRelayServerInfoPeric have max relayserver_tcp_connect[cur:%d, cfg:%d],so do nothing.\n",
				m_relaysvrinfomgr->m_connected_relaysvrs.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
			return;
		}

		if (m_relaysvrinfomgr->m_idle_relaysvrs.size() >= MCCfgInfo::instance()->get_idle_netmp_nums()) // ����netmp��������
		{
			sr_printf(SR_PRINT_INFO, "updateRelayServerInfoPeric m_idle_relaysvrs=%u >= cfg_idle_nums=%u,so do nothing.\n",
				m_relaysvrinfomgr->m_idle_relaysvrs.size(), MCCfgInfo::instance()->get_idle_netmp_nums());
			return;
		}
		//����Ƿ���Ҫ�½�����
		SR_uint32 connect_nums = 1u;
		for (std::map<SR_uint32, RelayServerInfo*>::iterator rs_itor = m_relaysvrinfomgr->m_relaysvrinfo.begin();
			rs_itor != m_relaysvrinfomgr->m_relaysvrinfo.end(); rs_itor++)
		{
			RelayServerInfo* pRSinfo = rs_itor->second;
			if (pRSinfo->m_connectionstate != e_dev_sock_inite)
			{
				sr_printf(SR_PRINT_INFO, "updateRelayServerInfoPeric pRSinfo->m_relaysvrdevid=%u,m_connectionstate=%u, not inite state continue search.\n", pRSinfo->m_relaysvrdevid, pRSinfo->m_connectionstate);
				continue;
			}
			if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums()) // ����netmp��������
			{
				sr_printf(SR_PRINT_WARN, "updateRelayServerInfoPeric connect_nums=%u > cfg_idle_nums=%u,so do nothing.\n", connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
				break;
			}
			sr_printf(SR_PRINT_INFO, "updateRelayServerInfoPeric idle_relayservers=%lu,cfg_idle_relayservers=%u,connected_relayservers=%lu\n",
				m_relaysvrinfomgr->m_idle_relaysvrs.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
				m_relaysvrinfomgr->m_connected_relaysvrs.size());
			connect_nums++;

			SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
			struct in_addr s; // IPv4��ַ�ṹ��
			s.s_addr = pRSinfo->m_ip;
			inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
			sr_printf(SR_PRINT_INFO, "updateRelayServerInfoPeric next connect relayserver(%u)(%s,%u,%u)\n", pRSinfo->m_ip, IPdotdec, pRSinfo->m_port, pRSinfo->m_relaysvrdevid);
			SockRelateData srd(krelayserver_sock_type, pRSinfo->m_relaysvrdevid);
			pRSinfo->m_connectionstate = e_dev_sock_connecting;
			if (m_deviceMan)
				m_deviceMan->ConnectRelayserver(IPdotdec, pRSinfo->m_port, srd);

			sr_printf(SR_PRINT_INFO, "updateRelayServerInfoPeric new relayserver(%u)(%s,%u,%u) is connecting.....\n", pRSinfo->m_ip, IPdotdec, pRSinfo->m_port, pRSinfo->m_relaysvrdevid);
		}
	}
}

SR_void DevMgrNetMPProcessThread::updateRelayMCInfoPeric()
{
	if (MCCfgInfo::instance()->get_mcdeviceid() != 0) // mcע��ɹ��ŷ��ͣ����ⷢ������deviceidΪ0������devmgr��������
	{
		SRMsgs::ReqGetDeviceInfo reqgdi;
		reqgdi.set_token(MCCfgInfo::instance()->get_mctoken());
		reqgdi.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
		reqgdi.set_devnums(10);
		reqgdi.set_get_svr_type(DEVICE_SERVER::DEVICE_RELAY_MC);//��ȡrelaymc�豸��Ϣ
		reqgdi.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
		SerialAndSendDevmgr_nobuf(getMsgIdByClassName(ReqGetDeviceInfo), &reqgdi);
	}
}

SR_void DevMgrNetMPProcessThread::updateSystemCurLoadPeric()
{
	if (MCCfgInfo::instance()->get_mcdeviceid() != 0) // mcע��ɹ��ŷ��ͣ����ⷢ������deviceidΪ0������devmgr��������
	{
		SRMsgs::ReqGetSystemCurLoad reqgscl;
		reqgscl.set_token(MCCfgInfo::instance()->get_mctoken());
		reqgscl.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
		SerialAndSendDevmgr_nobuf(getMsgIdByClassName(ReqGetSystemCurLoad), &reqgscl);
	}
}

void* DevMgrNetMPProcessThread::createTimer(SR_uint64 millisecond,SR_int32 timertype,SR_uint32 timerflag,SR_uint64 timerdata)
{
	TimerData* ptd = new TimerData(timerflag, timerdata);
	bool isPerio = ((timertype == e_periodicity_timer) ? true : false);
	return m_timermanger->CreateTimer(this, ptd, (SR_uint32)millisecond, isPerio);
}

SR_bool DevMgrNetMPProcessThread::deleteTimer(SR_void* timerid)
{
	if (timerid != NULL)
	{
		sr_printf(SR_PRINT_CRIT, "DevMgrNetMPProcessThread::deleteTimer,timerid=%p.\n", timerid);
		return m_timermanger->DeleteTimer((TIMERID)timerid);
	}
	else
	{
		sr_printf(SR_PRINT_CRIT, "DevMgrNetMPProcessThread::deleteTimer,timerid is null.\n");
		return false;
	}
}

SR_void DevMgrNetMPProcessThread::onSockError(const SockErrorData* psed)
{
	sr_printf(SR_PRINT_ERROR, "onSockError socketptr=%p is error\n", psed->m_pErrorSocket);

	if (m_netmpinfomanager->m_current_devmgr_sockptr == psed->m_pErrorSocket)
	{
		sr_printf(SR_PRINT_ERROR, "onSockError devmgr socketptr=%p is error,then connect other devmgr\n", psed->m_pErrorSocket);
		m_netmpinfomanager->m_current_devmgr_sockptr = NULL;

		//��ֹmc��devmgr����ߺ�������֮ǰ�л������ٿ����߽���(�����ն�������)����ʱ��devmgr�������Ϣm_buffered_dev����ŵ���Ϣ��mc deviceid��token���ܱ�����
		/*
		MCCfgInfo::instance()->set_mcdeviceid(0u);
		std::string str_null;
		MCCfgInfo::instance()->set_mctoken(str_null);
		*/

		//����Ᵽ֤�������ͬһ��sock�׳����δ����¼�
		//DevmgrConnect* pdeleteConnect = static_cast<DevmgrConnect*>(psed->m_pErrorSocket);
		//delete pdeleteConnect; -- �ŵ����߳� timer ����


		for(std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
			devs_itor != m_netmpinfomanager->m_devmgrInfos.end();++devs_itor)
		{
			DevSockInfo* pdsinfo = *devs_itor;
			if((psed->m_pErrorSocket == pdsinfo->m_sockptr) && (e_DevMgr_sock_connect_ok == pdsinfo->m_devsockstat))
			{
				pdsinfo->m_devsockstat = e_DevMgr_sock_error;
				pdsinfo->m_sockptr = NULL;
				break;
			}
		}
		
		//�����µ�devmgr��������
		for(std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
			devs_itor != m_netmpinfomanager->m_devmgrInfos.end();++devs_itor)
		{
			DevSockInfo* pdsinfo = *devs_itor;
			if((NULL == pdsinfo->m_sockptr) && (e_DevMgr_sock_init == pdsinfo->m_devsockstat))
			{
				pdsinfo->m_sockptr = m_deviceMan->ConnectDevmgr(pdsinfo->m_ip.c_str(),pdsinfo->m_port);
				m_netmpinfomanager->m_current_dev_connectcnt++;//ÿ����һ�μ�1

				if (m_netmpinfomanager->m_current_dev_connectcnt == 3)
				{
					PostToTerThread(e_notify_to_check_confcrsinfo, 0);
				}

				if(false == m_deviceMan->isConnecting())
				{
					sr_printf(SR_PRINT_ERROR, "connect new devmgr(ip=%s, port=%d) is failed.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port);
					pdsinfo->m_devsockstat = e_DevMgr_sock_error;
					pdsinfo->m_sockptr = NULL;
				}
				else
				{
					sr_printf(SR_PRINT_NORMAL, "connect new devmgr(ip=%s, port=%d) is connecting, m_sockptr=%p.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port, pdsinfo->m_sockptr);
					pdsinfo->m_devsockstat = e_DevMgr_sock_connecting;
					m_netmpinfomanager->m_current_devmgr_sockptr = pdsinfo->m_sockptr;
					return;
				}
			}
		}
		//��ʱû���ҵ����õ�devmgr,��ʱȥ��֮ǰ�����ϵ�
		for(std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
			devs_itor != m_netmpinfomanager->m_devmgrInfos.end();++devs_itor)
		{
			DevSockInfo* pdsinfo = *devs_itor;
			pdsinfo->m_devsockstat = e_DevMgr_sock_init;
			pdsinfo->m_sockptr = NULL;
		}

		m_netmpinfomanager->m_connect_dev_loopcnt++;//ÿ����һ�����Ӻ��1
		if (m_netmpinfomanager->m_connect_dev_loopcnt == MCCfgInfo::instance()->get_reconnect_devmgr_loopcnt())
		{
			sr_printf(SR_PRINT_WARN, "onSockError devmgr socketptr=%p is error,and connect all devmgr third loop is failed, so end all conf.\n", psed->m_pErrorSocket);

			PostToTerThread(e_notify_to_end_conf, 0);
		}
		sr_printf(SR_PRINT_WARN, "devmgr socketptr=%p is error,and connect all devmgr is failed, so continue search other devmgr.\n", psed->m_pErrorSocket);
		
		Msgid_Timerid_Pair* ppair = NULL;
		ppair = new Msgid_Timerid_Pair();
		if (ppair != NULL)
		{
			ppair->m_msgid = e_connect_devmgr_timer;
			ppair->m_ptimer = NULL;
			//��ʱ��ʱ��ǳ��̵�ʱ��,����֮�� ppair ��δ��ֵ ��Ӱ����
			ppair->m_ptimer = createTimer(MCCfgInfo::instance()->get_reconnect_devmgr_time(), e_dispoable_timer, e_connect_devmgr_timer, (SR_uint64)ppair);
			if (ppair->m_ptimer != NULL)
			{
				sr_printf(SR_PRINT_CRIT, "devmgr socketptr=%p is error,ppair=%p [msgid=0x%x, timerid=%p] push_back list for continue search other devmgr.\n", psed->m_pErrorSocket, ppair, ppair->m_msgid, ppair->m_ptimer);
				m_connect_devmgr_timer_list.push_back(ppair);
			}
			else
			{
				sr_printf(SR_PRINT_CRIT, "devmgr socketptr=%p is error,createTimer is null.\n", psed->m_pErrorSocket);
				delete ppair;
			}
		}
		else
		{
			sr_printf(SR_PRINT_CRIT, "devmgr socketptr=%p is error,new ppair is null.\n", psed->m_pErrorSocket);
		}
		return;
	}
	else
	{			
		SR_uint32 netmpid = 0u;
		std::map<SR_uint32, SR_void*>::const_iterator netmpidsockptr_citor = m_netmpinfomanager->m_netmpsockptr.begin();
		for (; netmpidsockptr_citor != m_netmpinfomanager->m_netmpsockptr.end(); netmpidsockptr_citor++)
		{
			if (netmpidsockptr_citor->second == psed->m_pErrorSocket)
			{
				netmpid = netmpidsockptr_citor->first;
				break;
			}
		}
		//if (0u == netmpid)
		//	sr_printf(SR_PRINT_ERROR, "can not find netmpid by socketptr=%p\n", psed->m_pErrorSocket);
		//else
		//	sr_printf(SR_PRINT_INFO, "netmp=%u, socketptr=%p is error\n", netmpid, psed->m_pErrorSocket);

		if (netmpid != 0)
		{
			sr_printf(SR_PRINT_ERROR, "onSockError netmpid=%u, socketptr=%p is error\n", netmpid, psed->m_pErrorSocket);

			NetMPSockErrorData* p_notify_Ex = new NetMPSockErrorData();
			p_notify_Ex->m_pSocket = psed->m_pErrorSocket;
			p_notify_Ex->m_netmpid = netmpid;
			PostToTerThread(e_notify_netmpsock_error, p_notify_Ex);
			// TODO:��Ϊfind
			std::map<SR_uint32, NetMPInfo*>::iterator netmpinfos = m_netmpinfomanager->m_netmpinfo.begin();
			for (; netmpinfos != m_netmpinfomanager->m_netmpinfo.end(); netmpinfos++)
			{
				NetMPInfo* pnetmpinfo = netmpinfos->second;
				if (pnetmpinfo->m_netmpdeviceid == netmpid)
				{
					pnetmpinfo->m_connectionstate = e_Netmp_sock_closed;

					std::set<SR_uint32>::iterator netmp_set_itor;
					netmp_set_itor = m_netmpinfomanager->m_connected_nemtps.find(netmpid);
					if (netmp_set_itor != m_netmpinfomanager->m_connected_nemtps.end())
						m_netmpinfomanager->m_connected_nemtps.erase(netmp_set_itor);

					netmp_set_itor = m_netmpinfomanager->m_idle_netmps.find(netmpid);
					if (netmp_set_itor != m_netmpinfomanager->m_idle_netmps.end())
						m_netmpinfomanager->m_idle_netmps.erase(netmp_set_itor);

					netmp_set_itor = m_netmpinfomanager->m_highload_netmps.find(netmpid);
					if (netmp_set_itor != m_netmpinfomanager->m_highload_netmps.end())
						m_netmpinfomanager->m_highload_netmps.erase(netmp_set_itor);

					netmp_set_itor = m_netmpinfomanager->m_notuse_netmps.find(netmpid);
					if (netmp_set_itor != m_netmpinfomanager->m_notuse_netmps.end())
						m_netmpinfomanager->m_notuse_netmps.erase(netmp_set_itor);

					//netmp_set_itor = m_netmpinfomanager->m_netmpinconf.find(netmpid);
					//if (netmp_set_itor != m_netmpinfomanager->m_netmpinconf.end())
					//	m_netmpinfomanager->m_netmpinconf.erase(netmp_set_itor);

					//fix BUG: NetMP����֮�󲻿�ʹ��
					m_netmpinfomanager->m_netmpinfo.erase(netmpinfos);// ɾ�������netmp ��Ϣ,���ܴ����ڴ�й©��������
					std::map<SR_uint32, SR_void*>::iterator netmpidsockptr_itor = m_netmpinfomanager->m_netmpsockptr.find(netmpid);
					if (netmpidsockptr_itor != m_netmpinfomanager->m_netmpsockptr.end())
					{
						sr_printf(SR_PRINT_ERROR, "onSockError erase netmpid=%u,sockptr=%p in NetMPInfoManager::m_netmpsockptr\n", netmpid, netmpidsockptr_itor->second);
						m_netmpinfomanager->m_netmpsockptr.erase(netmpidsockptr_itor);// netmpidsockptr_itor->second��Ӧ��һ������psrd->m_pSocket,������DevmgrConnect,�˴����ܴ����ڴ�й©����������
					}

					//SRMsgs::IndNetMPConnStatusInMC inddevnetmpdisconn;
					//inddevnetmpdisconn.set_deviceid(SRMC::MCCfgInfo::instance()->get_mcdeviceid());
					//inddevnetmpdisconn.set_token(SRMC::MCCfgInfo::instance()->get_mctoken());
					//inddevnetmpdisconn.set_netmpid(pnetmpinfo->m_netmpdeviceid);
					//inddevnetmpdisconn.set_connstatus(0); // �Ͽ�����
					//inddevnetmpdisconn.set_groupid(pnetmpinfo->m_groupid);
					//SerialAndSendDevmgr_nobuf(getMsgIdByClassName(IndNetMPConnStatusInMC), &inddevnetmpdisconn);
					////SerialAndSendDevmgr_buffered(getMsgIdByClassName(IndNetMPConnStatusInMC), &inddevnetmpdisconn);

					break;
				}
			}

			//�Ѿ��ﵽ�������
			if (m_netmpinfomanager->m_connected_nemtps.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp())
			{
				//sr_printf(SR_PRINT_INFO,"");
				sr_printf(SR_PRINT_INFO,"onSockError have max netmp_tcp_connect[cur:%d, cfg:%d],so do nothing.\n",
					m_netmpinfomanager->m_connected_nemtps.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
				return;
			}

			if (m_netmpinfomanager->m_idle_netmps.size() >= MCCfgInfo::instance()->get_idle_netmp_nums())
			{
				sr_printf(SR_PRINT_INFO,"onSockError m_idle_netmps=%u,cfg_idle_nums=%u,so do nothing.\n",
					m_netmpinfomanager->m_idle_netmps.size(),MCCfgInfo::instance()->get_idle_netmp_nums());
				return;
			}
			//����Ƿ���Ҫ�½�����
			SR_uint32 connect_nums = 1u;
			for (std::map<SR_uint32, NetMPInfo*>::iterator netmpitor = m_netmpinfomanager->m_netmpinfo.begin();
				netmpitor != m_netmpinfomanager->m_netmpinfo.end(); netmpitor++)
			{
				NetMPInfo* pnetinfo = netmpitor->second;
				if (pnetinfo->m_connectionstate != e_Netmp_sock_inite)
				{
					sr_printf(SR_PRINT_INFO, "onSockError pnetinfo->m_netmpdeviceid=%d, m_connectionstate=%u, not inite state continue search.\n", pnetinfo->m_netmpdeviceid, pnetinfo->m_connectionstate);
					continue;
				}
				if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums())
				{
					sr_printf(SR_PRINT_WARN, "onSockError netmp connect_nums=%u > cfg_idle_nums=%u,so do nothing.\n", connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
					break;
				}
				sr_printf(SR_PRINT_INFO, "onSockError idle_netmps=%lu,cfg_idle_netmps=%u,connected_netmps=%lu\n",
					m_netmpinfomanager->m_idle_netmps.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
					m_netmpinfomanager->m_connected_nemtps.size());
				connect_nums++;

				SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
				struct in_addr s; // IPv4��ַ�ṹ��
				s.s_addr = pnetinfo->m_ip;
				inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
				sr_printf(SR_PRINT_INFO, "onSockError check connect netmp(%u)(%s,%u,%u)\n", pnetinfo->m_ip, IPdotdec, pnetinfo->m_port, pnetinfo->m_netmpdeviceid);
				SockRelateData srd(knetmp_sock_type, pnetinfo->m_netmpdeviceid);
				pnetinfo->m_connectionstate = e_Netmp_sock_connecting;
				if (m_deviceMan)
					m_deviceMan->ConnectNetmp(IPdotdec, pnetinfo->m_port, srd);

				sr_printf(SR_PRINT_INFO, "onSockError check connect netmp(%u)(%s,%u,%u) is connecting.....\n", pnetinfo->m_ip, IPdotdec, pnetinfo->m_port, pnetinfo->m_netmpdeviceid);
			}

			return;
		}
		

		SR_uint32 mpid = 0u;
		std::map<SR_uint32, SR_void*>::const_iterator mpidsockptr_citor = m_mpinfomanager->m_mpsockptr.begin();
		for (; mpidsockptr_citor != m_mpinfomanager->m_mpsockptr.end(); mpidsockptr_citor++)
		{
			if (mpidsockptr_citor->second == psed->m_pErrorSocket)
			{
				mpid = mpidsockptr_citor->first;
				break;
			}
		}
		//if (0u == mpid)
		//	sr_printf(SR_PRINT_ERROR, "can not find mpid by socketptr=%p\n", psed->m_pErrorSocket);
		//else
		//	sr_printf(SR_PRINT_INFO, "mp=%u, socketptr=%p is error\n", mpid, psed->m_pErrorSocket);

		if (mpid != 0)
		{
			sr_printf(SR_PRINT_ERROR, "onSockError mp=%u, socketptr=%p is error\n", mpid, psed->m_pErrorSocket);

			MPSockErrorData* p_notify_Ex = new MPSockErrorData();
			p_notify_Ex->m_pSocket = psed->m_pErrorSocket;
			p_notify_Ex->m_mpid = mpid;
			PostToTerThread(e_notify_mpsock_error, p_notify_Ex);
			// TODO:��Ϊfind
			std::map<SR_uint32, MPInfo*>::iterator mpinfos = m_mpinfomanager->m_mpinfo.begin();
			for (; mpinfos != m_mpinfomanager->m_mpinfo.end(); mpinfos++)
			{
				MPInfo* pMpinfo = mpinfos->second;
				if (pMpinfo->m_mpdeviceid == mpid)
				{
					pMpinfo->m_connectionstate = e_Mp_sock_closed;

					std::set<SR_uint32>::iterator mp_set_itor;
					mp_set_itor = m_mpinfomanager->m_connected_mps.find(mpid);
					if (mp_set_itor != m_mpinfomanager->m_connected_mps.end())
						m_mpinfomanager->m_connected_mps.erase(mp_set_itor);

					mp_set_itor = m_mpinfomanager->m_idle_mps.find(mpid);
					if (mp_set_itor != m_mpinfomanager->m_idle_mps.end())
						m_mpinfomanager->m_idle_mps.erase(mp_set_itor);

					mp_set_itor = m_mpinfomanager->m_highload_mps.find(mpid);
					if (mp_set_itor != m_mpinfomanager->m_highload_mps.end())
						m_mpinfomanager->m_highload_mps.erase(mp_set_itor);

					mp_set_itor = m_mpinfomanager->m_notuse_mps.find(mpid);
					if (mp_set_itor != m_mpinfomanager->m_notuse_mps.end())
						m_mpinfomanager->m_notuse_mps.erase(mp_set_itor);

					//mp_set_itor = m_mpinfomanager->m_mpinconf.find(mpid);
					//if (mp_set_itor != m_mpinfomanager->m_mpinconf.end())
					//	m_mpinfomanager->m_mpinconf.erase(mp_set_itor);

					//fix BUG: NetMP����֮�󲻿�ʹ��
					m_mpinfomanager->m_mpinfo.erase(mpinfos);// ɾ�������netmp ��Ϣ
					std::map<SR_uint32, SR_void*>::iterator mpidsockptr_itor = m_mpinfomanager->m_mpsockptr.find(mpid);
					if (mpidsockptr_itor != m_mpinfomanager->m_mpsockptr.end())
					{
						sr_printf(SR_PRINT_ERROR, "onSockError erase mpid=%u,sockptr=%p in MPInfoManager::m_mpsockptr\n", mpid, mpidsockptr_itor->second);
						m_mpinfomanager->m_mpsockptr.erase(mpidsockptr_itor);
					}

					break;
				}
			}

			//�Ѿ��ﵽ�������
			if (m_mpinfomanager->m_connected_mps.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp()) // ������chen songhua������MP��ʱ������netmp��������
			{
				//sr_printf(SR_PRINT_INFO,"");
				sr_printf(SR_PRINT_INFO,"onSockError have max mp_tcp_connect[cur:%d, cfg:%d],so do nothing.\n",
					m_mpinfomanager->m_connected_mps.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
				return;
			}

			if (m_mpinfomanager->m_idle_mps.size() >= MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
			{
				sr_printf(SR_PRINT_INFO,"onSockError m_idle_mps=%u >= cfg_idle_nums=%u,so do nothing.\n",
					m_mpinfomanager->m_idle_mps.size(),MCCfgInfo::instance()->get_idle_netmp_nums());
				return;
			}
			//����Ƿ���Ҫ�½�����
			SR_uint32 connect_nums = 1u;
			for (std::map<SR_uint32, MPInfo*>::iterator mpitor = m_mpinfomanager->m_mpinfo.begin();
				mpitor != m_mpinfomanager->m_mpinfo.end(); mpitor++)
			{
				MPInfo* pMpinfo = mpitor->second;
				if (pMpinfo->m_connectionstate != e_Mp_sock_inite)
				{
					sr_printf(SR_PRINT_INFO, "onSockError pMpinfo->m_mpdeviceid=%u,m_connectionstate=%u, not inite state continue search.\n", pMpinfo->m_mpdeviceid, pMpinfo->m_connectionstate);
					continue;
				}
				if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
				{
					sr_printf(SR_PRINT_WARN, "onSockError mp connect_nums=%u > cfg_idle_nums=%u,so do nothing.\n", connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
					break;
				}
				sr_printf(SR_PRINT_INFO, "onSockError idle_mps=%lu,cfg_idle_mps=%u,connected_mps=%lu\n",
					m_mpinfomanager->m_idle_mps.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
					m_mpinfomanager->m_connected_mps.size());
				connect_nums++;

				SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
				struct in_addr s; // IPv4��ַ�ṹ��
				s.s_addr = pMpinfo->m_ip;
				inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
				sr_printf(SR_PRINT_INFO, "onSockError check connect mp(%u)(%s,%u,%u)\n", pMpinfo->m_ip, IPdotdec, pMpinfo->m_port, pMpinfo->m_mpdeviceid);
				SockRelateData srd(kmp_sock_type, pMpinfo->m_mpdeviceid);
				pMpinfo->m_connectionstate = e_Mp_sock_connecting;
				if (m_deviceMan)
					m_deviceMan->ConnectMP(IPdotdec, pMpinfo->m_port, srd);

				sr_printf(SR_PRINT_INFO, "onSockError check connect mp(%u)(%s,%u,%u) is connecting.....\n", pMpinfo->m_ip, IPdotdec, pMpinfo->m_port, pMpinfo->m_mpdeviceid);
			}

			return;
		}

		SR_uint32 gwid = 0u;
		std::map<SR_uint32, SR_void*>::const_iterator gwidsockptr_citor = m_gwinfomanager->m_gwsockptr.begin();
		for (; gwidsockptr_citor != m_gwinfomanager->m_gwsockptr.end(); gwidsockptr_citor++)
		{
			if (gwidsockptr_citor->second == psed->m_pErrorSocket)
			{
				gwid = gwidsockptr_citor->first;
				break;
			}
		}

		if (gwid != 0)
		{
			sr_printf(SR_PRINT_ERROR, "onSockError gwid=%u, socketptr=%p is error\n", gwid, psed->m_pErrorSocket);

			GWSockErrorData* p_notify_Ex = new GWSockErrorData();
			p_notify_Ex->m_pSocket = psed->m_pErrorSocket;
			p_notify_Ex->m_gwid = gwid;
			PostToTerThread(e_notify_gwsock_error, p_notify_Ex);
			// TODO:��Ϊfind
			std::map<SR_uint32, GWInfo*>::iterator gwinfos = m_gwinfomanager->m_gwinfo.begin();
			for (; gwinfos != m_gwinfomanager->m_gwinfo.end(); gwinfos++)
			{
				GWInfo* pGWinfo = gwinfos->second;
				if (pGWinfo->m_gwdeviceid == gwid)
				{
					pGWinfo->m_connectionstate = e_GW_sock_closed;

					std::set<SR_uint32>::iterator gw_set_itor;
					gw_set_itor = m_gwinfomanager->m_connected_gws.find(gwid);
					if (gw_set_itor != m_gwinfomanager->m_connected_gws.end())
						m_gwinfomanager->m_connected_gws.erase(gw_set_itor);

					gw_set_itor = m_gwinfomanager->m_idle_gws.find(gwid);
					if (gw_set_itor != m_gwinfomanager->m_idle_gws.end())
						m_gwinfomanager->m_idle_gws.erase(gw_set_itor);

					gw_set_itor = m_gwinfomanager->m_highload_gws.find(gwid);
					if (gw_set_itor != m_gwinfomanager->m_highload_gws.end())
						m_gwinfomanager->m_highload_gws.erase(gw_set_itor);

					gw_set_itor = m_gwinfomanager->m_notuse_gws.find(gwid);
					if (gw_set_itor != m_gwinfomanager->m_notuse_gws.end())
						m_gwinfomanager->m_notuse_gws.erase(gw_set_itor);

					//mp_set_itor = m_mpinfomanager->m_mpinconf.find(mpid);
					//if (mp_set_itor != m_mpinfomanager->m_mpinconf.end())
					//	m_mpinfomanager->m_mpinconf.erase(mp_set_itor);

					//fix BUG: NetMP����֮�󲻿�ʹ��
					m_gwinfomanager->m_gwinfo.erase(gwinfos);// ɾ�������gw ��Ϣ
					std::map<SR_uint32, SR_void*>::iterator gwidsockptr_itor = m_gwinfomanager->m_gwsockptr.find(gwid);
					if (gwidsockptr_itor != m_gwinfomanager->m_gwsockptr.end())
					{
						sr_printf(SR_PRINT_ERROR, "onSockError erase gwid=%u,sockptr=%p in GWInfoManager::m_gwsockptr\n", gwid, gwidsockptr_itor->second);
						m_gwinfomanager->m_gwsockptr.erase(gwidsockptr_itor);
					}

					break;
				}
			}

			//�Ѿ��ﵽ�������
			if (m_gwinfomanager->m_connected_gws.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp()) // ������chen songhua������MP��ʱ������netmp��������
			{
				sr_printf(SR_PRINT_INFO,"onSockError have max gw_tcp_connect[cur:%d, cfg:%d],so do nothing.\n",
					m_gwinfomanager->m_connected_gws.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
				return;
			}

			if (m_gwinfomanager->m_idle_gws.size() >= MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
			{
				sr_printf(SR_PRINT_INFO,"onSockError m_idle_gws=%u >= cfg_idle_nums=%u,so do nothing.\n",
					m_gwinfomanager->m_idle_gws.size(), MCCfgInfo::instance()->get_idle_netmp_nums());
				return;
			}
			//����Ƿ���Ҫ�½�����
			SR_uint32 connect_nums = 1u;
			for (std::map<SR_uint32, GWInfo*>::iterator gwitor = m_gwinfomanager->m_gwinfo.begin();
				gwitor != m_gwinfomanager->m_gwinfo.end(); gwitor++)
			{
				GWInfo* pGWinfo = gwitor->second;
				if (pGWinfo->m_connectionstate != e_GW_sock_inite)
				{
					sr_printf(SR_PRINT_INFO, "onSockError pGWinfo->m_gwdeviceid=%u,m_connectionstate=%u, not inite state continue search.\n", pGWinfo->m_gwdeviceid, pGWinfo->m_connectionstate);
					continue;
				}
				if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
				{
					sr_printf(SR_PRINT_WARN, "onSockError gw connect_nums=%u > cfg_idle_nums=%u,so do nothing.\n", 
						connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
					break;
				}
				sr_printf(SR_PRINT_INFO, "onSockError idle_gws=%lu,cfg_idle_gws=%u,connected_gws=%lu\n",
					m_gwinfomanager->m_idle_gws.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
					m_gwinfomanager->m_connected_gws.size());
				connect_nums++;

				SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
				struct in_addr s; // IPv4��ַ�ṹ��
				s.s_addr = pGWinfo->m_ip;
				inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
				sr_printf(SR_PRINT_INFO, "onSockError check connect gw(%u)(%s,%u,%u)\n", pGWinfo->m_ip, IPdotdec, pGWinfo->m_port, pGWinfo->m_gwdeviceid);
				SockRelateData srd(kgw_sock_type, pGWinfo->m_gwdeviceid);
				pGWinfo->m_connectionstate = e_GW_sock_connecting;
				if (m_deviceMan)
					m_deviceMan->ConnectGW(IPdotdec, pGWinfo->m_port, srd);

				sr_printf(SR_PRINT_INFO, "onSockError check connect gw(%u)(%s,%u,%u) is connecting.....\n", pGWinfo->m_ip, IPdotdec, pGWinfo->m_port, pGWinfo->m_gwdeviceid);
			}

			return;
		}

		SR_uint32 crsid = 0u;
		std::map<SR_uint32, SR_void*>::const_iterator crsidsockptr_citor = m_crsinfomanager->m_crssockptr.begin();
		for (; crsidsockptr_citor != m_crsinfomanager->m_crssockptr.end(); crsidsockptr_citor++)
		{
			if (crsidsockptr_citor->second == psed->m_pErrorSocket)
			{
				crsid = crsidsockptr_citor->first;
				break;
			}
		}

		if (crsid != 0)
		{
			sr_printf(SR_PRINT_ERROR, "onSockError crsid=%u, socketptr=%p is error\n", crsid, psed->m_pErrorSocket);

			CRSSockErrorData* p_notify_Ex = new CRSSockErrorData();
			p_notify_Ex->m_pSocket = psed->m_pErrorSocket;
			p_notify_Ex->m_crsid = crsid;
			PostToTerThread(e_notify_crssock_error, p_notify_Ex);
			// TODO:��Ϊfind
			std::map<SR_uint32, CRSInfo*>::iterator crsinfos_itor = m_crsinfomanager->m_crsinfo.begin();
			for (; crsinfos_itor != m_crsinfomanager->m_crsinfo.end(); crsinfos_itor++)
			{
				CRSInfo* pCRSinfo = crsinfos_itor->second;
				if (pCRSinfo->m_crsdeviceid == crsid)
				{
					pCRSinfo->m_connectionstate = e_dev_sock_closed;

					std::set<SR_uint32>::iterator crs_set_itor;
					crs_set_itor = m_crsinfomanager->m_connected_crss.find(crsid);
					if (crs_set_itor != m_crsinfomanager->m_connected_crss.end())
						m_crsinfomanager->m_connected_crss.erase(crs_set_itor);

					crs_set_itor = m_crsinfomanager->m_idle_crss.find(crsid);
					if (crs_set_itor != m_crsinfomanager->m_idle_crss.end())
						m_crsinfomanager->m_idle_crss.erase(crs_set_itor);

					//crs_set_itor = m_crsinfomanager->m_highload_crss.find(crsid);
					//if (crs_set_itor != m_crsinfomanager->m_highload_crss.end())
					//	m_crsinfomanager->m_highload_crss.erase(crs_set_itor);

					//crs_set_itor = m_crsinfomanager->m_notuse_crss.find(crsid);
					//if (crs_set_itor != m_crsinfomanager->m_notuse_crss.end())
					//	m_crsinfomanager->m_notuse_crss.erase(crs_set_itor);

					//fix BUG: NetMP����֮�󲻿�ʹ��
					m_crsinfomanager->m_crsinfo.erase(crsinfos_itor);// ɾ�������crs��Ϣ
					std::map<SR_uint32, SR_void*>::iterator crsidsockptr_itor = m_crsinfomanager->m_crssockptr.find(crsid);
					if (crsidsockptr_itor != m_crsinfomanager->m_crssockptr.end())
					{
						sr_printf(SR_PRINT_ERROR, "onSockError erase crsid=%u,sockptr=%p in CRSInfoManager::m_crssockptr\n", crsid, crsidsockptr_itor->second);
						m_crsinfomanager->m_crssockptr.erase(crsidsockptr_itor);
					}

					break;
				}
			}

			//�Ѿ��ﵽ�������
			if (m_crsinfomanager->m_connected_crss.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp()) // ������chen songhua������CRS��ʱ������netmp��������
			{
				sr_printf(SR_PRINT_INFO, "onSockError have max crs_tcp_connect[cur:%d, cfg:%d],so do nothing.\n",
					m_crsinfomanager->m_connected_crss.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
				return;
			}

			if (m_crsinfomanager->m_idle_crss.size() >= MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������CRS��ʱ������netmp��������
			{
				sr_printf(SR_PRINT_INFO, "onSockError m_idle_crss=%u >= cfg_idle_nums=%u,so do nothing.\n",
					m_crsinfomanager->m_idle_crss.size(), MCCfgInfo::instance()->get_idle_netmp_nums());
				return;
			}
			//����Ƿ���Ҫ�½�����
			SR_uint32 connect_nums = 1u;
			for (std::map<SR_uint32, CRSInfo*>::iterator crsitor = m_crsinfomanager->m_crsinfo.begin();
				crsitor != m_crsinfomanager->m_crsinfo.end(); crsitor++)
			{
				CRSInfo* pCRSinfo = crsitor->second;
				if (pCRSinfo->m_connectionstate != e_dev_sock_inite)
				{
					sr_printf(SR_PRINT_INFO, "onSockError pCRSinfo->m_crsdeviceid=%u,m_connectionstate=%u, not inite state continue search.\n", pCRSinfo->m_crsdeviceid, pCRSinfo->m_connectionstate);
					continue;
				}
				if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������CRS��ʱ������netmp��������
				{
					sr_printf(SR_PRINT_WARN, "onSockError crs connect_nums=%u > cfg_idle_nums=%u,so do nothing.\n",
						connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
					break;
				}
				sr_printf(SR_PRINT_INFO, "onSockError idle_crss=%lu,cfg_idle_crss=%u,connected_crss=%lu\n",
					m_crsinfomanager->m_idle_crss.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),	m_crsinfomanager->m_connected_crss.size());
				connect_nums++;

				SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
				struct in_addr s; // IPv4��ַ�ṹ��
				s.s_addr = pCRSinfo->m_ip;
				inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
				sr_printf(SR_PRINT_INFO, "onSockError check connect crs(%u)(%s,%u,%u)\n", pCRSinfo->m_ip, IPdotdec, pCRSinfo->m_port, pCRSinfo->m_crsdeviceid);
				SockRelateData srd(kcrs_sock_type, pCRSinfo->m_crsdeviceid);
				pCRSinfo->m_connectionstate = e_dev_sock_connecting;
				if (m_deviceMan)
					m_deviceMan->ConnectCRS(IPdotdec, pCRSinfo->m_port, srd);

				sr_printf(SR_PRINT_INFO, "onSockError check connect crs(%u)(%s,%u,%u) is connecting.....\n", pCRSinfo->m_ip, IPdotdec, pCRSinfo->m_port, pCRSinfo->m_crsdeviceid);
			}

			return;
		}

		SR_uint32 relayserverid = 0u;
		std::map<SR_uint32, SR_void*>::const_iterator relayserveridsockptr_citor = m_relaysvrinfomgr->m_relaysvrsockptr.begin();
		for (; relayserveridsockptr_citor != m_relaysvrinfomgr->m_relaysvrsockptr.end(); relayserveridsockptr_citor++)
		{
			if (relayserveridsockptr_citor->second == psed->m_pErrorSocket)
			{
				relayserverid = relayserveridsockptr_citor->first;
				break;
			}
		}

		if (relayserverid != 0)
		{
			sr_printf(SR_PRINT_ERROR, "onSockError relayserverid=%u, socketptr=%p is error\n", relayserverid, psed->m_pErrorSocket);

			RelayServerSockErrorData* p_notify_Ex = new RelayServerSockErrorData();
			p_notify_Ex->m_pSocket = psed->m_pErrorSocket;
			p_notify_Ex->m_relayserverid = relayserverid;
			PostToTerThread(e_notify_relayserversock_error, p_notify_Ex);
			// TODO:��Ϊfind
			std::map<SR_uint32, RelayServerInfo*>::iterator rsinfos_itor = m_relaysvrinfomgr->m_relaysvrinfo.begin();
			for (; rsinfos_itor != m_relaysvrinfomgr->m_relaysvrinfo.end(); rsinfos_itor++)
			{
				RelayServerInfo* pRSinfo = rsinfos_itor->second;
				if (pRSinfo->m_relaysvrdevid == relayserverid)
				{
					pRSinfo->m_connectionstate = e_dev_sock_closed;

					std::set<SR_uint32>::iterator rs_set_itor;
					rs_set_itor = m_relaysvrinfomgr->m_connected_relaysvrs.find(relayserverid);
					if (rs_set_itor != m_relaysvrinfomgr->m_connected_relaysvrs.end())
						m_relaysvrinfomgr->m_connected_relaysvrs.erase(rs_set_itor);

					rs_set_itor = m_relaysvrinfomgr->m_idle_relaysvrs.find(relayserverid);
					if (rs_set_itor != m_relaysvrinfomgr->m_idle_relaysvrs.end())
						m_relaysvrinfomgr->m_idle_relaysvrs.erase(rs_set_itor);

					rs_set_itor = m_relaysvrinfomgr->m_highload_relaysvrs.find(relayserverid);
					if (rs_set_itor != m_relaysvrinfomgr->m_highload_relaysvrs.end())
						m_relaysvrinfomgr->m_highload_relaysvrs.erase(rs_set_itor);

					rs_set_itor = m_relaysvrinfomgr->m_notuse_relaysvrs.find(relayserverid);
					if (rs_set_itor != m_relaysvrinfomgr->m_notuse_relaysvrs.end())
						m_relaysvrinfomgr->m_notuse_relaysvrs.erase(rs_set_itor);

					//mp_set_itor = m_mpinfomanager->m_mpinconf.find(mpid);
					//if (mp_set_itor != m_mpinfomanager->m_mpinconf.end())
					//	m_mpinfomanager->m_mpinconf.erase(mp_set_itor);

					//fix BUG: NetMP����֮�󲻿�ʹ��
					m_relaysvrinfomgr->m_relaysvrinfo.erase(rsinfos_itor);// ɾ�������relayserver��Ϣ
					std::map<SR_uint32, SR_void*>::iterator rsidsockptr_itor = m_relaysvrinfomgr->m_relaysvrsockptr.find(relayserverid);
					if (rsidsockptr_itor != m_relaysvrinfomgr->m_relaysvrsockptr.end())
					{
						sr_printf(SR_PRINT_ERROR, "onSockError erase relayserverid=%u,sockptr=%p in RelayServerInfoManager::m_relaysvrsockptr\n", relayserverid, rsidsockptr_itor->second);
						m_relaysvrinfomgr->m_relaysvrsockptr.erase(rsidsockptr_itor);
					}

					break;
				}
			}

			//�Ѿ��ﵽ�������
			if (m_relaysvrinfomgr->m_connected_relaysvrs.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp()) // ����netmp��������
			{
				sr_printf(SR_PRINT_INFO, "onSockError have max relayserver_tcp_connect[cur:%d, cfg:%d],so do nothing.\n",
					m_relaysvrinfomgr->m_connected_relaysvrs.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
				return;
			}

			if (m_relaysvrinfomgr->m_idle_relaysvrs.size() >= MCCfgInfo::instance()->get_idle_netmp_nums()) // ����netmp��������
			{
				sr_printf(SR_PRINT_INFO, "onSockError m_idle_relaysvrs=%u >= cfg_idle_nums=%u,so do nothing.\n",
					m_relaysvrinfomgr->m_idle_relaysvrs.size(), MCCfgInfo::instance()->get_idle_netmp_nums());
				return;
			}
			//����Ƿ���Ҫ�½�����
			SR_uint32 connect_nums = 1u;
			for (std::map<SR_uint32, RelayServerInfo*>::iterator rsitor = m_relaysvrinfomgr->m_relaysvrinfo.begin();
				rsitor != m_relaysvrinfomgr->m_relaysvrinfo.end(); rsitor++)
			{
				RelayServerInfo* pRSinfo = rsitor->second;
				if (pRSinfo->m_connectionstate != e_dev_sock_inite)
				{
					sr_printf(SR_PRINT_INFO, "onSockError pRSinfo->m_relaysvrdevid=%u,m_connectionstate=%u, not inite state continue search.\n", pRSinfo->m_relaysvrdevid, pRSinfo->m_connectionstate);
					continue;
				}
				if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������CRS��ʱ������netmp��������
				{
					sr_printf(SR_PRINT_WARN, "onSockError relayserver connect_nums=%u > cfg_idle_nums=%u,so do nothing.\n",
						connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
					break;
				}
				sr_printf(SR_PRINT_INFO, "onSockError idle_relayservers=%lu,cfg_idle_relayservers=%u,connected_relayservers=%lu\n",
					m_relaysvrinfomgr->m_idle_relaysvrs.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
					m_relaysvrinfomgr->m_connected_relaysvrs.size());
				connect_nums++;

				SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
				struct in_addr s; // IPv4��ַ�ṹ��
				s.s_addr = pRSinfo->m_ip;
				inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
				sr_printf(SR_PRINT_INFO, "onSockError check connect relayserver(%u)(%s,%u,%u)\n", pRSinfo->m_ip, IPdotdec, pRSinfo->m_port, pRSinfo->m_relaysvrdevid);
				SockRelateData srd(krelayserver_sock_type, pRSinfo->m_relaysvrdevid);
				pRSinfo->m_connectionstate = e_dev_sock_connecting;
				if (m_deviceMan)
					m_deviceMan->ConnectRelayserver(IPdotdec, pRSinfo->m_port, srd);

				sr_printf(SR_PRINT_INFO, "onSockError check connect relayserver(%u)(%s,%u,%u) is connecting.....\n", pRSinfo->m_ip, IPdotdec, pRSinfo->m_port, pRSinfo->m_relaysvrdevid);
			}

			return;
		}

	}//netmp��mp��gw��crs��relayserver
}
SR_void DevMgrNetMPProcessThread::onTimerArrive(const TimerData* ptd)
{
	SR_uint32 timerflag = ptd->m_timerflag;
	//sr_printf(SR_PRINT_DEBUG,"DevMgrNetMPProcessThread timerflag=0x%x\n",timerflag);
	switch(timerflag)
	{
		case e_update_netmp_timer:
		{
			updateNetmpInfoPeric();
			break;
		}
		case e_update_mp_timer:
		{
			updateMPInfoPeric();
			break;
		}
		case e_update_gw_timer:
		{
			updateGWInfoPeric();
			break;
		}
		case e_update_crs_timer:
		{
			updateCRSInfoPeric();
			break;
		}
		case e_update_srs_timer:
		{
			updateSRSInfoPeric();
			break;
		}
		case e_update_relayserver_timer:
		{
			updateRelayServerInfoPeric();
			break;
		}
		case e_update_relaymc_timer:
		{
			updateRelayMCInfoPeric();
			break;
		}
		case e_update_sys_load_timer:
		{
			updateSystemCurLoadPeric();
			break;
		}
		case e_hearttodevmgr_timer:
		{
			//SRMsgs::IndsertodevHeart indheart;
			//indheart.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
			//indheart.set_token(MCCfgInfo::instance()->get_mctoken());
			//indheart.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
			//indheart.set_load1(0);//TODO:��������cpu loadֵ,ȥ������
			//indheart.set_load2(GlobalVars::getAllTerNums());
			//SerialAndSendDevmgr_nobuf(getMsgIdByClassName(IndsertodevHeart), &indheart);

			PostToTerThread(e_notify_to_send_heart2devmgr, 0);

			break;
		}
		case e_check_devmgr_timer:
		{
			// sr_printf(SR_PRINT_DEBUG,"check_devmgr_timer current_sockptr=%p\n",m_netmpinfomanager->m_current_devmgr_sockptr);
			if(m_netmpinfomanager->m_current_devmgr_sockptr && (MCCfgInfo::instance()->get_mctoken().length() > 0))
			{
				if (DEVMGR_HEART_FLAGS != m_netmpinfomanager->m_current_dev_stat)
				{
					sr_printf(SR_PRINT_ERROR, "devmgr timeout\n");

					if (m_check_devmgr_heart_timer != 0)
					{
						sr_printf(SR_PRINT_INFO, "e_check_devmgr_timer deleteTimer=%p.\n", m_check_devmgr_heart_timer);
						//m_timermanger->DeleteTimer((TIMERID)m_check_devmgr_heart_timer);
						deleteTimer(m_check_devmgr_heart_timer);
						m_check_devmgr_heart_timer = (void*)0;
					}
					((DevmgrConnect*)(m_netmpinfomanager->m_current_devmgr_sockptr))->Close();
					SockErrorData devsockerro(m_netmpinfomanager->m_current_devmgr_sockptr);

					this->onSockError(&devsockerro);
				}
				else
				{
					// sr_printf(SR_PRINT_DEBUG,"devmgr heartbeat normal\n");
					m_netmpinfomanager->m_current_dev_stat = 0;
				}
			}
		}
		case e_hearttonetmp_timer: //������������ӵ�netmp��������
		{
			SRMsgs::IndNetMPHeartBeatOfMC indnetmpheart;
			indnetmpheart.set_mcid(MCCfgInfo::instance()->get_mcdeviceid());
			std::set<SR_uint32>::const_iterator netmps_citor = m_netmpinfomanager->m_connected_nemtps.begin();
			for(; netmps_citor != m_netmpinfomanager->m_connected_nemtps.end();++netmps_citor)
			{
				indnetmpheart.set_netmpid(*netmps_citor);
				std::map<SR_uint32,SR_void*>::iterator netmpptr_itor = m_netmpinfomanager->m_netmpsockptr.find(*netmps_citor);
				if(m_netmpinfomanager->m_netmpsockptr.end() == netmpptr_itor)
				{
					sr_printf(SR_PRINT_ERROR,"cannot find netmp=%u ptr\n",*netmps_citor);
				}
				else
				{
					SerialAndSend(netmpptr_itor->second,getMsgIdByClassName(IndNetMPHeartBeatOfMC),&indnetmpheart);
				}
			}
			
			break;
		}
		case e_hearttomp_timer: //������������ӵ�mp��������
		{
			SRMsgs::IndMCHeartBeatToMP indmpheart;
			indmpheart.set_mcid(MCCfgInfo::instance()->get_mcdeviceid());
			std::set<SR_uint32>::const_iterator mps_citor = m_mpinfomanager->m_connected_mps.begin();
			for (; mps_citor != m_mpinfomanager->m_connected_mps.end(); ++mps_citor)
			{
				indmpheart.set_mpid(*mps_citor);
				std::map<SR_uint32, SR_void*>::iterator mpptr_itor = m_mpinfomanager->m_mpsockptr.find(*mps_citor);
				if (m_mpinfomanager->m_mpsockptr.end() == mpptr_itor)
				{
					sr_printf(SR_PRINT_ERROR, "e_hearttomp_timer cannot find mp=%u ptr\n", *mps_citor);
				}
				else
				{
					SerialAndSend(mpptr_itor->second, getMsgIdByClassName(IndMCHeartBeatToMP), &indmpheart);
				}
			}

			break;
		}
		case e_hearttogw_timer: //������������ӵ�mp��������
		{
			SRMsgs::IndMCHeartBeatToGW indgwheart;
			indgwheart.set_mcid(MCCfgInfo::instance()->get_mcdeviceid());
			std::set<SR_uint32>::const_iterator gws_citor = m_gwinfomanager->m_connected_gws.begin();
			for (; gws_citor != m_gwinfomanager->m_connected_gws.end(); ++gws_citor)
			{
				indgwheart.set_gwid(*gws_citor);
				std::map<SR_uint32, SR_void*>::iterator gwptr_itor = m_gwinfomanager->m_gwsockptr.find(*gws_citor);
				if (m_gwinfomanager->m_gwsockptr.end() == gwptr_itor)
				{
					sr_printf(SR_PRINT_ERROR, "e_hearttogw_timer cannot find gw=%u ptr\n", *gws_citor);
				}
				else
				{
					SerialAndSend(gwptr_itor->second, getMsgIdByClassName(IndMCHeartBeatToGW), &indgwheart);
				}
			}

			break;
		}
		case e_hearttocrs_timer: //������������ӵ�crs��������
		{
			SRMsgs::IndMCHeartBeatToCRS indcrsheart;
			indcrsheart.set_mcid(MCCfgInfo::instance()->get_mcdeviceid());
			std::set<SR_uint32>::const_iterator crs_citor = m_crsinfomanager->m_connected_crss.begin();
			for (; crs_citor != m_crsinfomanager->m_connected_crss.end(); ++crs_citor)
			{
				indcrsheart.set_crsid(*crs_citor);
				std::map<SR_uint32, SR_void*>::iterator crsptr_itor = m_crsinfomanager->m_crssockptr.find(*crs_citor);
				if (m_crsinfomanager->m_crssockptr.end() == crsptr_itor)
				{
					sr_printf(SR_PRINT_ERROR, "e_hearttocrs_timer cannot find crs=%u ptr\n", *crs_citor);
				}
				else
				{
					SerialAndSend(crsptr_itor->second, getMsgIdByClassName(IndMCHeartBeatToCRS), &indcrsheart);
				}
			}

			break;
		}
		case e_hearttorelaysvr_timer: //������������ӵ�relayserver��������
		{
			SRMsgs::IndMCHeartBeatToRelayserver indrsheart;
			indrsheart.set_mcid(MCCfgInfo::instance()->get_mcdeviceid());
			std::set<SR_uint32>::const_iterator rs_citor = m_relaysvrinfomgr->m_connected_relaysvrs.begin();
			for (; rs_citor != m_relaysvrinfomgr->m_connected_relaysvrs.end(); ++rs_citor)
			{
				indrsheart.set_relaysvrid(*rs_citor);
				std::map<SR_uint32, SR_void*>::iterator rsptr_itor = m_relaysvrinfomgr->m_relaysvrsockptr.find(*rs_citor);
				if (m_relaysvrinfomgr->m_relaysvrsockptr.end() == rsptr_itor)
				{
					sr_printf(SR_PRINT_ERROR, "e_hearttorelaysvr_timer cannot find relayserver=%u ptr\n", *rs_citor);
				}
				else
				{
					SerialAndSend(rsptr_itor->second, getMsgIdByClassName(IndMCHeartBeatToRelayserver), &indrsheart);
				}
			}

			break;
		}
		case e_checkrelaymc_timeout: //���relaymc�Ƿ�ʱ
		{
			time_t timeNow;
			timeNow = time(NULL);

			std::set<SR_uint32> timeoutrelaymc;
			timeoutrelaymc.clear();
			
			for (std::map<SR_uint32, RelayMcInfo*>::const_iterator rm_citor = m_relaymcinfomgr->m_relaymcinfo.begin();
				rm_citor != m_relaymcinfomgr->m_relaymcinfo.end(); ++rm_citor)
			{
				if ((timeNow - rm_citor->second->m_relaymcdevtime) > (3 * MCCfgInfo::instance()->get_beatto_netmp_time()))
				{
					timeoutrelaymc.insert(rm_citor->first);
				}
			}

			for (std::set<SR_uint32>::iterator torm_itor = timeoutrelaymc.begin();
				torm_itor != timeoutrelaymc.end(); torm_itor++)
			{
				std::map<SR_uint32, RelayMcInfo*>::iterator frm_itor = m_relaymcinfomgr->m_relaymcinfo.find(*torm_itor);
				if (frm_itor != m_relaymcinfomgr->m_relaymcinfo.end())
				{
					sr_printf(SR_PRINT_CRIT, "e_checkrelaymc_timeout delete timeoutrelaymc=%u.\n", frm_itor->first);

					RelayMcInfo* pRelayMcInfo = NULL;
					pRelayMcInfo = frm_itor->second;
					if (pRelayMcInfo != NULL)
					{
						RelayMcInfoData* pRminfodata = new RelayMcInfoData();
						pRminfodata->m_relaymcid = pRelayMcInfo->m_relaymcdevid;
						pRminfodata->m_addordel = 2;
						PostToTerThread(e_update_relaymcinfo_inconf, pRminfodata);

						delete pRelayMcInfo;
						pRelayMcInfo = NULL;

						m_relaymcinfomgr->m_relaymcinfo.erase(frm_itor);
					}
				}
			}

			break;
		}
		case e_connect_devmgr_timer://һ���Զ�ʱ��,�������Ӷ��devmgr
		{
			////ֻ������devmgr ������ʧ��ʱ�Ż����,������һ�ֵ�ѭ��
			//for(std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
			//	devs_itor != m_netmpinfomanager->m_devmgrInfos.end();++devs_itor)
			//{
			//	DevSockInfo* pdsinfo = *devs_itor;
			//	if((NULL == pdsinfo->m_sockptr) && (e_DevMgr_sock_init == pdsinfo->m_devsockstat))
			//	{
			//		
			//		pdsinfo->m_sockptr = m_deviceMan->ConnectDevmgr(pdsinfo->m_ip.c_str(),pdsinfo->m_port);
			//		m_netmpinfomanager->m_current_dev_connectcnt++;//ÿ����һ�μ�1

			//		if (m_netmpinfomanager->m_current_dev_connectcnt == 3)
			//		{
			//			PostToTerThread(e_notify_to_check_confcrsinfo, 0);
			//		}

			//		if(false == m_deviceMan->isConnecting())
			//		{
			//			sr_printf(SR_PRINT_ERROR, "e_connect_devmgr_timer connect new devmgr(ip=%s, port=%d) is failed.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port);
			//			pdsinfo->m_devsockstat = e_DevMgr_sock_error;
			//			pdsinfo->m_sockptr = NULL;
			//		}
			//		else
			//		{
			//			sr_printf(SR_PRINT_NORMAL, "e_connect_devmgr_timer connect new devmgr(ip=%s, port=%d) is connecting, m_sockptr=%p.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port, pdsinfo->m_sockptr);
			//			pdsinfo->m_devsockstat = e_DevMgr_sock_connecting;
			//			m_netmpinfomanager->m_current_devmgr_sockptr = pdsinfo->m_sockptr;
			//			return;
			//		}
			//	}
			//}

			////��ʱû���ҵ����õ�devmgr,��ʱȥ��֮ǰ�����ϵ�
			//for(std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
			//	devs_itor != m_netmpinfomanager->m_devmgrInfos.end();++devs_itor)
			//{
			//	DevSockInfo* pdsinfo = *devs_itor;
			//	pdsinfo->m_devsockstat = e_DevMgr_sock_init;
			//	pdsinfo->m_sockptr = NULL;
			//}

			//sr_printf(SR_PRINT_WARN, "e_connect_devmgr_timer connect all devmgr is failed, so continue search other devmgr.\n");

			//createTimer(MCCfgInfo::instance()->get_reconnect_devmgr_time(),e_dispoable_timer,e_connect_devmgr_timer,0ull);	
			Msgid_Timerid_Pair* pidpair = NULL;
			pidpair = (Msgid_Timerid_Pair*)(ptd->m_timerdata);
			if (pidpair != NULL)
			{
				if (pidpair->m_ptimer != NULL)
				{
					for (std::list<SR_void*>::iterator timerdatalist_itor = m_connect_devmgr_timer_list.begin();
						timerdatalist_itor != m_connect_devmgr_timer_list.end(); /*timerdatalist_itor++*/)
					{
						sr_printf(SR_PRINT_CRIT, "e_connect_devmgr_timer in m_connect_devmgr_timer_list[msgid=0x%x, timerid=%p].\n",
							((Msgid_Timerid_Pair*)(*timerdatalist_itor))->m_msgid, ((Msgid_Timerid_Pair*)(*timerdatalist_itor))->m_ptimer);

						if (pidpair->m_ptimer == ((Msgid_Timerid_Pair*)(*timerdatalist_itor))->m_ptimer)
						{
							sr_printf(SR_PRINT_CRIT, "  xxxx  ==--->> e_connect_devmgr_timer deleteTimer=%p.\n", pidpair->m_ptimer);
							deleteTimer(pidpair->m_ptimer);
							m_connect_devmgr_timer_list.erase(timerdatalist_itor++);
						}
						else
						{
							timerdatalist_itor++;
						}
					}

					//ֻ������devmgr ������ʧ��ʱ�Ż����,������һ�ֵ�ѭ��
					for (std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
						devs_itor != m_netmpinfomanager->m_devmgrInfos.end(); ++devs_itor)
					{
						DevSockInfo* pdsinfo = *devs_itor;
						if ((NULL == pdsinfo->m_sockptr) && (e_DevMgr_sock_init == pdsinfo->m_devsockstat))
						{

							pdsinfo->m_sockptr = m_deviceMan->ConnectDevmgr(pdsinfo->m_ip.c_str(), pdsinfo->m_port);
							m_netmpinfomanager->m_current_dev_connectcnt++;//ÿ����һ�μ�1

							if (m_netmpinfomanager->m_current_dev_connectcnt == 3)
							{
								PostToTerThread(e_notify_to_check_confcrsinfo, 0);
							}

							if (false == m_deviceMan->isConnecting())
							{
								sr_printf(SR_PRINT_ERROR, "e_connect_devmgr_timer connect new devmgr(ip=%s, port=%d) is failed.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port);
								pdsinfo->m_devsockstat = e_DevMgr_sock_error;
								pdsinfo->m_sockptr = NULL;
							}
							else
							{
								sr_printf(SR_PRINT_NORMAL, "e_connect_devmgr_timer connect new devmgr(ip=%s, port=%d) is connecting, m_sockptr=%p.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port, pdsinfo->m_sockptr);
								pdsinfo->m_devsockstat = e_DevMgr_sock_connecting;
								m_netmpinfomanager->m_current_devmgr_sockptr = pdsinfo->m_sockptr;
								return;
							}
						}
					}

					//��ʱû���ҵ����õ�devmgr,��ʱȥ��֮ǰ�����ϵ�
					for (std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
						devs_itor != m_netmpinfomanager->m_devmgrInfos.end(); ++devs_itor)
					{
						DevSockInfo* pdsinfo = *devs_itor;
						pdsinfo->m_devsockstat = e_DevMgr_sock_init;
						pdsinfo->m_sockptr = NULL;
					}

					m_netmpinfomanager->m_connect_dev_loopcnt++;//ÿ����һ�����Ӻ��1
					if (m_netmpinfomanager->m_connect_dev_loopcnt == MCCfgInfo::instance()->get_reconnect_devmgr_loopcnt())
					{
						sr_printf(SR_PRINT_WARN, "e_connect_devmgr_timer devmgr connect all devmgr third loop is failed, so end all conf.\n");
						PostToTerThread(e_notify_to_end_conf, 0);
					}
					sr_printf(SR_PRINT_WARN, "e_connect_devmgr_timer connect all devmgr is failed, so continue search other devmgr.\n");

					Msgid_Timerid_Pair* ppair = NULL;
					ppair = new Msgid_Timerid_Pair();
					if (ppair != NULL)
					{
						ppair->m_msgid = e_connect_devmgr_timer;
						ppair->m_ptimer = NULL;
						//��ʱ��ʱ��ǳ��̵�ʱ��,����֮�� ppair ��δ��ֵ ��Ӱ����
						ppair->m_ptimer = createTimer(MCCfgInfo::instance()->get_reconnect_devmgr_time(), e_dispoable_timer, e_connect_devmgr_timer, (SR_uint64)ppair);
						if (ppair->m_ptimer != NULL)
						{
							sr_printf(SR_PRINT_CRIT, "e_connect_devmgr_timer connect all devmgr is failed,ppair=%p [msgid=0x%x, timerid=%p] push_back list for continue search other devmgr.\n", ppair, ppair->m_msgid, ppair->m_ptimer);
							m_connect_devmgr_timer_list.push_back(ppair);
						}
						else
						{
							sr_printf(SR_PRINT_CRIT, "e_connect_devmgr_timer connect all devmgr is failed,createTimer is null.\n");
							delete ppair;
						}
					}
					else
					{
						sr_printf(SR_PRINT_CRIT, "e_connect_devmgr_timer connect all devmgr is failed,new ppair is null.\n");
					}
				}
				else
				{
					sr_printf(SR_PRINT_CRIT, "error in e_connect_devmgr_timer data,ppair=%p [msgid=0x%x, timerid=%p].\n", pidpair, pidpair->m_msgid, pidpair->m_ptimer);
				}

				delete pidpair;
				pidpair = NULL;
			}
			else
			{
				sr_printf(SR_PRINT_CRIT, "e_connect_devmgr_timer pidpair is null.\n");
			}
			break;
		}
		case e_re_register_timer:
		{
			Msgid_Timerid_Pair* pidpair = NULL;
			pidpair = (Msgid_Timerid_Pair*)(ptd->m_timerdata);
			if (pidpair != NULL)
			{
				if (pidpair->m_ptimer != NULL)
				{
					sr_printf(SR_PRINT_CRIT, "in e_re_register_timer data,ppair=%p [msgid=0x%x, timerid=%p].\n", pidpair, pidpair->m_msgid, pidpair->m_ptimer);

					for (std::list<SR_void*>::iterator timerdatalist_itor = m_re_register_timer_list.begin();
						timerdatalist_itor != m_re_register_timer_list.end(); /*timerdatalist_itor++*/)
					{
						sr_printf(SR_PRINT_CRIT, "e_re_register_timer in m_re_register_timer_list[msgid=0x%x, timerid=%p].\n",
							((Msgid_Timerid_Pair*)(*timerdatalist_itor))->m_msgid, ((Msgid_Timerid_Pair*)(*timerdatalist_itor))->m_ptimer);

						if (pidpair->m_ptimer == ((Msgid_Timerid_Pair*)(*timerdatalist_itor))->m_ptimer)
						{
							sr_printf(SR_PRINT_CRIT, "  xxxx  ==--->> e_re_register_timer deleteTimer=%p.\n", pidpair->m_ptimer);
							deleteTimer(pidpair->m_ptimer);
							m_re_register_timer_list.erase(timerdatalist_itor++);
						}
						else
						{
							timerdatalist_itor++;
						}
					}
					//m_re_register_timer_list.clear();
				}
				else
				{
					sr_printf(SR_PRINT_CRIT, "error in e_re_register_timer data,ppair=%p [msgid=0x%x, timerid=%p].\n", pidpair, pidpair->m_msgid, pidpair->m_ptimer);
				}

				delete pidpair;
				pidpair = NULL;

				PostToTerThread(e_notify_to_register_devmgr, 0);
			}
			else
			{
				sr_printf(SR_PRINT_CRIT, "e_re_register_timer pidpair is null.\n");
			}
			break;
		}
	}	
}

SR_void DevMgrNetMPProcessThread::onConnectSuccess(const SockRelateData* psrd)
{
	sr_printf(SR_PRINT_NORMAL, "=== devmgr/netmp/mp/gw-> mc DevMgrNetMPProcessThread::onConnectSuccess m_pSocketptr=%p,socktype=0x%x connect ok\n", psrd->m_pSocket, psrd->m_socktype);
	SR_uint32 socktype = psrd->m_socktype;
	if(kdevmgr_sock_type == socktype)
	{
		SR_bool bFindSocket = false;
		//m_netmpinfomanager->m_current_dev_stat = DEVMGR_HEART_FLAGS;
		//sr_printf(SR_PRINT_INFO,"devmgr connnect ok,sockptr=%p\n",psrd->m_pSocket);
		for(std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
			devs_itor != m_netmpinfomanager->m_devmgrInfos.end();++devs_itor)
		{
			DevSockInfo* pdsinfo = *devs_itor;
			if((psrd->m_pSocket == pdsinfo->m_sockptr))
			{
				for (std::list<SR_void*>::iterator timerdatalist_itor = m_connect_devmgr_timer_list.begin();
					timerdatalist_itor != m_connect_devmgr_timer_list.end();/* timerdatalist_itor++*/)
				{
					Msgid_Timerid_Pair* pidpair = NULL;
					pidpair = (Msgid_Timerid_Pair*)(*timerdatalist_itor);
					if (pidpair != NULL)
					{
						sr_printf(SR_PRINT_CRIT, "onConnectSuccess in m_connect_devmgr_timer_list[msgid=0x%x, timerid=%p].\n", pidpair->m_msgid, pidpair->m_ptimer);

						if (NULL != pidpair->m_ptimer)
						{
							sr_printf(SR_PRINT_CRIT, "  xxxx  ==--->> onConnectSuccess deleteTimer=%p.\n", pidpair->m_ptimer);
							deleteTimer(pidpair->m_ptimer);
							m_connect_devmgr_timer_list.erase(timerdatalist_itor++);

							delete pidpair;
							pidpair = NULL;
						}
						else
						{
							timerdatalist_itor++;
						}
					}
				}

				bFindSocket = true;
				m_netmpinfomanager->m_current_dev_stat = DEVMGR_HEART_FLAGS;
				m_netmpinfomanager->m_current_devmgr_sockptr = pdsinfo->m_sockptr;
				pdsinfo->m_devsockstat = e_DevMgr_sock_connect_ok;
				m_netmpinfomanager->m_current_dev_connectcnt = 0;

				m_netmpinfomanager->m_connect_dev_loopcnt = 0;
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

				PostToTerThread(e_notify_to_register_devmgr, 0);
				sr_printf(SR_PRINT_INFO, "devmgr ip = %s port = %d  connnect ok,sockptr=%p,then to register devmgr.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port, psrd->m_pSocket);

				break;
			}
		}

		if (bFindSocket == false)
		{
			sr_printf(SR_PRINT_ERROR, "devmgr sockptr=%p not in devmgr set.\n", psrd->m_pSocket);
		}
	}
	else if(knetmp_sock_type == psrd->m_socktype)//���Ѿ��������ӵ�netmpid<->sock
	{
		sr_printf(SR_PRINT_INFO, "netmpid=%u,m_pSocketptr=%p,connect ok\n", psrd->m_relatedata, psrd->m_pSocket);

		std::map<SR_uint32, SR_void*>::iterator netmpidsockptr_itor = m_netmpinfomanager->m_netmpsockptr.find(psrd->m_relatedata);
		if (netmpidsockptr_itor != m_netmpinfomanager->m_netmpsockptr.end())
		{
			sr_printf(SR_PRINT_ERROR, " DevMgrNetMPProcessThread::onConnectSuccess erase netmpid=%u,sockptr=%p in NetMPInfoManager::m_netmpsockptr\n", psrd->m_relatedata, psrd->m_pSocket);
			m_netmpinfomanager->m_netmpsockptr.erase(netmpidsockptr_itor);// netmpidsockptr_itor->second��Ӧ��һ������psrd->m_pSocket,������DevmgrConnect,�˴����ܴ����ڴ�й©����������
		}
		sr_printf(SR_PRINT_INFO, " DevMgrNetMPProcessThread::onConnectSuccess insert netmpid=%u,sockptr=%p to NetMPInfoManager::m_netmpsockptr\n", psrd->m_relatedata, psrd->m_pSocket);
		m_netmpinfomanager->m_netmpsockptr[psrd->m_relatedata]= psrd->m_pSocket;

		NetMPInfoData* pninfodata = new NetMPInfoData();
		pninfodata->m_netmpid = psrd->m_relatedata;
		std::map<SR_uint32,NetMPInfo*>::iterator netmpinfo_itor = m_netmpinfomanager->m_netmpinfo.find(psrd->m_relatedata);
		if(netmpinfo_itor == m_netmpinfomanager->m_netmpinfo.end())
		{
			sr_printf(SR_PRINT_ERROR,"onConnectSuccess ERROR!!! can not find such netmpinfo %u\n",psrd->m_relatedata);
			delete pninfodata;
		}
		else
		{
			NetMPInfo* pnmi = netmpinfo_itor->second;
			pnmi->m_connectionstate = e_Netmp_sock_connect_ok;
			m_netmpinfomanager->m_connected_nemtps.insert(pnmi->m_netmpdeviceid);//����ͳ���ѽ���tcp���ӵ�netmp��Ŀ
			pninfodata->m_load = pnmi->m_load;
			pninfodata->m_load2 = pnmi->m_load2;
			pninfodata->m_mapinternetips.clear();

			std::list<std::string> mapinternetiplist;
			mapinternetiplist.clear();
			for (std::list<std::string>::iterator mapip_itor = pnmi->m_mapinternetips.begin();
				mapip_itor != pnmi->m_mapinternetips.end(); mapip_itor++)
			{
				if ((*mapip_itor).length() > 0)
				{
					pninfodata->m_mapinternetips.push_back((*mapip_itor));
					mapinternetiplist.push_back((*mapip_itor));
				}
			}
			
			if(pnmi->m_load <= MCCfgInfo::instance()->get_netmp_lowload())
			{
				//insert �ɹ�orʧ�ܲ�Ӱ��
				m_netmpinfomanager->m_idle_netmps.insert(pnmi->m_netmpdeviceid);
			}
			else if((pnmi->m_load > MCCfgInfo::instance()->get_netmp_lowload()) && (pnmi->m_load <= MCCfgInfo::instance()->get_netmp_highload()))
			{
				m_netmpinfomanager->m_highload_netmps.insert(pnmi->m_netmpdeviceid);
			}
			pninfodata->m_nettype = pnmi->m_nettype;
			pninfodata->m_max_terms = pnmi->m_max_terms;
			pninfodata->m_max_bandwidth = pnmi->m_max_bandwidth;
			SR_char srcIPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
			struct in_addr srcinaddr; // IPv4��ַ�ṹ��
			srcinaddr.s_addr = pnmi->m_ip;
			inet_ntop(AF_INET, (void *)&srcinaddr, srcIPdotdec, sizeof(srcIPdotdec));
			SR_char srIPPortdotdec[128] = {};
			pninfodata->m_ip = srcIPdotdec;
			sprintf(srIPPortdotdec, "%s:%d", srcIPdotdec, pnmi->m_port);
			pninfodata->m_ipandport = srIPPortdotdec;
			PostToTerThread(e_notify_connected_netmp, new ConnectedNetMPData(psrd->m_relatedata, pnmi->m_groupid, psrd->m_pSocket, mapinternetiplist));
			PostToTerThread(e_update_netmpinfo_inconf, pninfodata);
			if(!m_periodtimer_hearttonetmp)
				m_periodtimer_hearttonetmp = createTimer(MCCfgInfo::instance()->get_beatto_netmp_time(),e_periodicity_timer,e_hearttonetmp_timer,0ull);	

			//SRMsgs::IndNetMPConnStatusInMC inddevnetmpconn;
			//inddevnetmpconn.set_deviceid(SRMC::MCCfgInfo::instance()->get_mcdeviceid());
			//inddevnetmpconn.set_token(SRMC::MCCfgInfo::instance()->get_mctoken());
			//inddevnetmpconn.set_netmpid(pnmi->m_netmpdeviceid);
			//inddevnetmpconn.set_connstatus(1); // ���ӳɹ�
			//inddevnetmpconn.set_groupid(pnmi->m_groupid);
			//SerialAndSendDevmgr_nobuf(getMsgIdByClassName(IndNetMPConnStatusInMC), &inddevnetmpconn);
			////SerialAndSendDevmgr_buffered(getMsgIdByClassName(IndNetMPConnStatusInMC), &inddevnetmpconn);
		}		
	}
	else if (kmp_sock_type == psrd->m_socktype)//���Ѿ��������ӵ�mpid<->sock
	{
		sr_printf(SR_PRINT_INFO, "mpid=%u,m_pSocketptr=%p,connect ok\n", psrd->m_relatedata, psrd->m_pSocket);

		std::map<SR_uint32, SR_void*>::iterator mpidsockptr_itor = m_mpinfomanager->m_mpsockptr.find(psrd->m_relatedata);
		if (mpidsockptr_itor != m_mpinfomanager->m_mpsockptr.end())
		{
			sr_printf(SR_PRINT_ERROR, " DevMgrNetMPProcessThread::onConnectSuccess erase mpid=%u,sockptr=%p in MPInfoManager::m_mpsockptr\n", psrd->m_relatedata, psrd->m_pSocket);
			m_mpinfomanager->m_mpsockptr.erase(mpidsockptr_itor);// mpidsockptr_itor->second��Ӧ��һ������psrd->m_pSocket,������DevmgrConnect,�˴����ܴ����ڴ�й©����������
		}
		sr_printf(SR_PRINT_INFO, " DevMgrNetMPProcessThread::onConnectSuccess insert mpid=%u,sockptr=%p to MPInfoManager::m_mpsockptr\n", psrd->m_relatedata, psrd->m_pSocket);
		m_mpinfomanager->m_mpsockptr[psrd->m_relatedata] = psrd->m_pSocket;

		MPInfoData* pMpinfodata = new MPInfoData();
		pMpinfodata->m_mpid = psrd->m_relatedata;
		std::map<SR_uint32, MPInfo*>::iterator mpinfo_itor = m_mpinfomanager->m_mpinfo.find(psrd->m_relatedata);
		if (mpinfo_itor == m_mpinfomanager->m_mpinfo.end())
		{
			sr_printf(SR_PRINT_ERROR, "onConnectSuccess ERROR!!! can not find such mpinfo %u\n", psrd->m_relatedata);
			delete pMpinfodata;
		}
		else
		{
			MPInfo* pMpinfo = mpinfo_itor->second;
			pMpinfo->m_connectionstate = e_Mp_sock_connect_ok;
			m_mpinfomanager->m_connected_mps.insert(pMpinfo->m_mpdeviceid);//����ͳ���ѽ���tcp���ӵ�mp��Ŀ
			pMpinfodata->m_load = pMpinfo->m_load;
			pMpinfodata->m_load2 = pMpinfo->m_load2;

			if (pMpinfo->m_load <= MCCfgInfo::instance()->get_netmp_lowload())// ������chen songhua������MP��ʱ������netmp��������
			{
				//insert �ɹ�orʧ�ܲ�Ӱ��
				m_mpinfomanager->m_idle_mps.insert(pMpinfo->m_mpdeviceid);
			}
			else if ((pMpinfo->m_load > MCCfgInfo::instance()->get_netmp_lowload()) && (pMpinfo->m_load <= MCCfgInfo::instance()->get_netmp_highload()))// ������chen songhua������MP��ʱ������netmp��������
			{
				m_mpinfomanager->m_highload_mps.insert(pMpinfo->m_mpdeviceid);
			}
			PostToTerThread(e_notify_connected_mp, new ConnectedMPData(psrd->m_relatedata, psrd->m_pSocket));
			PostToTerThread(e_update_mpinfo_inconf, pMpinfodata);
			if (!m_periodtimer_hearttomp)
				m_periodtimer_hearttomp = createTimer(MCCfgInfo::instance()->get_beatto_netmp_time(), e_periodicity_timer, e_hearttomp_timer, 0ull); // ������chen songhua������MP��ʱ������netmp��������
		}
	}
	else if (kgw_sock_type == psrd->m_socktype)//���Ѿ��������ӵ�gwid<->sock
	{
		sr_printf(SR_PRINT_INFO, "gwid=%u,m_pSocketptr=%p,connect ok\n", psrd->m_relatedata, psrd->m_pSocket);

		std::map<SR_uint32, SR_void*>::iterator gwidsockptr_itor = m_gwinfomanager->m_gwsockptr.find(psrd->m_relatedata);
		if (gwidsockptr_itor != m_gwinfomanager->m_gwsockptr.end())
		{
			sr_printf(SR_PRINT_ERROR, " DevMgrNetMPProcessThread::onConnectSuccess erase gwid=%u,sockptr=%p in GWInfoManager::m_gwsockptr\n", psrd->m_relatedata, psrd->m_pSocket);
			m_gwinfomanager->m_gwsockptr.erase(gwidsockptr_itor);// gwidsockptr_itor->second��Ӧ��һ������psrd->m_pSocket,������DevmgrConnect,�˴����ܴ����ڴ�й©����������
		}
		sr_printf(SR_PRINT_INFO, " DevMgrNetMPProcessThread::onConnectSuccess insert gwid=%u,sockptr=%p to GWInfoManager::m_gwsockptr\n", psrd->m_relatedata, psrd->m_pSocket);
		m_gwinfomanager->m_gwsockptr[psrd->m_relatedata] = psrd->m_pSocket;

		GWInfoData* pGWinfodata = new GWInfoData();
		pGWinfodata->m_gwid = psrd->m_relatedata;
		std::map<SR_uint32, GWInfo*>::iterator gwinfo_itor = m_gwinfomanager->m_gwinfo.find(psrd->m_relatedata);
		if (gwinfo_itor == m_gwinfomanager->m_gwinfo.end())
		{
			sr_printf(SR_PRINT_ERROR, "onConnectSuccess ERROR!!! can not find such gwinfo %u\n", psrd->m_relatedata);
			delete pGWinfodata;
		}
		else
		{
			GWInfo* pGWinfo = gwinfo_itor->second;
			pGWinfo->m_connectionstate = e_GW_sock_connect_ok;
			m_gwinfomanager->m_connected_gws.insert(pGWinfo->m_gwdeviceid);//����ͳ���ѽ���tcp���ӵ�mp��Ŀ
			pGWinfodata->m_load = pGWinfo->m_load; // gw�ն���
			pGWinfodata->m_load2 = pGWinfo->m_load2; // gw��cpuռ����

			if (pGWinfo->m_load <= MCCfgInfo::instance()->get_netmp_lowload())// ������chen songhua������MP��ʱ������netmp��������
			{
				//insert �ɹ�orʧ�ܲ�Ӱ��
				m_gwinfomanager->m_idle_gws.insert(pGWinfo->m_gwdeviceid);
			}
			else if ((pGWinfo->m_load > MCCfgInfo::instance()->get_netmp_lowload()) && (pGWinfo->m_load <= MCCfgInfo::instance()->get_netmp_highload()))// ������chen songhua������MP��ʱ������netmp��������
			{
				m_gwinfomanager->m_highload_gws.insert(pGWinfo->m_gwdeviceid);
			}
			PostToTerThread(e_notify_connected_gw, new ConnectedGWData(psrd->m_relatedata, psrd->m_pSocket));
			PostToTerThread(e_update_gwinfo_inconf, pGWinfodata);
			if (!m_periodtimer_hearttogw)
				m_periodtimer_hearttogw = createTimer(MCCfgInfo::instance()->get_beatto_netmp_time(), e_periodicity_timer, e_hearttogw_timer, 0ull); // ������chen songhua������MP��ʱ������netmp��������
		}
	}
	else if (kcrs_sock_type == psrd->m_socktype)//���Ѿ��������ӵ�crsid<->sock
	{
		sr_printf(SR_PRINT_INFO, "crsid=%u,m_pSocketptr=%p,connect ok\n", psrd->m_relatedata, psrd->m_pSocket);

		std::map<SR_uint32, SR_void*>::iterator crsidsockptr_itor = m_crsinfomanager->m_crssockptr.find(psrd->m_relatedata);
		if (crsidsockptr_itor != m_crsinfomanager->m_crssockptr.end())
		{
			sr_printf(SR_PRINT_ERROR, " DevMgrNetMPProcessThread::onConnectSuccess erase crsid=%u,sockptr=%p in CRSInfoManager::m_mpsockptr\n", psrd->m_relatedata, psrd->m_pSocket);
			m_crsinfomanager->m_crssockptr.erase(crsidsockptr_itor);// crsidsockptr_itor->second��Ӧ��һ������psrd->m_pSocket,������DevmgrConnect,�˴����ܴ����ڴ�й©����������
		}
		sr_printf(SR_PRINT_INFO, " DevMgrNetMPProcessThread::onConnectSuccess insert crsid=%u,sockptr=%p to CRSInfoManager::m_mpsockptr\n", psrd->m_relatedata, psrd->m_pSocket);
		m_crsinfomanager->m_crssockptr[psrd->m_relatedata] = psrd->m_pSocket;

		CRSInfoData* pCRSinfodata = new CRSInfoData();
		pCRSinfodata->m_crsid = psrd->m_relatedata;
		std::map<SR_uint32, CRSInfo*>::iterator crsinfo_itor = m_crsinfomanager->m_crsinfo.find(psrd->m_relatedata);
		if (crsinfo_itor == m_crsinfomanager->m_crsinfo.end())
		{
			sr_printf(SR_PRINT_ERROR, "onConnectSuccess ERROR!!! can not find such crsinfo %u\n", psrd->m_relatedata);
			delete pCRSinfodata;
		}
		else
		{
			CRSInfo* pCRSinfo = crsinfo_itor->second;
			pCRSinfo->m_connectionstate = e_dev_sock_connect_ok;
			m_crsinfomanager->m_connected_crss.insert(pCRSinfo->m_crsdeviceid);//����ͳ���ѽ���tcp���ӵ�crs��Ŀ
			pCRSinfodata->m_load = pCRSinfo->m_load;
			pCRSinfodata->m_load2 = pCRSinfo->m_load2;

			//if (pCRSinfo->m_load <= MCCfgInfo::instance()->get_netmp_lowload())// ������chen songhua������MP��ʱ������netmp��������
			//{
			//	//insert �ɹ�orʧ�ܲ�Ӱ��
			//	m_crsinfomanager->m_idle_crss.insert(pCRSinfo->m_crsdeviceid);
			//}
			//else if ((pCRSinfo->m_load > MCCfgInfo::instance()->get_netmp_lowload()) && (pCRSinfo->m_load <= MCCfgInfo::instance()->get_netmp_highload()))// ������chen songhua������CRS��ʱ������netmp��������
			//{
			//	m_crsinfomanager->m_highload_crss.insert(pCRSinfo->m_crsdeviceid);
			//}

			// ��ǰcrsʣ�������������1GB����Ϊ�ǿ��п��õ�
			if (pCRSinfo->m_diskremaincapacity >= 1)
			{
				//insert �ɹ�orʧ�ܲ�Ӱ��
				m_crsinfomanager->m_idle_crss.insert(pCRSinfo->m_crsdeviceid);
			}
			PostToTerThread(e_notify_connected_crs, new ConnectedCRSData(psrd->m_relatedata, psrd->m_pSocket));
			PostToTerThread(e_update_crsinfo_inconf, pCRSinfodata);
			if (!m_periodtimer_hearttocrs)
				m_periodtimer_hearttocrs = createTimer(MCCfgInfo::instance()->get_beatto_netmp_time(), e_periodicity_timer, e_hearttocrs_timer, 0ull); // ������chen songhua������CRS��ʱ������netmp��������
		}
	}
	else if (krelayserver_sock_type == psrd->m_socktype)//���Ѿ��������ӵ�crsid<->sock
	{
		sr_printf(SR_PRINT_INFO, "relayserverid=%u,m_pSocketptr=%p,connect ok\n", psrd->m_relatedata, psrd->m_pSocket);

		std::map<SR_uint32, SR_void*>::iterator rssockptr_itor = m_relaysvrinfomgr->m_relaysvrsockptr.find(psrd->m_relatedata);
		if (rssockptr_itor != m_relaysvrinfomgr->m_relaysvrsockptr.end())
		{
			sr_printf(SR_PRINT_ERROR, " DevMgrNetMPProcessThread::onConnectSuccess erase relayserverid=%u,sockptr=%p in RelayServerInfoManager::m_pSocket\n", psrd->m_relatedata, psrd->m_pSocket);
			m_relaysvrinfomgr->m_relaysvrsockptr.erase(rssockptr_itor);// crsidsockptr_itor->second��Ӧ��һ������psrd->m_pSocket,������DevmgrConnect,�˴����ܴ����ڴ�й©����������
		}
		sr_printf(SR_PRINT_INFO, " DevMgrNetMPProcessThread::onConnectSuccess insert relayserverid=%u,sockptr=%p to RelayServerInfoManager::m_pSocket\n", psrd->m_relatedata, psrd->m_pSocket);
		m_relaysvrinfomgr->m_relaysvrsockptr[psrd->m_relatedata] = psrd->m_pSocket;

		RelayServerInfoData* pRSinfodata = new RelayServerInfoData();
		pRSinfodata->m_relayserverid = psrd->m_relatedata;
		std::map<SR_uint32, RelayServerInfo*>::iterator rsinfo_itor = m_relaysvrinfomgr->m_relaysvrinfo.find(psrd->m_relatedata);
		if (rsinfo_itor == m_relaysvrinfomgr->m_relaysvrinfo.end())
		{
			sr_printf(SR_PRINT_ERROR, "onConnectSuccess ERROR!!! can not find such relayserverinfo %u\n", psrd->m_relatedata);
			delete pRSinfodata;
		}
		else
		{
			RelayServerInfo* pRSinfo = rsinfo_itor->second;
			pRSinfo->m_connectionstate = e_dev_sock_connect_ok;
			m_relaysvrinfomgr->m_connected_relaysvrs.insert(pRSinfo->m_relaysvrdevid);//����ͳ���ѽ���tcp���ӵ�relayserver��Ŀ
			pRSinfodata->m_ternum = pRSinfo->m_ternum;
			pRSinfodata->m_tx = pRSinfo->m_tx;

			if (pRSinfo->m_tx <= MCCfgInfo::instance()->get_netmp_lowload())// ����netmp��������
			{
				//insert �ɹ�orʧ�ܲ�Ӱ��
				m_relaysvrinfomgr->m_idle_relaysvrs.insert(pRSinfo->m_relaysvrdevid);
			}
			else if ((pRSinfo->m_tx > MCCfgInfo::instance()->get_netmp_lowload()) && (pRSinfo->m_tx <= MCCfgInfo::instance()->get_netmp_highload()))// ����netmp��������
			{
				m_relaysvrinfomgr->m_highload_relaysvrs.insert(pRSinfo->m_relaysvrdevid);
			}
			PostToTerThread(e_notify_connected_relayserver, new ConnectedRelayServerData(psrd->m_relatedata, psrd->m_pSocket));
			PostToTerThread(e_update_relayserverinfo_inconf, pRSinfodata);
			if (!m_periodtimer_hearttorelayserver)
				m_periodtimer_hearttorelayserver = createTimer(MCCfgInfo::instance()->get_beatto_netmp_time(), e_periodicity_timer, e_hearttorelaysvr_timer, 0ull); // ����netmp��������
		}
	}
}
SR_void DevMgrNetMPProcessThread::processRspRegister(const SRMsgs::RspRegister* s)
{
	if((!s->has_isok())||(!s->has_deviceid())||(!s->has_token()))
	{
		sr_printf(SR_PRINT_ERROR, "RspRegister param is null,then after(%ds) to re-Register.\n", MCCfgInfo::instance()->get_reconnect_devmgr_time());
		//m_isRegisterOK = false;

		//SR_bool bCanregister = false;
		//for (std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
		//	devs_itor != m_netmpinfomanager->m_devmgrInfos.end(); ++devs_itor)
		//{
		//	DevSockInfo* pdsinfo = *devs_itor;
		//	if (m_netmpinfomanager->m_current_devmgr_sockptr != NULL
		//		&& pdsinfo->m_devsockstat == e_DevMgr_sock_connect_ok)
		//	{
		//		bCanregister = true;
		//		break;
		//	}
		//}

		//if (bCanregister)
		//{
		//	PostToTerThread(e_notify_to_register_devmgr, 0);
		//} 
		//else
		//{
		//	// ���붨ʱ��
		//}

		////PostToTerThread(e_notify_to_register_devmgr, 0);
		//createTimer(MCCfgInfo::instance()->get_reconnect_devmgr_time(), e_dispoable_timer, e_re_register_timer, 0ull);

		Msgid_Timerid_Pair* ppair = NULL;
		ppair = new Msgid_Timerid_Pair();
		if (ppair != NULL)
		{
			ppair->m_msgid = e_re_register_timer;
			ppair->m_ptimer = NULL;
			//��ʱ��ʱ��ǳ��̵�ʱ��,����֮�� ppair ��δ��ֵ ��Ӱ����
			ppair->m_ptimer = createTimer(MCCfgInfo::instance()->get_reconnect_devmgr_time(), e_dispoable_timer, e_re_register_timer, (SR_uint64)ppair);
			if (ppair->m_ptimer != NULL)
			{
				sr_printf(SR_PRINT_CRIT, "RspRegister param is null,ppair=%p [msgid=0x%x, timerid=%p] push_back list for re-Register.\n", ppair, ppair->m_msgid, ppair->m_ptimer);
				m_re_register_timer_list.push_back(ppair);
			}
			else
			{
				sr_printf(SR_PRINT_CRIT, "RspRegister param is null,createTimer is null.\n");
				delete ppair;
			}
		}
		else
		{
			sr_printf(SR_PRINT_CRIT, "RspRegister param is null,new ppair is null.\n");
		}
		return;
	}
	if(false == s->isok()
		|| s->deviceid() == 0)
	{
		sr_printf(SR_PRINT_INFO, "MC Register DevMgr Fail,then after(%ds) to re-Register.\n", MCCfgInfo::instance()->get_reconnect_devmgr_time());
		//m_isRegisterOK = false;

		////PostToTerThread(e_notify_to_register_devmgr, 0);
		//createTimer(MCCfgInfo::instance()->get_reconnect_devmgr_time(), e_dispoable_timer, e_re_register_timer, 0ull);

		Msgid_Timerid_Pair* ppair = NULL;
		ppair = new Msgid_Timerid_Pair();
		if (ppair != NULL)
		{
			ppair->m_msgid = e_re_register_timer;
			ppair->m_ptimer = NULL;
			//��ʱ��ʱ��ǳ��̵�ʱ��,����֮�� ppair ��δ��ֵ ��Ӱ����
			ppair->m_ptimer = createTimer(MCCfgInfo::instance()->get_reconnect_devmgr_time(), e_dispoable_timer, e_re_register_timer, (SR_uint64)ppair);
			if (ppair->m_ptimer != NULL)
			{
				sr_printf(SR_PRINT_CRIT, "MC Register DevMgr Fail,ppair=%p [msgid=0x%x, timerid=%p] push_back list for re-Register.\n", ppair, ppair->m_msgid, ppair->m_ptimer);
				m_re_register_timer_list.push_back(ppair);
			}
			else
			{
				sr_printf(SR_PRINT_CRIT, "MC Register DevMgr Fail,createTimer is null.\n");
				delete ppair;
			}
		}
		else
		{
			sr_printf(SR_PRINT_CRIT, "MC Register DevMgr Fail,new ppair is null.\n");
		}
		return;
	}

	//m_isRegisterOK = true;
	
	if((void*)0 == m_periodtimer_updatenetmp)
		m_periodtimer_updatenetmp = createTimer(MCCfgInfo::instance()->get_update_netmpinfo_time(),
														e_periodicity_timer,e_update_netmp_timer,0ull);

	// �������Ӻ�mp,�������ȫ��SR�ն˵Ļ��ò���mp,���ϻ�ȡmp��Ϣ������mp�е��˷���Դ
	//if (MCCfgInfo::instance()->get_use_videomixer() != 0)
	{
		// MP��ʱ������netmp��������
		if ((void*)0 == m_periodtimer_updatemp)
			m_periodtimer_updatemp = createTimer(MCCfgInfo::instance()->get_update_netmpinfo_time(),
			e_periodicity_timer, e_update_mp_timer, 0ull);
	}

	if (MCCfgInfo::instance()->get_callinvite() != 0)
	{
		// GW��ʱ������netmp��������
		if ((void*)0 == m_periodtimer_updategw)
			m_periodtimer_updategw = createTimer(MCCfgInfo::instance()->get_update_netmpinfo_time(),
			e_periodicity_timer, e_update_gw_timer, 0ull);
	}

	// CRS��ʱ������netmp��������
	if ((void*)0 == m_periodtimer_updatecrs)
		m_periodtimer_updatecrs = createTimer(MCCfgInfo::instance()->get_update_netmpinfo_time(),
		e_periodicity_timer, e_update_crs_timer, 0ull);

	// SRS��ʱ������netmp��������
	if ((void*)0 == m_periodtimer_updatesrs)
		m_periodtimer_updatesrs = createTimer(MCCfgInfo::instance()->get_update_netmpinfo_time(),
		e_periodicity_timer, e_update_srs_timer, 0ull);

	// ϵͳ��ǰ������Ϣ��ʱ���µĶ�ʱ������netmp��������
	if ((void*)0 == m_periodtimer_updatesyscurload)
		m_periodtimer_updatesyscurload = createTimer(MCCfgInfo::instance()->get_update_syscurload_time(),
		e_periodicity_timer, e_update_sys_load_timer, 0ull);

	// relayserver��ʱ������netmp��������
	if ((void*)0 == m_periodtimer_updaterelayserver)
		m_periodtimer_updaterelayserver = createTimer(MCCfgInfo::instance()->get_update_netmpinfo_time(),
		e_periodicity_timer, e_update_relayserver_timer, 0ull);

	// relaymc��ʱ������netmp��������
	if ((void*)0 == m_periodtimer_updaterelaymc)
		m_periodtimer_updaterelaymc = createTimer(MCCfgInfo::instance()->get_update_netmpinfo_time(),
		e_periodicity_timer, e_update_relaymc_timer, 0ull);

	if((void*)0 == m_periodtimer_hearttodevmgr)
		m_periodtimer_hearttodevmgr = createTimer(MCCfgInfo::instance()->get_beatto_devmgr_time(),
														e_periodicity_timer,e_hearttodevmgr_timer,0ull);
	m_netmpinfomanager->m_current_dev_stat = DEVMGR_HEART_FLAGS;
	if(!m_check_devmgr_heart_timer)
	{
		sr_printf(SR_PRINT_NORMAL,"next startup check devmgr_heart_timer\n");
		m_check_devmgr_heart_timer = createTimer(MCCfgInfo::instance()->get_check_devmgr_time(),e_periodicity_timer,e_check_devmgr_timer,0llu);
	}

	for (std::list<SR_void*>::iterator timerdatalist_itor = m_re_register_timer_list.begin();
		timerdatalist_itor != m_re_register_timer_list.end(); /*timerdatalist_itor++*/)
	{
		Msgid_Timerid_Pair* pidpair = NULL;
		pidpair = (Msgid_Timerid_Pair*)(*timerdatalist_itor);
		if (pidpair != NULL)
		{
			sr_printf(SR_PRINT_CRIT, "processRspRegister in m_re_register_timer_list[msgid=0x%x, timerid=%p].\n", pidpair->m_msgid, pidpair->m_ptimer);

			if (NULL != pidpair->m_ptimer)
			{
				sr_printf(SR_PRINT_CRIT, "  xxxx  ==--->> processRspRegister deleteTimer=%p.\n", pidpair->m_ptimer);
				deleteTimer(pidpair->m_ptimer);
				m_re_register_timer_list.erase(timerdatalist_itor++);

				delete pidpair;
				pidpair = NULL;
			}
			else
			{
				timerdatalist_itor++;
			}
		}
	}
	
	MCCfgInfo::instance()->set_mctoken(s->token());
	MCCfgInfo::instance()->set_mcdeviceid(s->deviceid());
	MCCfgInfo::instance()->set_mcgroupid(s->groupid());
	MCCfgInfo::instance()->set_mcdomainname(s->domainname());
	for (int i=0; i < s->compinfos_size(); i++)
	{
		CompanyInfo* pcompanyinfo = new CompanyInfo();
		pcompanyinfo->m_compid = s->compinfos(i).companyid();
		pcompanyinfo->m_compname = s->compinfos(i).companyname();
		for (int j = 0; j < s->compinfos(i).totallicinfos_size(); j++)
		{
			CompLicenceInfo* complicinfos = new CompLicenceInfo();
			complicinfos->m_licencetype = s->compinfos(i).totallicinfos(j).licencetype();
			complicinfos->m_licencenum = s->compinfos(i).totallicinfos(j).licencenum();
			complicinfos->m_starttime = s->compinfos(i).totallicinfos(j).starttime();
			complicinfos->m_exptime = s->compinfos(i).totallicinfos(j).exptime();
			pcompanyinfo->m_complicenceinfos.insert(std::make_pair(complicinfos->m_licencetype, complicinfos));
		}
		PostToTerThread(e_notify_to_save_complicenceinfo, new TotalCompLicenceInfoData(pcompanyinfo));
	}
	sr_printf(SR_PRINT_INFO,"MC Register DevMgr OK\n");

	SRMsgs::ReqGetDeviceInfo reqgdi;
	reqgdi.set_token(MCCfgInfo::instance()->get_mctoken());
	reqgdi.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
	reqgdi.set_devnums(10);//�����Ի�ȡ���豸��
	reqgdi.set_get_svr_type(DEVICE_SERVER::DEVICE_NETMP);//��Ҫ��ȡ���豸����
	reqgdi.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
	SerialAndSendDevmgr_nobuf(getMsgIdByClassName(ReqGetDeviceInfo), &reqgdi);

	PostToTerThread(e_notify_sync_confinfo2devmgr, 0);

	//SRMsgs::IndsertodevHeart hearbeattodevmgr;
	//hearbeattodevmgr.set_svr_type(MCCfgInfo::instance()->get_mc_svr_type());
	//hearbeattodevmgr.set_deviceid(MCCfgInfo::instance()->get_mcdeviceid());
	//hearbeattodevmgr.set_token(MCCfgInfo::instance()->get_mctoken());
	//hearbeattodevmgr.set_load1(0u);
	////hearbeattodevmgr.set_load2(0u);
	//hearbeattodevmgr.set_load2(GlobalVars::getAllTerNums());//(��һ�λ��߶�������)ע��ɹ�����ǰ�ĸ��ظ���devmgr
	//SerialAndSendDevmgr_nobuf(getMsgIdByClassName(IndsertodevHeart), &hearbeattodevmgr);

	PostToTerThread(e_notify_to_send_heart2devmgr, 0);

	//���͸�DevMgr
	if(m_netmpinfomanager->m_buffered_dev.size() > 0)
	{
		sr_printf(SR_PRINT_NORMAL,"have buffer to devmgr,next send\n");
		std::list<BufferedProtoMsgPair*>::iterator send_itor = m_netmpinfomanager->m_buffered_dev.begin();
		for(;send_itor != m_netmpinfomanager->m_buffered_dev.end();++send_itor)
		{
			BufferedProtoMsgPair* pbpmp = *send_itor;
			SerialAndSendDevmgr_nobuf(pbpmp->m_msguid,pbpmp->m_pmsg);
			delete pbpmp;
		}
		m_netmpinfomanager->m_buffered_dev.clear();
		sr_printf(SR_PRINT_NORMAL, "send over buffer to devmgr\n");
	}
}


SR_void DevMgrNetMPProcessThread::onTcpDataRecv(const SockTcpData* pmsgdata)
{
#ifdef SRMC_DEBUG_TIME
	struct timespec process_timestamp;
	::clock_gettime(CLOCK_MONOTONIC,&process_timestamp);
#endif
	//const S_SRMsgHeader* psrmh = &(pmsgdata->m_srmh);
	//SUIRUI_UNUSE_VAR(psrmh);
#if 0
	sr_printf(SR_PRINT_INFO,"devmgr_netmp sock=%d,type=0x%x,receive a message\n",
		pmsgdata->m_sock,pmsgdata->m_srmh.m_cmdtype);
	for(SR_uint32 i = 0;i < pmsgdata->m_srmh.m_data_len;i++)
		printf("%02x",*((SR_uint8 *)(pmsgdata->m_buf+i)));
	printf("\n");
#endif
	S_ProtoMsgHeader protoheader;
	unpackProtoMsgHeader(pmsgdata->m_buf,&protoheader);
	//TODO: �ϸ��� m_cmdtype �� m_msguid ��Χ
	//sr_printf(SR_PRINT_INFO, " DevMgrNetMPProcessThread onTcpDataRecv m_pSocket = %p, m_msguid=0x%x\n", pmsgdata->m_pSocket, protoheader.m_msguid);
	switch(protoheader.m_msguid)
	{
		case SRMsgId_IndDevtoserHeart:
		{
			//: �豸�����������
			SRMsgs::IndDevtoserHeart indheart;
			SR_bool isparseok = indheart.ParsePartialFromArray(pmsgdata->m_buf+sizeof(S_ProtoMsgHeader),protoheader.m_msglen);
			if(false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR,"protobuf_msguid=0x%x,ParsePartialFromArray error\n",protoheader.m_msguid);
				return;
			}
			if(m_netmpinfomanager->m_current_devmgr_sockptr != pmsgdata->m_pSocket)
			{
				sr_printf(SR_PRINT_ERROR, "IndDevtoserHeart devmgr sockptr(%p) is not the current connected sockptr(%p)\n", pmsgdata->m_pSocket, m_netmpinfomanager->m_current_devmgr_sockptr);
				return;
			}

			sr_printf(SR_PRINT_DEBUG, "devmgr->mc, IndDevtoserHeart -- \n%s\n", indheart.Utf8DebugString().c_str());

			m_netmpinfomanager->m_current_dev_stat = DEVMGR_HEART_FLAGS;
			
			break;
		}
		case SRMsgId_RspRegister:
		{
			SRMsgs::RspRegister s;
			SR_bool isparseok = s.ParsePartialFromArray(pmsgdata->m_buf+sizeof(S_ProtoMsgHeader),protoheader.m_msglen);
			if(false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR,"protobuf_msguid=0x%x,ParseFromArray error\n",protoheader.m_msguid);
				return;
			}
			if(m_netmpinfomanager->m_current_devmgr_sockptr != pmsgdata->m_pSocket)
			{
				sr_printf(SR_PRINT_ERROR,"RspRegister devmgr isnot the current connected\n");
				return;
			}

			sr_printf(SR_PRINT_NORMAL, "devmgr->mc, RspRegister -- \n%s\n", s.Utf8DebugString().c_str());

			this->processRspRegister(&s);
			
			break;
		}
		case SRMsgId_RspUnRegister://�ݲ�����
		{
			break;
		}
		case SRMsgId_RspGetDeviceInfo://����ʽ��Ϣ�������ǻ�ȡnetmp��Ҳ�����ǻ�ȡmp�豸��Ϣ
		{
			//SRMsgs::RspGetDeviceInfo s;
			static SRMsgs::RspGetDeviceInfo* s_rspGetDI = new SRMsgs::RspGetDeviceInfo();		
			SR_bool isparseok = s_rspGetDI->ParsePartialFromArray(pmsgdata->m_buf+sizeof(S_ProtoMsgHeader),protoheader.m_msglen);
			if(false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR,"protobuf_msguid=0x%x,RspGetDeviceInfo error\n",protoheader.m_msguid);
				return;
			}	
			sr_printf(SR_PRINT_DEBUG,"devmgr->mc, RspGetDeviceInfo -- \n%s\n",s_rspGetDI->Utf8DebugString().c_str());

			std::map<SR_uint32,NetMPInfo*>::iterator netmpinfos;// = m_netmpinfomanager->m_netmpinfo
			std::map<SR_uint32, MPInfo*>::iterator mpinfos;
			std::map<SR_uint32, GWInfo*>::iterator gwinfos;
			std::map<SR_uint32, CRSInfo*>::iterator crsinfos;
			std::map<SR_uint32, SRSInfo*>::iterator srs_itor;
			std::map<SR_uint32, RelayServerInfo*>::iterator relayserver_itor;
			std::map<SR_uint32, RelayMcInfo*>::iterator relaymc_itor;
			if(0u == s_rspGetDI->deviceinfolist_size())
			{
				sr_printf(SR_PRINT_INFO,"devmgr->mc, RspGetDeviceInfo devmgr have no netmp/mp.\n");
				//TODO:ɾ������netmp
				
				return;
			}

			SR_uint32 connect_nums = 1u;
			for(SR_int32 i = 0;i < s_rspGetDI->deviceinfolist_size();i++)//����NETMP��������MP��������GW
			{
				const SRMsgs::RspGetDeviceInfo_DeviceInfo & dil = s_rspGetDI->deviceinfolist(i);
				if ((!dil.has_deviceid()) || (!dil.has_ip()) || (!dil.has_port()) 
					|| (!((dil.svr_type() == DEVICE_SERVER::DEVICE_NETMP)
					|| (dil.svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC)
					|| (dil.svr_type() == DEVICE_SERVER::DEVICE_RELAYSERVER)
					|| (dil.svr_type() == DEVICE_SERVER::DEVICE_MP) 
					|| (dil.svr_type() == DEVICE_SERVER::DEVICE_GW) 
					|| (dil.svr_type() == DEVICE_SERVER::DEVICE_CRS)
					|| (dil.svr_type() == DEVICE_SERVER::DEVICE_SRS))))
				{
					sr_printf(SR_PRINT_ERROR,"deviceinfolist=\n%s info invalid\n",dil.Utf8DebugString().c_str());
					continue;
				}
				if((0u == dil.deviceid())||(0u == dil.ip())||(0u == dil.port()))
				{
					sr_printf(SR_PRINT_ERROR, "deviceinfolist[deviceid=%d, ip=%d, port=%d] error continue!!!\n", dil.deviceid(), dil.ip(), dil.port());
					continue;
				}

				if (dil.svr_type() == DEVICE_SERVER::DEVICE_NETMP) // NETMP
				{
					//m_netmpinfomanager
					netmpinfos = m_netmpinfomanager->m_netmpinfo.find(dil.deviceid());
					if (m_netmpinfomanager->m_netmpinfo.end() == netmpinfos)//����
					{
						NetMPInfo* netmpinfo = new NetMPInfo();
						netmpinfo->m_connectionstate = e_Netmp_sock_inite;

						SR_char srcIPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
						struct in_addr srcinaddr; // IPv4��ַ�ṹ��
						srcinaddr.s_addr = dil.ip();
						inet_ntop(AF_INET, (void *)&srcinaddr, srcIPdotdec, sizeof(srcIPdotdec));
						if (MCCfgInfo::instance()->get_replacenetmp_regsrcipmapip(srcIPdotdec).size() > 0)
						{
							struct in_addr ipaddr_in;
							inet_pton(AF_INET, MCCfgInfo::instance()->get_replacenetmp_regsrcipmapip(srcIPdotdec).c_str(), &ipaddr_in);
							netmpinfo->m_ip = (SR_uint32)(ipaddr_in.s_addr);
						}
						else
						{
							netmpinfo->m_ip = dil.ip();
						}
						netmpinfo->m_port = dil.port();
						netmpinfo->m_netmpdeviceid = dil.deviceid();
						netmpinfo->m_load = dil.load(); // ����
						netmpinfo->m_load2 = dil.load2(); // �ն���
						if (dil.max_bandwidth() == 0)
						{
							netmpinfo->m_max_bandwidth = MCCfgInfo::instance()->get_netmp_highload();
						}
						else
						{
							netmpinfo->m_max_bandwidth = dil.max_bandwidth() * 1024;
						}
						if (dil.max_terms() == 0)
						{
							netmpinfo->m_max_terms = MCCfgInfo::instance()->get_maxters_in_netmp();
						}
						else
						{
							netmpinfo->m_max_terms = dil.max_terms();
						}
						netmpinfo->m_groupid = dil.groupid();
						//netmpinfo->m_ternums = 0u;
						//netmpinfo->m_ters.clear();
						for (int j = 0; j < dil.mapinternetips_size(); j++)
						{
							const SRMsgs::RspGetDeviceInfo_MapIPInfo& mapinfo = dil.mapinternetips(j);
							if (mapinfo.mapip().length() > 0)
							{
								netmpinfo->m_mapinternetips.push_back(mapinfo.mapip());
							}
						}
						netmpinfo->m_nettype = dil.nettype();
						m_netmpinfomanager->m_netmpinfo.insert(std::make_pair(dil.deviceid(), netmpinfo));
						sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo new netmp(ip=%u,port=%u,netmpid=%u,load=%u,load2=%u,max_bandwidth=%u,max_terms=%u)\n", dil.ip(), dil.port(), dil.deviceid(), dil.load(), dil.load2(), dil.max_bandwidth(),dil.max_terms());

						{
							//�Ѿ��ﵽ�������
							if (m_netmpinfomanager->m_connected_nemtps.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp())
							{
								//sr_printf(SR_PRINT_INFO,"");
								sr_printf(SR_PRINT_INFO,"onTcpDataRecv SRMsgId_RspGetDeviceInfo have max netmp_tcp_connect[cur:%d, cfg:%d]\n",
									m_netmpinfomanager->m_connected_nemtps.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
								return;
							}

							if (m_netmpinfomanager->m_idle_netmps.size() >= MCCfgInfo::instance()->get_idle_netmp_nums())
							{
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo m_idle_netmps=%" SR_PRI_size_t ",cfg_idle_nums=%u\n", 
									m_netmpinfomanager->m_idle_netmps.size(), MCCfgInfo::instance()->get_idle_netmp_nums());
								return;
							}
							//����Ƿ���Ҫ�½�����

							for (std::map<SR_uint32, NetMPInfo*>::iterator netmpitor = m_netmpinfomanager->m_netmpinfo.begin();
								netmpitor != m_netmpinfomanager->m_netmpinfo.end(); netmpitor++)
							{
								NetMPInfo* pnetinfo = netmpitor->second;
								if (pnetinfo->m_connectionstate != e_Netmp_sock_inite)
								{
									sr_printf(SR_PRINT_INFO,"onTcpDataRecv SRMsgId_RspGetDeviceInfo pnetinfo->m_netmpdeviceid=%u m_connectionstate=%u(not inite), continue search netmp.\n", pnetinfo->m_netmpdeviceid,pnetinfo->m_connectionstate);
									continue;
								}

								if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums())
								{
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo connect_nums=%u,cfg_idle_nums=%u\n", connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
									break;
								}

								sr_printf(SR_PRINT_DEBUG, "onTcpDataRecv SRMsgId_RspGetDeviceInfo next connect_nums=%d,idle_netmps=%lu,cfg_idle_netmps=%u,connected_netmps=%lu\n", connect_nums,
									m_netmpinfomanager->m_idle_netmps.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
									m_netmpinfomanager->m_connected_nemtps.size());
								connect_nums++;
								SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
								struct in_addr s; // IPv4��ַ�ṹ��
								s.s_addr = pnetinfo->m_ip;
								inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo check connect netmp(%u)(%s,%u,%u)\n", pnetinfo->m_ip, IPdotdec, pnetinfo->m_port, pnetinfo->m_netmpdeviceid);
								SockRelateData srd(knetmp_sock_type, pnetinfo->m_netmpdeviceid);
								pnetinfo->m_connectionstate = e_Netmp_sock_connecting;
								if (m_deviceMan)
									m_deviceMan->ConnectNetmp(IPdotdec, pnetinfo->m_port, srd);

								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo check connect netmp(%u)(%s,%u,%u) is connecting.....\n", pnetinfo->m_ip, IPdotdec, pnetinfo->m_port, pnetinfo->m_netmpdeviceid);

							}
						}//����netmp
					}
					else//����
					{
						NetMPInfo* netmpinfo = netmpinfos->second;
						if (netmpinfo->m_connectionstate != e_Netmp_sock_connect_ok)
							netmpinfo->m_connectionstate = e_Netmp_sock_inite;
						if (netmpinfo->m_ip != dil.ip())
						{
							//TODO:ɾ��֮ǰ��netmpid
						}
						if (netmpinfo->m_port != dil.port())
						{
							//TODO:
						}

						if ((netmpinfo->m_load != dil.load()) 
							||(netmpinfo->m_load2 != dil.load2())
							|| netmpinfo->m_mapinternetips.size() != dil.mapinternetips_size()
							|| netmpinfo->m_nettype != dil.nettype()
							|| netmpinfo->m_max_bandwidth != dil.max_bandwidth()
							|| netmpinfo->m_max_terms != dil.max_terms())//Loadֵ�иı�
						{
							netmpinfo->m_load = dil.load();// ����
							netmpinfo->m_load2 = dil.load2();// �ն���
							netmpinfo->m_nettype = dil.nettype();
							if (dil.max_bandwidth() == 0)
							{
								netmpinfo->m_max_bandwidth = MCCfgInfo::instance()->get_netmp_highload();
							}
							else
							{
								netmpinfo->m_max_bandwidth = dil.max_bandwidth() * 1024;
							}
							if (dil.max_terms() == 0)
							{
								netmpinfo->m_max_terms = MCCfgInfo::instance()->get_maxters_in_netmp();
							}
							else
							{
								netmpinfo->m_max_terms = dil.max_terms();
							}
							if (netmpinfo->m_connectionstate == e_Netmp_sock_connect_ok)
							{
								std::set<SR_uint32>::iterator netmp_set_itor;
								netmp_set_itor = m_netmpinfomanager->m_idle_netmps.find(netmpinfo->m_netmpdeviceid);
								if (netmp_set_itor != m_netmpinfomanager->m_idle_netmps.end())
									m_netmpinfomanager->m_idle_netmps.erase(netmp_set_itor);

								netmp_set_itor = m_netmpinfomanager->m_highload_netmps.find(netmpinfo->m_netmpdeviceid);
								if (netmp_set_itor != m_netmpinfomanager->m_highload_netmps.end())
									m_netmpinfomanager->m_highload_netmps.erase(netmp_set_itor);

								netmp_set_itor = m_netmpinfomanager->m_notuse_netmps.find(netmpinfo->m_netmpdeviceid);
								if (netmp_set_itor != m_netmpinfomanager->m_notuse_netmps.end())
									m_netmpinfomanager->m_notuse_netmps.erase(netmp_set_itor);

								if (netmpinfo->m_load <= MCCfgInfo::instance()->get_netmp_lowload())
								{
									m_netmpinfomanager->m_idle_netmps.insert(netmpinfo->m_netmpdeviceid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update netmpid=%u in 0-lowload,m_idle_netmps.size=%d.\n", netmpinfo->m_netmpdeviceid, m_netmpinfomanager->m_idle_netmps.size());
								}
								else if ((netmpinfo->m_load > MCCfgInfo::instance()->get_netmp_lowload()) && (netmpinfo->m_load <= MCCfgInfo::instance()->get_netmp_highload()))
								{
									m_netmpinfomanager->m_highload_netmps.insert(netmpinfo->m_netmpdeviceid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update netmpid=%u in lowload-highload,m_highload_netmps.size=%d.\n", netmpinfo->m_netmpdeviceid, m_netmpinfomanager->m_highload_netmps.size());
								}

								if (netmpinfo->m_load > MCCfgInfo::instance()->get_netmp_highload())
								{
									m_netmpinfomanager->m_notuse_netmps.insert(netmpinfo->m_netmpdeviceid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update netmpid=%u in m_notuse_netmps.size=%d.\n", netmpinfo->m_netmpdeviceid, m_netmpinfomanager->m_notuse_netmps.size());
								}

								NetMPInfoData* pninfodata = new NetMPInfoData();
								pninfodata->m_netmpid = netmpinfo->m_netmpdeviceid;
								pninfodata->m_load = netmpinfo->m_load;
								pninfodata->m_load2 = netmpinfo->m_load2;
								pninfodata->m_max_bandwidth = netmpinfo->m_max_bandwidth;
								pninfodata->m_max_terms = netmpinfo->m_max_terms;
								SR_char srcIPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
								struct in_addr srcinaddr; // IPv4��ַ�ṹ��
								srcinaddr.s_addr = netmpinfo->m_ip;
								inet_ntop(AF_INET, (void *)&srcinaddr, srcIPdotdec, sizeof(srcIPdotdec));
								SR_char srIPPortdotdec[128] = {};
								pninfodata->m_ip = srcIPdotdec;
								sprintf(srIPPortdotdec, "%s:%d", srcIPdotdec, netmpinfo->m_port);
								pninfodata->m_ipandport = srIPPortdotdec;
								pninfodata->m_mapinternetips.clear();
								for (int j = 0; j < dil.mapinternetips_size(); j++)
								{
									const SRMsgs::RspGetDeviceInfo_MapIPInfo& mapinfo = dil.mapinternetips(j);
									if (mapinfo.mapip().length() > 0)
									{
										pninfodata->m_mapinternetips.push_back(mapinfo.mapip());
									}
								}
								pninfodata->m_nettype = netmpinfo->m_nettype;

								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo update netmp=%u,m_mapinternetips:\n", dil.deviceid());
								for (std::list<std::string>::iterator mapip_itor = pninfodata->m_mapinternetips.begin();
									mapip_itor != pninfodata->m_mapinternetips.end(); mapip_itor++)
								{
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo update netmp=%u,m_mapinternetips=%s.\n", dil.deviceid(), (*mapip_itor).c_str());
								}
								PostToTerThread(e_update_netmpinfo_inconf, pninfodata);

								if (m_netmpinfomanager->m_idle_netmps.size() < MCCfgInfo::instance()->get_idle_netmp_nums())
								{
									for (std::map<SR_uint32, NetMPInfo*>::iterator netmpitor = m_netmpinfomanager->m_netmpinfo.begin();
										netmpitor != m_netmpinfomanager->m_netmpinfo.end(); netmpitor++)
									{
										NetMPInfo* pnetinfo = netmpitor->second;
										if (pnetinfo->m_connectionstate != e_Netmp_sock_inite)
										{
											continue;
										}

										SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
										struct in_addr s; // IPv4��ַ�ṹ��
										s.s_addr = pnetinfo->m_ip;
										inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
										sr_printf(SR_PRINT_INFO, "increase idle_netmp next connnect nemtp(%u)(%s,%u,%u)\n", pnetinfo->m_ip, IPdotdec, pnetinfo->m_port, pnetinfo->m_netmpdeviceid);
										SockRelateData srd(knetmp_sock_type, pnetinfo->m_netmpdeviceid);
										pnetinfo->m_connectionstate = e_Netmp_sock_connecting;
										//connect(IPdotdec,pnetinfo->m_port,srd);
										break;
									}
								}
							}
						}
					}
				}//else if (dil.svr_type() == 2) // NETMP
				else if (dil.svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC) // RELAYMC
				{
					time_t timeNow;
					timeNow = time(NULL);

					if (!m_periodtimer_checkrelaymctimeout)
						m_periodtimer_checkrelaymctimeout = createTimer(MCCfgInfo::instance()->get_beatto_netmp_time(), e_periodicity_timer, e_checkrelaymc_timeout, 0ull); // ����netmp��������

					relaymc_itor = m_relaymcinfomgr->m_relaymcinfo.find(dil.deviceid());
					if (m_relaymcinfomgr->m_relaymcinfo.end() == relaymc_itor)//����
					{
						RelayMcInfo* relaymcinfo = new RelayMcInfo();
						//relaymcinfo->m_connectionstate = e_dev_sock_inite;
						relaymcinfo->m_ip = dil.ip();
						relaymcinfo->m_port = dil.port();
						relaymcinfo->m_relaymcdevid = dil.deviceid();
						relaymcinfo->m_ternum = dil.load();// �����ն���
						relaymcinfo->m_groupid = dil.groupid();
						relaymcinfo->m_relaymcdevtime = timeNow;
						relaymcinfo->m_mapinternetips.clear();
						for (int j = 0; j < dil.mapinternetips_size(); j++)
						{
							const SRMsgs::RspGetDeviceInfo_MapIPInfo& mapips = dil.mapinternetips(j);
							if (mapips.mapip().length() > 0)
							{
								relaymcinfo->m_mapinternetips.push_back(mapips.mapip());
							}
						}
						m_relaymcinfomgr->m_relaymcinfo.insert(std::make_pair(dil.deviceid(), relaymcinfo));

						sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo new relaymc(ip=%u,port=%u,relaymcid=%u,ternum=%u)\n", dil.ip(), dil.port(), dil.deviceid(), dil.load());

						RelayMcInfoData* pRminfodata = new RelayMcInfoData();
						pRminfodata->m_relaymcid = relaymcinfo->m_relaymcdevid;
						pRminfodata->m_ip = relaymcinfo->m_ip;
						pRminfodata->m_port = relaymcinfo->m_port;
						pRminfodata->m_ternum = relaymcinfo->m_ternum;
						pRminfodata->m_groupid = relaymcinfo->m_groupid;
						pRminfodata->m_addordel = 1;
						for (int j = 0; j < dil.mapinternetips_size(); j++)
						{
							const SRMsgs::RspGetDeviceInfo_MapIPInfo& mapips = dil.mapinternetips(j);
							if (mapips.mapip().length() > 0)
							{
								pRminfodata->m_mapinternetips.push_back(mapips.mapip());
							}
						}
						PostToTerThread(e_update_relaymcinfo_inconf, pRminfodata);
					}
					else//����relaymc
					{
						RelayMcInfo* relaymcinfo = relaymc_itor->second;
						
						relaymcinfo->m_ip = dil.ip();
						relaymcinfo->m_port = dil.port();
						relaymcinfo->m_relaymcdevid = dil.deviceid();
						relaymcinfo->m_ternum = dil.load();// �����ն���
						relaymcinfo->m_groupid = dil.groupid();
						relaymcinfo->m_relaymcdevtime = timeNow;
						relaymcinfo->m_mapinternetips.clear();
						for (int j = 0; j < dil.mapinternetips_size(); j++)
						{
							const SRMsgs::RspGetDeviceInfo_MapIPInfo& mapips = dil.mapinternetips(j);
							if (mapips.mapip().length() > 0)
							{
								relaymcinfo->m_mapinternetips.push_back(mapips.mapip());
							}
						}

						sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo update relaymc=%u,m_mapinternetips:\n", dil.deviceid());
						for (std::list<std::string>::iterator mapip_itor = relaymcinfo->m_mapinternetips.begin();
							mapip_itor != relaymcinfo->m_mapinternetips.end(); mapip_itor++)
						{
							sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo update relaymc=%u,m_mapinternetips=%s.\n", dil.deviceid(), (*mapip_itor).c_str());
						}

						RelayMcInfoData* pRminfodata = new RelayMcInfoData();
						pRminfodata->m_relaymcid = relaymcinfo->m_relaymcdevid;
						pRminfodata->m_ip = relaymcinfo->m_ip;
						pRminfodata->m_port = relaymcinfo->m_port;
						pRminfodata->m_ternum = relaymcinfo->m_ternum;
						pRminfodata->m_groupid = relaymcinfo->m_groupid;
						pRminfodata->m_addordel = 3;// ����
						for (int j = 0; j < dil.mapinternetips_size(); j++)
						{
							const SRMsgs::RspGetDeviceInfo_MapIPInfo& mapips = dil.mapinternetips(j);
							if (mapips.mapip().length() > 0)
							{
								pRminfodata->m_mapinternetips.push_back(mapips.mapip());
							}
						}
						PostToTerThread(e_update_relaymcinfo_inconf, pRminfodata);
					}
				}//else if (dil.svr_type() == DEVICE_SERVER::DEVICE_RELAY_MC) // RELAYMC
				else if (dil.svr_type() == DEVICE_SERVER::DEVICE_RELAYSERVER) // RELAYSERVER
				{
					relayserver_itor = m_relaysvrinfomgr->m_relaysvrinfo.find(dil.deviceid());
					if (m_relaysvrinfomgr->m_relaysvrinfo.end() == relayserver_itor)//����
					{
						RelayServerInfo* relayserverinfo = new RelayServerInfo();
						relayserverinfo->m_connectionstate = e_dev_sock_inite;
						relayserverinfo->m_ip = dil.ip();
						relayserverinfo->m_port = dil.port();
						relayserverinfo->m_relaysvrdevid = dil.deviceid();
						relayserverinfo->m_ternum = dil.load();// �����ն���
						relayserverinfo->m_tx = dil.load2();// ���д�����relayserver->ter��
						m_relaysvrinfomgr->m_relaysvrinfo.insert(std::make_pair(dil.deviceid(), relayserverinfo));
						sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo new relayserver(ip=%u,port=%u,relayserverid=%u,tx=%u,ternum=%u)\n", dil.ip(), dil.port(), dil.deviceid(), dil.load(), dil.load2());

						{
							//�Ѿ��ﵽ�������
							if (m_relaysvrinfomgr->m_connected_relaysvrs.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp())// ����netmp��������
							{
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo have max mp_tcp_connect[cur:%d, cfg:%d]\n",
									m_relaysvrinfomgr->m_connected_relaysvrs.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
								return;
							}

							if (m_relaysvrinfomgr->m_idle_relaysvrs.size() >= MCCfgInfo::instance()->get_idle_netmp_nums())// ����netmp��������
							{
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo m_idle_relaysvrs=%" SR_PRI_size_t ",cfg_idle_nums=%u\n",
									m_relaysvrinfomgr->m_idle_relaysvrs.size(), MCCfgInfo::instance()->get_idle_netmp_nums());
								return;
							}
							//����Ƿ���Ҫ�½�����

							for (std::map<SR_uint32, RelayServerInfo*>::iterator rsitor = m_relaysvrinfomgr->m_relaysvrinfo.begin();
								rsitor != m_relaysvrinfomgr->m_relaysvrinfo.end(); rsitor++)
							{
								RelayServerInfo* pRSinfo = rsitor->second;
								if (pRSinfo->m_connectionstate != e_dev_sock_inite)
								{
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo pRSinfo->m_connectionstate=%u\n", pRSinfo->m_connectionstate);
									continue;
								}

								if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums())// ����netmp��������
								{
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo connect_nums=%u,cfg_idle_nums=%u\n", connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
									break;
								}

								sr_printf(SR_PRINT_DEBUG, "onTcpDataRecv SRMsgId_RspGetDeviceInfo next connect_nums=%d,idle_relayservers=%lu,cfg_idle_relayservers=%u,connected_relayservers=%lu\n", connect_nums,
									m_relaysvrinfomgr->m_idle_relaysvrs.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
									m_relaysvrinfomgr->m_connected_relaysvrs.size());
								connect_nums++;
								SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
								struct in_addr s; // IPv4��ַ�ṹ��
								s.s_addr = pRSinfo->m_ip;
								inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo next connect relayserver(%u)(%s,%u,%u)\n", pRSinfo->m_ip, IPdotdec, pRSinfo->m_port, pRSinfo->m_relaysvrdevid);
								SockRelateData srd(krelayserver_sock_type, pRSinfo->m_relaysvrdevid);
								pRSinfo->m_connectionstate = e_dev_sock_connecting;
								if (m_deviceMan)
									m_deviceMan->ConnectRelayserver(IPdotdec, pRSinfo->m_port, srd);

								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo new relayserver(%u)(%s,%u,%u) is connecting.....\n", pRSinfo->m_ip, IPdotdec, pRSinfo->m_port, pRSinfo->m_relaysvrdevid);

							}
						}//����relayserver
					}
					else//����relayserver
					{
						RelayServerInfo* relayserverinfo = relayserver_itor->second;
						if (relayserverinfo->m_connectionstate != e_dev_sock_connect_ok)
							relayserverinfo->m_connectionstate = e_dev_sock_inite;
						if (relayserverinfo->m_ip != dil.ip())
						{
							//TODO:ɾ��֮ǰ��mpid
						}
						if (relayserverinfo->m_port != dil.port())
						{
							//TODO:
						}

						//SR_uint32 	m_load;// relayserver:�����ն���
						//SR_uint32 	m_load2;// relayserver:����(����)����
						if ((relayserverinfo->m_ternum != dil.load()) ||
							(relayserverinfo->m_tx != dil.load2()))//Loadֵ�иı�
						{
							relayserverinfo->m_ternum = dil.load();// relayserver:�����ն���
							relayserverinfo->m_tx = dil.load2();// relayserver:����(����)����

							if (relayserverinfo->m_connectionstate == e_dev_sock_connect_ok)
							{
								std::set<SR_uint32>::iterator rs_set_itor;
								rs_set_itor = m_relaysvrinfomgr->m_idle_relaysvrs.find(relayserverinfo->m_relaysvrdevid);
								if (rs_set_itor != m_relaysvrinfomgr->m_idle_relaysvrs.end())
									m_relaysvrinfomgr->m_idle_relaysvrs.erase(rs_set_itor);

								rs_set_itor = m_relaysvrinfomgr->m_highload_relaysvrs.find(relayserverinfo->m_relaysvrdevid);
								if (rs_set_itor != m_relaysvrinfomgr->m_highload_relaysvrs.end())
									m_relaysvrinfomgr->m_highload_relaysvrs.erase(rs_set_itor);

								rs_set_itor = m_relaysvrinfomgr->m_notuse_relaysvrs.find(relayserverinfo->m_relaysvrdevid);
								if (rs_set_itor != m_relaysvrinfomgr->m_notuse_relaysvrs.end())
									m_relaysvrinfomgr->m_notuse_relaysvrs.erase(rs_set_itor);

								if (relayserverinfo->m_tx <= MCCfgInfo::instance()->get_netmp_lowload()) // ����netmp��������
								{
									m_relaysvrinfomgr->m_idle_relaysvrs.insert(relayserverinfo->m_relaysvrdevid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update relayserverid=%u in 0-lowload,m_idle_relaysvrs.size=%d.\n", relayserverinfo->m_relaysvrdevid, m_relaysvrinfomgr->m_idle_relaysvrs.size());
								}
								else if ((relayserverinfo->m_tx > MCCfgInfo::instance()->get_netmp_lowload()) && (relayserverinfo->m_tx <= MCCfgInfo::instance()->get_netmp_highload()))// ����netmp��������
								{
									m_relaysvrinfomgr->m_highload_relaysvrs.insert(relayserverinfo->m_relaysvrdevid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update relayserverid=%u in lowload-highload,m_highload_relaysvrs.size=%d.\n", relayserverinfo->m_relaysvrdevid, m_relaysvrinfomgr->m_highload_relaysvrs.size());
								}

								if (relayserverinfo->m_tx > MCCfgInfo::instance()->get_netmp_highload()) // ����netmp��������
								{
									m_relaysvrinfomgr->m_notuse_relaysvrs.insert(relayserverinfo->m_relaysvrdevid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update relayserverid=%u in m_notuse_relaysvrs.size=%d.\n", relayserverinfo->m_relaysvrdevid, m_relaysvrinfomgr->m_notuse_relaysvrs.size());
								}

								RelayServerInfoData* pRSinfodata = new RelayServerInfoData();
								pRSinfodata->m_relayserverid = relayserverinfo->m_relaysvrdevid;
								pRSinfodata->m_tx = relayserverinfo->m_tx;
								pRSinfodata->m_ternum = relayserverinfo->m_ternum;
								PostToTerThread(e_update_relayserverinfo_inconf, pRSinfodata);

								if (m_relaysvrinfomgr->m_idle_relaysvrs.size() < MCCfgInfo::instance()->get_idle_netmp_nums()) // ����netmp��������
								{
									for (std::map<SR_uint32, RelayServerInfo*>::iterator rsinfo_itor = m_relaysvrinfomgr->m_relaysvrinfo.begin();
										rsinfo_itor != m_relaysvrinfomgr->m_relaysvrinfo.end(); rsinfo_itor++)
									{
										RelayServerInfo* pRSinfo = rsinfo_itor->second;
										if (pRSinfo->m_connectionstate != e_dev_sock_inite)
										{
											continue;
										}

										SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
										struct in_addr s; // IPv4��ַ�ṹ��
										s.s_addr = pRSinfo->m_ip;
										inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
										sr_printf(SR_PRINT_INFO, "increase idle_relayserver next connnect relayserver(%u)(%s,%u,%u)\n", pRSinfo->m_ip, IPdotdec, pRSinfo->m_port, pRSinfo->m_relaysvrdevid);
										SockRelateData srd(krelayserver_sock_type, pRSinfo->m_relaysvrdevid);
										pRSinfo->m_connectionstate = e_dev_sock_connecting;
										break;
									}
								}
							}
						}
					}
				}//else if (dil.svr_type() == 4) // RELAYSERVER
				else if (dil.svr_type() == DEVICE_SERVER::DEVICE_MP) // MP
				{
					//m_mpinfomanager
					mpinfos = m_mpinfomanager->m_mpinfo.find(dil.deviceid());
					if (m_mpinfomanager->m_mpinfo.end() == mpinfos)//����
					{
						MPInfo* mpinfo = new MPInfo();
						mpinfo->m_connectionstate = e_Mp_sock_inite;
						mpinfo->m_ip = dil.ip();
						mpinfo->m_port = dil.port();
						mpinfo->m_mpdeviceid = dil.deviceid();
						mpinfo->m_load = dil.load();// ���д�����netmp->mp��
						mpinfo->m_load2 = dil.load2();// cpuʹ����
						m_mpinfomanager->m_mpinfo.insert(std::make_pair(dil.deviceid(), mpinfo));
						sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo new mp(ip=%u,port=%u,mpid=%u,load=%u,load2=%u)\n", dil.ip(), dil.port(), dil.deviceid(), dil.load(), dil.load2());

						{
							//�Ѿ��ﵽ�������
							if (m_mpinfomanager->m_connected_mps.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp())// ������chen songhua������MP��ʱ������netmp��������
							{
								sr_printf(SR_PRINT_INFO,"onTcpDataRecv SRMsgId_RspGetDeviceInfo have max mp_tcp_connect[cur:%d, cfg:%d]\n",
									m_mpinfomanager->m_connected_mps.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
								return;
							}

							if (m_mpinfomanager->m_idle_mps.size() >= MCCfgInfo::instance()->get_idle_netmp_nums())// ������chen songhua������MP��ʱ������netmp��������
							{
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo m_idle_mps=%" SR_PRI_size_t ",cfg_idle_nums=%u\n",
									m_mpinfomanager->m_idle_mps.size(), MCCfgInfo::instance()->get_idle_netmp_nums());
								return;
							}
							//����Ƿ���Ҫ�½�����

							for (std::map<SR_uint32, MPInfo*>::iterator mpitor = m_mpinfomanager->m_mpinfo.begin();
								mpitor != m_mpinfomanager->m_mpinfo.end(); mpitor++)
							{
								MPInfo* pMpinfo = mpitor->second;
								if (pMpinfo->m_connectionstate != e_Mp_sock_inite)
								{
									sr_printf(SR_PRINT_INFO,"onTcpDataRecv SRMsgId_RspGetDeviceInfo pMpinfo->m_connectionstate=%u\n",pMpinfo->m_connectionstate);
									continue;
								}

								if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums())// ������chen songhua������MP��ʱ������netmp��������
								{
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo connect_nums=%u,cfg_idle_nums=%u\n", connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
									break;
								}

								sr_printf(SR_PRINT_DEBUG, "onTcpDataRecv SRMsgId_RspGetDeviceInfo next connect_nums=%d,idle_mps=%lu,cfg_idle_mps=%u,connected_mps=%lu\n", connect_nums,
									m_mpinfomanager->m_idle_mps.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
									m_mpinfomanager->m_connected_mps.size());
								connect_nums++;
								SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
								struct in_addr s; // IPv4��ַ�ṹ��
								s.s_addr = pMpinfo->m_ip;
								inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo next connect mp(%u)(%s,%u,%u)\n", pMpinfo->m_ip, IPdotdec, pMpinfo->m_port, pMpinfo->m_mpdeviceid);
								SockRelateData srd(kmp_sock_type, pMpinfo->m_mpdeviceid);
								pMpinfo->m_connectionstate = e_Mp_sock_connecting;
								if (m_deviceMan)
									m_deviceMan->ConnectMP(IPdotdec, pMpinfo->m_port, srd);

								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo new mp(%u)(%s,%u,%u) is connecting.....\n", pMpinfo->m_ip, IPdotdec, pMpinfo->m_port, pMpinfo->m_mpdeviceid);

							}
						}//����mp
					}
					else//����mp
					{
						MPInfo* mpinfo = mpinfos->second;
						if (mpinfo->m_connectionstate != e_Mp_sock_connect_ok)
							mpinfo->m_connectionstate = e_Mp_sock_inite;
						if (mpinfo->m_ip != dil.ip())
						{
							//TODO:ɾ��֮ǰ��mpid
						}
						if (mpinfo->m_port != dil.port())
						{
							//TODO:
						}

						//SR_uint32 	m_load;// mp:���д�����netmp->mp��
						//SR_uint32 	m_load2;// mp:cpu����
						if ((mpinfo->m_load != dil.load()) ||
							(mpinfo->m_load2 != dil.load2()))//Loadֵ�иı�
						{
							mpinfo->m_load = dil.load(); // ���д�����netmp->mp��
							mpinfo->m_load2 = dil.load2(); // cpuʹ����

							if (mpinfo->m_connectionstate == e_Mp_sock_connect_ok)
							{
								std::set<SR_uint32>::iterator mp_set_itor;
								mp_set_itor = m_mpinfomanager->m_idle_mps.find(mpinfo->m_mpdeviceid);
								if (mp_set_itor != m_mpinfomanager->m_idle_mps.end())
									m_mpinfomanager->m_idle_mps.erase(mp_set_itor);

								mp_set_itor = m_mpinfomanager->m_highload_mps.find(mpinfo->m_mpdeviceid);
								if (mp_set_itor != m_mpinfomanager->m_highload_mps.end())
									m_mpinfomanager->m_highload_mps.erase(mp_set_itor);

								mp_set_itor = m_mpinfomanager->m_notuse_mps.find(mpinfo->m_mpdeviceid);
								if (mp_set_itor != m_mpinfomanager->m_notuse_mps.end())
									m_mpinfomanager->m_notuse_mps.erase(mp_set_itor);

								if (mpinfo->m_load <= MCCfgInfo::instance()->get_netmp_lowload()) // ������chen songhua������MP��ʱ������netmp��������
								{
									m_mpinfomanager->m_idle_mps.insert(mpinfo->m_mpdeviceid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update mpid=%u in 0-lowload,m_idle_mps.size=%d.\n", mpinfo->m_mpdeviceid, m_mpinfomanager->m_idle_mps.size());
								}
								else if ((mpinfo->m_load > MCCfgInfo::instance()->get_netmp_lowload()) && (mpinfo->m_load <= MCCfgInfo::instance()->get_netmp_highload()))// ������chen songhua������MP��ʱ������netmp��������
								{
									m_mpinfomanager->m_highload_mps.insert(mpinfo->m_mpdeviceid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update mpid=%u in lowload-highload,m_highload_mps.size=%d.\n", mpinfo->m_mpdeviceid, m_mpinfomanager->m_highload_mps.size());
								}

								if (mpinfo->m_load > MCCfgInfo::instance()->get_netmp_highload()) // ������chen songhua������MP��ʱ������netmp��������
								{
									m_mpinfomanager->m_notuse_mps.insert(mpinfo->m_mpdeviceid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update mpid=%u in m_notuse_mps.size=%d.\n", mpinfo->m_mpdeviceid, m_mpinfomanager->m_notuse_mps.size());
								}

								MPInfoData* pMpinfodata = new MPInfoData();
								pMpinfodata->m_mpid = mpinfo->m_mpdeviceid;
								pMpinfodata->m_load = mpinfo->m_load;
								pMpinfodata->m_load2 = mpinfo->m_load2;
								PostToTerThread(e_update_mpinfo_inconf, pMpinfodata);

								if (m_mpinfomanager->m_idle_mps.size() < MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
								{
									for (std::map<SR_uint32, MPInfo*>::iterator mpitor = m_mpinfomanager->m_mpinfo.begin();
										mpitor != m_mpinfomanager->m_mpinfo.end(); mpitor++)
									{
										MPInfo* pMpinfo = mpitor->second;
										if (pMpinfo->m_connectionstate != e_Mp_sock_inite)
										{
											continue;
										}

										SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
										struct in_addr s; // IPv4��ַ�ṹ��
										s.s_addr = pMpinfo->m_ip;
										inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
										sr_printf(SR_PRINT_INFO, "increase idle_mp next connnect mp(%u)(%s,%u,%u)\n", pMpinfo->m_ip, IPdotdec, pMpinfo->m_port, pMpinfo->m_mpdeviceid);
										SockRelateData srd(kmp_sock_type, pMpinfo->m_mpdeviceid);
										pMpinfo->m_connectionstate = e_Mp_sock_connecting;
										break;
									}
								}
							}
						}
					}
				}//else if (dil.svr_type() == 5) // MP
				else if (dil.svr_type() == DEVICE_SERVER::DEVICE_GW)
				{
					gwinfos = m_gwinfomanager->m_gwinfo.find(dil.deviceid());
					if (m_gwinfomanager->m_gwinfo.end() == gwinfos)//����
					{
						GWInfo* gwinfo = new GWInfo();
						gwinfo->m_connectionstate = e_GW_sock_inite;
						gwinfo->m_ip = dil.ip();
						gwinfo->m_port = dil.port();
						gwinfo->m_gwdeviceid = dil.deviceid();
						gwinfo->m_load = dil.load();// �ն���
						gwinfo->m_load2 = dil.load2();// cpuռ����
						m_gwinfomanager->m_gwinfo.insert(std::make_pair(dil.deviceid(), gwinfo));
						sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo new gw[ip=%u,port=%u,gwid=%u,load(ternum)=%u,load2(cpu)=%u]\n", dil.ip(), dil.port(), dil.deviceid(), dil.load(), dil.load2());

						{
							//�Ѿ��ﵽ�������
							if (m_gwinfomanager->m_connected_gws.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp())// ������chen songhua������MP��ʱ������netmp��������
							{
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo have max gw_tcp_connect[cur:%d,cfg:%d]\n", 
									m_gwinfomanager->m_connected_gws.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
								return;
							}

							if (m_gwinfomanager->m_idle_gws.size() >= MCCfgInfo::instance()->get_idle_netmp_nums())// ������chen songhua������MP��ʱ������netmp��������
							{
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo m_idle_gws=%" SR_PRI_size_t ",cfg_idle_nums=%u\n",
									m_gwinfomanager->m_idle_gws.size(), MCCfgInfo::instance()->get_idle_netmp_nums());
								return;
							}
							//����Ƿ���Ҫ�½�����

							for (std::map<SR_uint32, GWInfo*>::iterator gwitor = m_gwinfomanager->m_gwinfo.begin();
								gwitor != m_gwinfomanager->m_gwinfo.end(); gwitor++)
							{
								GWInfo* pGWinfo = gwitor->second;
								if (pGWinfo->m_connectionstate != e_GW_sock_inite)
								{
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo pGWinfo->m_gwdeviceid=%u,m_connectionstate=%u, not inite state continue search.\n", pGWinfo->m_gwdeviceid, pGWinfo->m_connectionstate);
									continue;
								}

								if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums())// ������chen songhua������MP��ʱ������netmp��������
								{
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo m_connect_gws=%u,cfg_idle_nums=%u\n", 
										connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
									break;
								}

								sr_printf(SR_PRINT_DEBUG, "onTcpDataRecv SRMsgId_RspGetDeviceInfo check connect_nums=%d,m_idle_gws=%lu,cfg_idle_gws=%u,connected_gws=%lu\n", connect_nums,
									m_gwinfomanager->m_idle_gws.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),
									m_gwinfomanager->m_connected_gws.size());
								connect_nums++;
								SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
								struct in_addr s; // IPv4��ַ�ṹ��
								s.s_addr = pGWinfo->m_ip;
								inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo check connect gw(%u)(%s,%u,%u)\n", pGWinfo->m_ip, IPdotdec, pGWinfo->m_port, pGWinfo->m_gwdeviceid);
								SockRelateData srd(kgw_sock_type, pGWinfo->m_gwdeviceid);
								pGWinfo->m_connectionstate = e_GW_sock_connecting;
								if (m_deviceMan)
									m_deviceMan->ConnectGW(IPdotdec, pGWinfo->m_port, srd);

								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo check connect gw(%u)(%s,%u,%u) is connecting.....\n", pGWinfo->m_ip, IPdotdec, pGWinfo->m_port, pGWinfo->m_gwdeviceid);
							}
						}//����gw
					}
					else//����gw
					{
						GWInfo* gwinfo = gwinfos->second;
						if (gwinfo->m_connectionstate != e_GW_sock_connect_ok)
							gwinfo->m_connectionstate = e_GW_sock_inite;
						if (gwinfo->m_ip != dil.ip())
						{
							//TODO:ɾ��֮ǰ��mpid
						}
						if (gwinfo->m_port != dil.port())
						{
							//TODO:
						}

						//SR_uint32 	m_load;// gw:�ն���
						//SR_uint32 	m_load2;// gw:cpu����
						if ((gwinfo->m_load != dil.load()) ||
							(gwinfo->m_load2 != dil.load2()))//Loadֵ�иı�
						{
							gwinfo->m_load = dil.load();// gw:�ն���
							gwinfo->m_load2 = dil.load2();// gw:cpu����

							if (gwinfo->m_connectionstate == e_GW_sock_connect_ok)
							{
								std::set<SR_uint32>::iterator gw_set_itor;
								gw_set_itor = m_gwinfomanager->m_idle_gws.find(gwinfo->m_gwdeviceid);
								if (gw_set_itor != m_gwinfomanager->m_idle_gws.end())
									m_gwinfomanager->m_idle_gws.erase(gw_set_itor);

								gw_set_itor = m_gwinfomanager->m_highload_gws.find(gwinfo->m_gwdeviceid);
								if (gw_set_itor != m_gwinfomanager->m_highload_gws.end())
									m_gwinfomanager->m_highload_gws.erase(gw_set_itor);

								gw_set_itor = m_gwinfomanager->m_notuse_gws.find(gwinfo->m_gwdeviceid);
								if (gw_set_itor != m_gwinfomanager->m_notuse_gws.end())
									m_gwinfomanager->m_notuse_gws.erase(gw_set_itor);

								if (gwinfo->m_load <= MCCfgInfo::instance()->get_netmp_lowload()) // ������chen songhua������MP��ʱ������netmp��������
								{
									m_gwinfomanager->m_idle_gws.insert(gwinfo->m_gwdeviceid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update gwid=%u in 0-lowload,m_idle_gws.size=%d.\n", gwinfo->m_gwdeviceid, m_gwinfomanager->m_idle_gws.size());
								}
								else if ((gwinfo->m_load > MCCfgInfo::instance()->get_netmp_lowload()) && (gwinfo->m_load <= MCCfgInfo::instance()->get_netmp_highload()))// ������chen songhua������MP��ʱ������netmp��������
								{
									m_gwinfomanager->m_highload_gws.insert(gwinfo->m_gwdeviceid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update gwid=%u in lowload-highload,m_highload_gws.size=%d.\n", gwinfo->m_gwdeviceid, m_gwinfomanager->m_highload_gws.size());
								}

								if (gwinfo->m_load > MCCfgInfo::instance()->get_netmp_highload()) // ������chen songhua������MP��ʱ������netmp��������
								{
									m_gwinfomanager->m_notuse_gws.insert(gwinfo->m_gwdeviceid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update gwid=%u in m_notuse_gws.size=%d.\n", gwinfo->m_gwdeviceid, m_gwinfomanager->m_notuse_gws.size());
								}

								GWInfoData* pGWinfodata = new GWInfoData();
								pGWinfodata->m_gwid = gwinfo->m_gwdeviceid;
								pGWinfodata->m_load = gwinfo->m_load;
								pGWinfodata->m_load2 = gwinfo->m_load2;
								PostToTerThread(e_update_gwinfo_inconf, pGWinfodata);

								if (m_gwinfomanager->m_idle_gws.size() < MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������MP��ʱ������netmp��������
								{
									for (std::map<SR_uint32, GWInfo*>::iterator gwitor = m_gwinfomanager->m_gwinfo.begin();
										gwitor != m_gwinfomanager->m_gwinfo.end(); gwitor++)
									{
										GWInfo* pGWinfo = gwitor->second;
										if (pGWinfo->m_connectionstate != e_GW_sock_inite)
										{
											continue;
										}

										SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
										struct in_addr s; // IPv4��ַ�ṹ��
										s.s_addr = pGWinfo->m_ip;
										inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
										sr_printf(SR_PRINT_INFO, "increase idle_gw next connnect gw(%u)(%s,%u,%u)\n", pGWinfo->m_ip, IPdotdec, pGWinfo->m_port, pGWinfo->m_gwdeviceid);
										SockRelateData srd(kgw_sock_type, pGWinfo->m_gwdeviceid);
										pGWinfo->m_connectionstate = e_GW_sock_connecting;
										break;
									}
								}
							}
						}
					}
				}//else if (dil.svr_type() == 6) // GW
				else if (dil.svr_type() == DEVICE_SERVER::DEVICE_CRS)
				{
					crsinfos = m_crsinfomanager->m_crsinfo.find(dil.deviceid());
					if (m_crsinfomanager->m_crsinfo.end() == crsinfos)//����
					{
						CRSInfo* crsinfo = new CRSInfo();
						crsinfo->m_connectionstate = e_dev_sock_inite;
						crsinfo->m_ip = dil.ip();
						crsinfo->m_port = dil.port();
						crsinfo->m_crsdeviceid = dil.deviceid();
						crsinfo->m_load = dil.load();// ���̿ռ�ʹ����,����16λ��������,��16λ��ʣ������,��λ��GB��
						crsinfo->m_load2 = dil.load2();// cpuʹ����
						crsinfo->m_disktotalcapacity = ((dil.load() & 0xffff0000) >> 16);
						crsinfo->m_diskremaincapacity = (dil.load() & 0x0000ffff);
						m_crsinfomanager->m_crsinfo.insert(std::make_pair(dil.deviceid(), crsinfo));
						sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo new crs[ip=%u,port=%u,crsid=%u,load(disk)=%u,load2(cpu)=%u]\n", dil.ip(), dil.port(), dil.deviceid(), dil.load(), dil.load2());

						{
							//�Ѿ��ﵽ�������
							if (m_crsinfomanager->m_connected_crss.size() >= MCCfgInfo::instance()->get_max_connected_with_netmp())// ������chen songhua������CRS��ʱ������netmp��������
							{
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo have max crs_tcp_connect[cur:%d,cfg:%d]\n",
									m_crsinfomanager->m_connected_crss.size(), MCCfgInfo::instance()->get_max_connected_with_netmp());
								return;
							}

							if (m_crsinfomanager->m_idle_crss.size() >= MCCfgInfo::instance()->get_idle_netmp_nums())// ������chen songhua������CRS��ʱ������netmp��������
							{
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo m_idle_crss=%" SR_PRI_size_t ",cfg_idle_nums=%u\n",
									m_crsinfomanager->m_idle_crss.size(), MCCfgInfo::instance()->get_idle_netmp_nums());
								return;
							}
							//����Ƿ���Ҫ�½�����

							for (std::map<SR_uint32, CRSInfo*>::iterator crsitor = m_crsinfomanager->m_crsinfo.begin();
								crsitor != m_crsinfomanager->m_crsinfo.end(); crsitor++)
							{
								CRSInfo* pCRSinfo = crsitor->second;
								if (pCRSinfo->m_connectionstate != e_dev_sock_inite)
								{
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo pCRSinfo->m_crsdeviceid=%u,m_connectionstate=%u, not inite state continue search.\n", pCRSinfo->m_crsdeviceid, pCRSinfo->m_connectionstate);
									continue;
								}

								if (connect_nums > MCCfgInfo::instance()->get_idle_netmp_nums())// ������chen songhua������CRS��ʱ������netmp��������
								{
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo m_connect_crs=%u,cfg_idle_nums=%u\n",
										connect_nums, MCCfgInfo::instance()->get_idle_netmp_nums());
									break;
								}

								sr_printf(SR_PRINT_DEBUG, "onTcpDataRecv SRMsgId_RspGetDeviceInfo check connect_nums=%d,m_idle_crss=%lu,cfg_idle_crss=%u,connected_crss=%lu\n", connect_nums,
									m_crsinfomanager->m_idle_crss.size(), MCCfgInfo::instance()->get_idle_netmp_nums(),	m_crsinfomanager->m_connected_crss.size());
								connect_nums++;
								SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
								struct in_addr s; // IPv4��ַ�ṹ��
								s.s_addr = pCRSinfo->m_ip;
								inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo check connect crs(%u)(%s,%u,%u)\n", pCRSinfo->m_ip, IPdotdec, pCRSinfo->m_port, pCRSinfo->m_crsdeviceid);
								SockRelateData srd(kcrs_sock_type, pCRSinfo->m_crsdeviceid);
								pCRSinfo->m_connectionstate = e_dev_sock_connecting;
								if (m_deviceMan)
									m_deviceMan->ConnectCRS(IPdotdec, pCRSinfo->m_port, srd);

								sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo check connect crs(%u)(%s,%u,%u) is connecting.....\n", pCRSinfo->m_ip, IPdotdec, pCRSinfo->m_port, pCRSinfo->m_crsdeviceid);
							}
						}//����crs
					}
					else//����crs
					{
						CRSInfo* crsinfo = crsinfos->second;
						if (crsinfo->m_connectionstate != e_dev_sock_connect_ok)
							crsinfo->m_connectionstate = e_dev_sock_inite;
						if (crsinfo->m_ip != dil.ip())
						{
							//TODO:ɾ��֮ǰ��mpid
						}
						if (crsinfo->m_port != dil.port())
						{
							//TODO:
						}

						//SR_uint32 	m_load;// crs:���̿ռ�ʹ����
						//SR_uint32 	m_load2;// crs:cpuʹ����
						if ((crsinfo->m_load != dil.load()) ||
							(crsinfo->m_load2 != dil.load2()))//Loadֵ�иı�
						{
							crsinfo->m_load = dil.load();// crs:���̿ռ�ʹ����,����16λ��������,��16λ��ʣ������,��λ��GB��
							crsinfo->m_load2 = dil.load2();// crs:cpuʹ����

							crsinfo->m_disktotalcapacity = ((dil.load() & 0xffff0000) >> 16);
							crsinfo->m_diskremaincapacity = (dil.load() & 0x0000ffff);

							if (crsinfo->m_connectionstate == e_dev_sock_connect_ok)
							{
								std::set<SR_uint32>::iterator crs_set_itor;
								crs_set_itor = m_crsinfomanager->m_idle_crss.find(crsinfo->m_crsdeviceid);
								if (crs_set_itor != m_crsinfomanager->m_idle_crss.end())
									m_crsinfomanager->m_idle_crss.erase(crs_set_itor);

								//crs_set_itor = m_crsinfomanager->m_highload_crss.find(crsinfo->m_crsdeviceid);
								//if (crs_set_itor != m_crsinfomanager->m_highload_crss.end())
								//	m_crsinfomanager->m_highload_crss.erase(crs_set_itor);

								//crs_set_itor = m_crsinfomanager->m_notuse_crss.find(crsinfo->m_crsdeviceid);
								//if (crs_set_itor != m_crsinfomanager->m_notuse_crss.end())
								//	m_crsinfomanager->m_notuse_crss.erase(crs_set_itor);

								//if (crsinfo->m_load <= MCCfgInfo::instance()->get_netmp_lowload()) // ������chen songhua������CRS��ʱ������netmp��������
								//{
								//	m_crsinfomanager->m_idle_crss.insert(crsinfo->m_crsdeviceid);
								//	sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update crsid=%u in 0-lowload,m_idle_crss.size=%d.\n", crsinfo->m_crsdeviceid, m_crsinfomanager->m_idle_crss.size());
								//}
								//else if ((crsinfo->m_load > MCCfgInfo::instance()->get_netmp_lowload()) && (crsinfo->m_load <= MCCfgInfo::instance()->get_netmp_highload()))// ������chen songhua������CRS��ʱ������netmp��������
								//{
								//	m_crsinfomanager->m_highload_crss.insert(crsinfo->m_crsdeviceid);
								//	sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update crsid=%u in lowload-highload,m_highload_crss.size=%d.\n", crsinfo->m_crsdeviceid, m_crsinfomanager->m_highload_crss.size());
								//}

								//if (crsinfo->m_load > MCCfgInfo::instance()->get_netmp_highload()) // ������chen songhua������CRS��ʱ������netmp��������
								//{
								//	m_crsinfomanager->m_notuse_crss.insert(crsinfo->m_crsdeviceid);
								//	sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update crsid=%u in m_notuse_crss.size=%d.\n", crsinfo->m_crsdeviceid, m_crsinfomanager->m_notuse_crss.size());
								//}

								// ֻҪ����1GB����Ϊ�ǿ���
								if (crsinfo->m_diskremaincapacity > 1)
								{
									m_crsinfomanager->m_idle_crss.insert(crsinfo->m_crsdeviceid);
									sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo add/update crsid=%u diskremaincapacity greater than 1GB,m_idle_crss.size=%d.\n", crsinfo->m_crsdeviceid, m_crsinfomanager->m_idle_crss.size());
								}

								CRSInfoData* pCRSinfodata = new CRSInfoData();
								pCRSinfodata->m_crsid = crsinfo->m_crsdeviceid;
								pCRSinfodata->m_load = crsinfo->m_load;
								pCRSinfodata->m_load2 = crsinfo->m_load2;
								PostToTerThread(e_update_crsinfo_inconf, pCRSinfodata);

								if (m_crsinfomanager->m_idle_crss.size() < MCCfgInfo::instance()->get_idle_netmp_nums()) // ������chen songhua������CRS��ʱ������netmp��������
								{
									for (std::map<SR_uint32, CRSInfo*>::iterator crsitor = m_crsinfomanager->m_crsinfo.begin();
										crsitor != m_crsinfomanager->m_crsinfo.end(); crsitor++)
									{
										CRSInfo* pCRSinfo = crsitor->second;
										if (pCRSinfo->m_connectionstate != e_dev_sock_inite)
										{
											continue;
										}

										SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
										struct in_addr s; // IPv4��ַ�ṹ��
										s.s_addr = pCRSinfo->m_ip;
										inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));
										sr_printf(SR_PRINT_INFO, "increase idle_crs next connnect crs(%u)(%s,%u,%u)\n", pCRSinfo->m_ip, IPdotdec, pCRSinfo->m_port, pCRSinfo->m_crsdeviceid);
										SockRelateData srd(kcrs_sock_type, pCRSinfo->m_crsdeviceid);
										pCRSinfo->m_connectionstate = e_dev_sock_connecting;
										break;
									}
								}
							}
						}
					}
				}//else if (dil.svr_type() == 9) // CRS
				else if (dil.svr_type() == DEVICE_SERVER::DEVICE_SRS)
				{
					time_t timeNow;
					timeNow = time(NULL);

					srs_itor = m_srsinfomanager->m_srsinfo.find(dil.deviceid());
					if (m_srsinfomanager->m_srsinfo.end() == srs_itor)//����
					{
						SRSInfo* srsinfo = new SRSInfo();
						//srsinfo->m_connectionstate = e_dev_sock_inite;
						srsinfo->m_ip = dil.ip();
						srsinfo->m_port = dil.port();
						srsinfo->m_srsdeviceid = dil.deviceid();
						srsinfo->m_load = dil.load();// �Ƿ��Ǹ��ڵ�,0-��,1-��
						srsinfo->m_load2 = dil.load2();// ֱ���ϴ���ռ�õĴ���
						srsinfo->m_srsdevtime = timeNow;

						SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
						struct in_addr s; // IPv4��ַ�ṹ��
						s.s_addr = dil.ip();
						inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));

						// 10.10.6.175
						srsinfo->m_strip = IPdotdec;
						//srsinfo->m_strip = IPdotdec;

						m_srsinfomanager->m_srsinfo.insert(std::make_pair(dil.deviceid(), srsinfo));
						sr_printf(SR_PRINT_INFO, "onTcpDataRecv SRMsgId_RspGetDeviceInfo new srs[ip=%s,port=%u,srsid=%u,isroot=%u]\n", srsinfo->m_strip.c_str(), dil.port(), dil.deviceid(), dil.load());

						SRSInfoData* pSRSinfodata = new SRSInfoData();
						pSRSinfodata->m_srsid = srsinfo->m_srsdeviceid;
						pSRSinfodata->m_strip = srsinfo->m_strip;
						pSRSinfodata->m_port = srsinfo->m_port;
						pSRSinfodata->m_isroot = srsinfo->m_load;
						pSRSinfodata->m_addordel = 1;//����
						pSRSinfodata->m_txbw = srsinfo->m_load2;
						PostToTerThread(e_update_srsinfo_inconf, pSRSinfodata);
					}
					else
					{
						// ���� ʱ��
						srs_itor->second->m_srsdevtime = timeNow;

						SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
						struct in_addr s; // IPv4��ַ�ṹ��
						s.s_addr = dil.ip();
						inet_ntop(AF_INET, (void *)&s, IPdotdec, sizeof(IPdotdec));

						// �����豸�ĸ��ڵ����Է����仯
						if (srs_itor->second->m_load != dil.load())
						{
							// ���ӽڵ��ɸ��ڵ�
							if (srs_itor->second->m_load == 0)
							{
								// ֱ������ڵ����Է����仯
								SRSInfoData* pSRSinfodata = new SRSInfoData();
								pSRSinfodata->m_srsid = dil.deviceid();
								pSRSinfodata->m_strip = IPdotdec;
								pSRSinfodata->m_port = dil.port();
								pSRSinfodata->m_isroot = dil.load();
								pSRSinfodata->m_addordel = 5;// ���ӽڵ��ɸ��ڵ�
								pSRSinfodata->m_txbw = dil.load2();
								PostToTerThread(e_update_srsinfo_inconf, pSRSinfodata);
							}
							// �ɸ��ڵ����ӽڵ�
							if (srs_itor->second->m_load == 1)
							{
								// ֱ������ڵ����Է����仯
								SRSInfoData* pSRSinfodata = new SRSInfoData();
								pSRSinfodata->m_srsid = dil.deviceid();
								pSRSinfodata->m_strip = IPdotdec;
								pSRSinfodata->m_port = dil.port();
								pSRSinfodata->m_isroot = dil.load();
								pSRSinfodata->m_addordel = 6;// �ɸ��ڵ����ӽڵ�
								pSRSinfodata->m_txbw = dil.load2();
								PostToTerThread(e_update_srsinfo_inconf, pSRSinfodata);
							}
						}
						else if (srs_itor->second->m_ip != dil.ip()
							|| srs_itor->second->m_port != dil.port())
						{
							// ֱ������ip���߶˿ڷ����仯
							SRSInfoData* pSRSinfodata = new SRSInfoData();
							pSRSinfodata->m_srsid = dil.deviceid();
							pSRSinfodata->m_strip = IPdotdec;
							pSRSinfodata->m_port = dil.port();
							pSRSinfodata->m_isroot = dil.load();
							pSRSinfodata->m_addordel = 3;// ����ip+port
							pSRSinfodata->m_txbw = dil.load2();
							PostToTerThread(e_update_srsinfo_inconf, pSRSinfodata);
						}
						else if (srs_itor->second->m_load2 != dil.load2())
						{
							// ֱ���ϴ���ռ�ô��������仯
							SRSInfoData* pSRSinfodata = new SRSInfoData();
							pSRSinfodata->m_srsid = dil.deviceid();
							pSRSinfodata->m_strip = IPdotdec;
							pSRSinfodata->m_port = dil.port();
							pSRSinfodata->m_isroot = dil.load();
							pSRSinfodata->m_addordel = 4;// ���¸���
							pSRSinfodata->m_txbw = dil.load2();
							PostToTerThread(e_update_srsinfo_inconf, pSRSinfodata);
						}
					}
				}//else if (dil.svr_type() == 10) // SRS
				
			}// for
			break;
		}
		case SRMsgId_RspUpdateDeviceInfo://��ʱʹ�� RspGetDeviceInfo ���
		{
			break;
		}
		case SRMsgId_RspGetSystemCurLoad:
		{
			SRMsgs::RspGetSystemCurLoad s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_INFO, "devmgr->mc, RspGetSystemCurLoad -- \n%s\n", s.Utf8DebugString().c_str());

			SystemCurLoadData* pninfodata = new SystemCurLoadData();
			pninfodata->m_curconfcnt = s.curconfcnt();
			pninfodata->m_curlivecnt = s.curlivecnt();
			pninfodata->m_curreccnt = s.curreccnt();

			PostToTerThread(e_update_sys_cur_load, pninfodata);

			break;
		}
		case SRMsgId_IndCompLicenseInfo:
		{
			SRMsgs::IndCompLicenseInfo s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_INFO, "devmgr->mc, IndCompLicenseInfo -- \n%s\n", s.Utf8DebugString().c_str());
			for (int i = 0; i < s.compinfos_size();i++)
			{
				CompanyInfo* pcompanyinfo = new CompanyInfo();
				pcompanyinfo->m_compid = s.compinfos(i).companyid();
				pcompanyinfo->m_compname = s.compinfos(i).companyname();
				for (int j = 0; j < s.compinfos(i).licenceinfos_size(); j++)
				{
					CompLicenceInfo* complicinfos = new CompLicenceInfo();
					complicinfos->m_licencetype = s.compinfos(i).licenceinfos(j).licencetype();
					complicinfos->m_licencenum = s.compinfos(i).licenceinfos(j).licencenum();
					complicinfos->m_starttime = s.compinfos(i).licenceinfos(j).starttime();
					complicinfos->m_exptime = s.compinfos(i).licenceinfos(j).exptime();
					pcompanyinfo->m_complicenceinfos.insert(std::make_pair(complicinfos->m_licencetype, complicinfos));
				}
				PostToTerThread(e_notify_to_save_complicenceinfo, new TotalCompLicenceInfoData(pcompanyinfo));
			}
			break;
		}
		case SRMsgId_RspLicenseToMC:
		{
			SRMsgs::RspLicenseToMC s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			LicenseToMCRsp* plincencetomc = new LicenseToMCRsp();
			plincencetomc->m_confid = s.confid();
			plincencetomc->m_seqnumrsp = s.seqnum();
			plincencetomc->m_isok = s.isok();
			for (int i = 0; i < s.compinfos_size(); i++)
			{
				SRMsgs::RspLicenseToMC_CompanyInfo liccomp = s.compinfos(i);
				CompanyInfo * pcompnayinfo = new CompanyInfo();
				pcompnayinfo->m_compid = liccomp.companyid();
				for (int j = 0; j < liccomp.getlicinfos_size(); j++)
				{
					SRMsgs::RspLicenseToMC_LicenceInfo complic = liccomp.getlicinfos(j);
					CompLicenceInfo* pcomplic = new CompLicenceInfo();
					pcomplic->m_licencetype = complic.licencetype();
					pcomplic->m_licencenum = complic.licencenum();
					pcompnayinfo->m_complicenceinfos.insert(std::make_pair(complic.licencetype(), pcomplic));
				}
				plincencetomc->m_rspcompinfos.insert(std::make_pair(liccomp.companyid(), pcompnayinfo));
			}
			sr_printf(SR_PRINT_INFO, "devmgr->mc, RspLicenseToMC -- \n%s\n", s.Utf8DebugString().c_str());
			PostToTerThread(e_rsplicensetomc, new CompLicenceInfoData(plincencetomc));
			break;
		}
		case SRMsgId_RspConfInfoToMC:
		{
			//�������ںܳ�,�ɴ��߳�����,post����һ�߳�
			SRMsgs::RspConfInfoToMC* prspconfinfofromdevmgr = new SRMsgs::RspConfInfoToMC();
			SR_bool isparseok = prspconfinfofromdevmgr->ParsePartialFromArray(pmsgdata->m_buf+sizeof(S_ProtoMsgHeader),protoheader.m_msglen);
			if(false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR,"protobuf_msguid=0x%x,ParseFromArray error\n",protoheader.m_msguid);
				return;
			}
			//// ���������Ϣ
			//SRMsgs::RspConfInfoToMC_Confcfg rsp_confcfg = prspconfinfofromdevmgr->confcfg();
			//SRMsgs::RspConfInfoToMC_AgcSetting rsp_agcsetting = rsp_confcfg.agc_setting();
			//// �޸Ļ�����Ϣ
			//SRMsgs::RspConfInfoToMC_AgcSetting* pAgcSetting = new SRMsgs::RspConfInfoToMC_AgcSetting();
			//pAgcSetting->set_agc_enable(rsp_agcsetting.agc_enable());
			//pAgcSetting->set_target_level_dbfs(rsp_agcsetting.target_level_dbfs());
			//pAgcSetting->set_compression_gain_db(rsp_agcsetting.compression_gain_db());
			//pAgcSetting->set_limiter_enable(rsp_agcsetting.limiter_enable());
			//SRMsgs::RspConfInfoToMC_Confcfg* pConfcfg = new SRMsgs::RspConfInfoToMC_Confcfg();
			//pConfcfg->set_max_audiomix_num(rsp_confcfg.max_audiomix_num()); //������������
			//pConfcfg->set_max_encoder_num(rsp_confcfg.max_encoder_num());//������������������
			//pConfcfg->set_useaudiomixer(rsp_confcfg.useaudiomixer());
			////pConfcfg->set_usevideomixer(rsp_confcfg.usevideomixer());
			//pConfcfg->set_usevideomixer(MCCfgInfo::instance()->get_use_videomixer());
			//pConfcfg->set_mixing_frequency(rsp_confcfg.mixing_frequency());
			//pConfcfg->set_allocated_agc_setting(pAgcSetting);
			//pConfcfg->set_videosize(rsp_confcfg.videosize());

			//prspconfinfofromdevmgr->set_allocated_confcfg(pConfcfg);
			////////////////////////////////////////////////////////////////////////////

			sr_printf(SR_PRINT_INFO,"devmgr->mc, RspConfInfoToMC -- \n%s\n",prspconfinfofromdevmgr->Utf8DebugString().c_str());
			
#ifdef SRMC_DEBUG_TIME

			time_printf("confid %"SR_PRIu64" suid %u sockthread post RspConfInfoToMC _ts_ %lu.%09lu \n",prspconfinfofromdevmgr->confid(),
				prspconfinfofromdevmgr->suid(),
				pmsgdata->m_timestamp.tv_sec,pmsgdata->m_timestamp.tv_nsec);

			time_printf("confid %"SR_PRIu64" suid %u netmpthread recv RspConfInfoToMC _ts_ %lu.%09lu \n",prspconfinfofromdevmgr->confid(),
				prspconfinfofromdevmgr->suid(),
				process_timestamp.tv_sec,process_timestamp.tv_nsec);
#endif

			PostToTerThread(e_rspconfinfotomc, new ConfInfoData(prspconfinfofromdevmgr));
			break;
		}
		case SRMsgId_RspMCConfCreate:
		{
			SRMsgs::RspMCConfCreate s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf+sizeof(S_ProtoMsgHeader),protoheader.m_msglen);
			if(false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR,"protobuf_msguid=0x%x,ParseFromArray error\n",protoheader.m_msguid);
				return;
			}	
			sr_printf(SR_PRINT_INFO,"netmp->mc, RspMCConfCreate -- \n%s\n",s.Utf8DebugString().c_str());
#if 1			
	#ifdef SRMC_DEBUG_TIME
			time_printf("confid %"SR_PRIu64" netmpid %u sockthread post RspMCConfCreate _ts_ %lu.%09lu \n",s.confid(),s.netmpid(),
					pmsgdata->m_timestamp.tv_sec,pmsgdata->m_timestamp.tv_nsec);

			time_printf("confid %"SR_PRIu64" netmpid %u netmpthread recv RspMCConfCreate _ts_ %lu.%09lu \n",s.confid(),s.netmpid(),
					process_timestamp.tv_sec,process_timestamp.tv_nsec);
	#endif
#endif	
			//���͸� �ն˴����߳�
			NetMPCreateConfRsp* ncrsp = new NetMPCreateConfRsp();
			ncrsp->m_isok = s.isok();
			if(s.has_netmpip())
				ncrsp->m_ip.assign(s.netmpip());
			ncrsp->m_netmpid = s.netmpid();
			ncrsp->m_confid = s.confid();
			ncrsp->m_audioctrlrecvport = s.audioctrlrecvport();
			ncrsp->m_audiodatarecvport = s.audiodatarecvport();
			ncrsp->m_videoctrlrecvport = s.videoctrlrecvport();
			ncrsp->m_videodatarecvport = s.videodatarecvport();
			ncrsp->m_video2ctrlrecvport = s.video2ctrlrecvport();
			ncrsp->m_video2datarecvport = s.video2datarecvport();				
			ncrsp->m_video3ctrlrecvport = s.video3ctrlrecvport();
			ncrsp->m_video3datarecvport = s.video3datarecvport();
			ncrsp->m_desktopctrlrecvport = s.desktopctrlrecvport();
			ncrsp->m_desktopdatarecvport = s.desktopdatarecvport();
			if(s.has_failreason())
				ncrsp->m_failreason.assign(s.failreason());
			if (s.has_netmpno())
			{
				ncrsp->m_netmpno = s.netmpno();
			}
			if (s.has_errorcode())
			{
				ncrsp->m_haserrorcode = true;
				ncrsp->m_errorcode = s.errorcode();
			}

			//// 40bitʱ�����1bit��������+ 39bit������,�����ϱ���λ��ʹ��34�꡿ + 18bit�豸id��8bit��devmgr��ţ�+ 10bit�ڸ�devmgr������deviceid��+ 6bit��ҵ�������Ҳ����ͬһ����ͬһdevice�ɲ�����id��
			//ncrsp->m_netmp_conf_detail_id = (Utils::Time() << 24) + (MCCfgInfo::instance()->get_mcdeviceid() << 6);			 

			std::map<SR_uint32,NetMPInfo*>::const_iterator mpcitor = m_netmpinfomanager->m_netmpinfo.find(s.netmpid());
			if(mpcitor == m_netmpinfomanager->m_netmpinfo.end())
				sr_printf(SR_PRINT_ERROR,"%s error\n",__FUNCTION__);
			const NetMPInfo* pnetinfo = mpcitor->second;
			SR_char IPdotdec[40] = {0}; //��ŵ��ʮ����IP��ַ
			struct in_addr addrs; // IPv4��ַ�ṹ��
			addrs.s_addr = pnetinfo->m_ip;
			inet_ntop(AF_INET,(void *)&addrs, IPdotdec, sizeof(IPdotdec));
			ncrsp->m_localip.assign(IPdotdec);
			ncrsp->m_nettype = pnetinfo->m_nettype;
			for (int i = 0; i < s.edgeipports_size(); i++)
			{
				const SRMsgs::RspMCConfCreate_IPPORT& edgeipport = s.edgeipports(i);
				IPPortInfo* pNewedgeipport = new IPPortInfo();
				pNewedgeipport->m_ip = edgeipport.ip();
				pNewedgeipport->m_port = edgeipport.port();

				ncrsp->m_edgeipports.push_back(pNewedgeipport);
			}
			
			//m_netmpinfomanager->m_netmpinconf.insert(s.netmpid());
			NetMPCreateConfInfoData* pmpcfdata = new NetMPCreateConfInfoData(ncrsp);
			//m_terthreadhandle->Post(e_rspnetmpcreateconf,pmpcfdata);
			PostToTerThread(e_rspnetmpcreateconf, pmpcfdata);
			break;
		}
		case SRMsgId_IndTerStatisticsInfoFromNetmp:
		{
			SRMsgs::IndTerStatisticsInfoFromNetmp s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_INFO, "netmp->mc, IndTerStatisticsInfoFromNetmp -- \n%s\n", s.Utf8DebugString().c_str());

			if (s.tsinfo_size() > 0)
			{
				//���͸� �ն˴����߳�
				NetmpIndTerStatisticsInfo* indtsi = new NetmpIndTerStatisticsInfo();
				indtsi->m_confid = s.confid();
				indtsi->m_netmpid = s.netmpid();

				for (SR_int32 i = 0; i < s.tsinfo_size(); ++i)
				{
					const SRMsgs::IndTerStatisticsInfoFromNetmp_TerStatisticsInfo& ind_tsi_msg = s.tsinfo(i);
					if (ind_tsi_msg.terid() != 0)
					{
						TerStatisticsInfo* pTerStatisticsInfo = new TerStatisticsInfo();
						pTerStatisticsInfo->m_terid = ind_tsi_msg.terid();
						pTerStatisticsInfo->m_txbweresult = ind_tsi_msg.txbweresult();
						// pars txinfo
						for (SR_int32 j = 0; j < ind_tsi_msg.txinfo_size(); ++j)
						{
							const SRMsgs::IndTerStatisticsInfoFromNetmp_XfeInfo& txinfo = ind_tsi_msg.txinfo(j);
							SR_bool bhavetxinfo = false;
							TSXfeInfo* pTSTXInfo = new TSXfeInfo();
							for (SR_int32 jj = 0; jj < txinfo.ainfo_size(); ++jj)
							{
								const SRMsgs::IndTerStatisticsInfoFromNetmp_AudioInfo& audioinfo = txinfo.ainfo(jj);
								TSAudioInfo* pTSAudioInfo = new TSAudioInfo();
								pTSAudioInfo->m_atype = audioinfo.atype();
								pTSAudioInfo->m_aterid = audioinfo.aterid();
								pTSAudioInfo->m_baseinfo.m_format = audioinfo.baseinfo().format();
								pTSAudioInfo->m_baseinfo.m_bitrate = audioinfo.baseinfo().bitrate();
								pTSAudioInfo->m_advinfo.m_delay = audioinfo.advinfo().delay();
								pTSAudioInfo->m_advinfo.m_jitter = audioinfo.advinfo().jitter();
								pTSAudioInfo->m_advinfo.m_loss = audioinfo.advinfo().loss();

								pTSTXInfo->m_ainfo.push_back(pTSAudioInfo);
								bhavetxinfo = true;
							}
							for (SR_int32 jj = 0; jj < txinfo.vinfo_size(); ++jj)
							{
								const SRMsgs::IndTerStatisticsInfoFromNetmp_VideoInfo& videoinfo = txinfo.vinfo(jj);
								TSVideoInfo* pTSVideoInfo = new TSVideoInfo();
								pTSVideoInfo->m_vtype = videoinfo.vtype();
								pTSVideoInfo->m_vterid = videoinfo.vterid();
								pTSVideoInfo->m_baseinfo.m_format = videoinfo.baseinfo().format();
								pTSVideoInfo->m_baseinfo.m_bitrate = videoinfo.baseinfo().bitrate();
								pTSVideoInfo->m_advinfo.m_delay = videoinfo.advinfo().delay();
								pTSVideoInfo->m_advinfo.m_jitter = videoinfo.advinfo().jitter();
								pTSVideoInfo->m_advinfo.m_loss = videoinfo.advinfo().loss();

								for (SR_int32 jjj = 0; jjj < videoinfo.frameinfo_size(); ++jjj)
								{
									const SRMsgs::IndTerStatisticsInfoFromNetmp_FrameInfo& frameinfo = videoinfo.frameinfo(jjj);
									TSFrameInfo* pTSFrameInfo = new TSFrameInfo();
									pTSFrameInfo->m_fsw = frameinfo.fsw();
									pTSFrameInfo->m_fsh = frameinfo.fsh();
									pTSFrameInfo->m_fps = frameinfo.fps();
									pTSVideoInfo->m_frameinfo.push_back(pTSFrameInfo);
								}

								pTSTXInfo->m_vinfo.push_back(pTSVideoInfo);
								bhavetxinfo = true;
							}

							if (bhavetxinfo == false)
							{
								delete pTSTXInfo;
							}
							else
							{
								pTerStatisticsInfo->m_txinfo.push_back(pTSTXInfo);
							}
						}
						// pars rxinfo
						for (SR_int32 k = 0; k < ind_tsi_msg.rxinfo_size(); ++k)
						{
							const SRMsgs::IndTerStatisticsInfoFromNetmp_XfeInfo& rxinfo = ind_tsi_msg.rxinfo(k);
							SR_bool bhaverxinfo = false;
							TSXfeInfo* pTSRXInfo = new TSXfeInfo();
							for (SR_int32 kk = 0; kk < rxinfo.ainfo_size(); ++kk)
							{
								const SRMsgs::IndTerStatisticsInfoFromNetmp_AudioInfo& audioinfo = rxinfo.ainfo(kk);
								TSAudioInfo* pTSAudioInfo = new TSAudioInfo();
								pTSAudioInfo->m_atype = audioinfo.atype();
								pTSAudioInfo->m_aterid = audioinfo.aterid();
								pTSAudioInfo->m_baseinfo.m_format = audioinfo.baseinfo().format();
								pTSAudioInfo->m_baseinfo.m_bitrate = audioinfo.baseinfo().bitrate();
								pTSAudioInfo->m_advinfo.m_delay = audioinfo.advinfo().delay();
								pTSAudioInfo->m_advinfo.m_jitter = audioinfo.advinfo().jitter();
								pTSAudioInfo->m_advinfo.m_loss = audioinfo.advinfo().loss();

								pTSRXInfo->m_ainfo.push_back(pTSAudioInfo);
								bhaverxinfo = true;
							}
							for (SR_int32 kk = 0; kk < rxinfo.vinfo_size(); ++kk)
							{
								const SRMsgs::IndTerStatisticsInfoFromNetmp_VideoInfo& videoinfo = rxinfo.vinfo(kk);
								TSVideoInfo* pTSVideoInfo = new TSVideoInfo();
								pTSVideoInfo->m_vtype = videoinfo.vtype();
								pTSVideoInfo->m_vterid = videoinfo.vterid();
								pTSVideoInfo->m_baseinfo.m_format = videoinfo.baseinfo().format();
								pTSVideoInfo->m_baseinfo.m_bitrate = videoinfo.baseinfo().bitrate();
								pTSVideoInfo->m_advinfo.m_delay = videoinfo.advinfo().delay();
								pTSVideoInfo->m_advinfo.m_jitter = videoinfo.advinfo().jitter();
								pTSVideoInfo->m_advinfo.m_loss = videoinfo.advinfo().loss();

								for (SR_int32 kkk = 0; kkk < videoinfo.frameinfo_size(); ++kkk)
								{
									const SRMsgs::IndTerStatisticsInfoFromNetmp_FrameInfo& frameinfo = videoinfo.frameinfo(kkk);
									TSFrameInfo* pTSFrameInfo = new TSFrameInfo();
									pTSFrameInfo->m_fsw = frameinfo.fsw();
									pTSFrameInfo->m_fsh = frameinfo.fsh();
									pTSFrameInfo->m_fps = frameinfo.fps();
									pTSVideoInfo->m_frameinfo.push_back(pTSFrameInfo);
								}

								pTSRXInfo->m_vinfo.push_back(pTSVideoInfo);
								bhaverxinfo = true;
							}

							if (bhaverxinfo == false)
							{
								delete pTSRXInfo;
							}
							else
							{
								pTerStatisticsInfo->m_rxinfo.push_back(pTSRXInfo);
							}
						}
						indtsi->m_tsinfos.push_back(pTerStatisticsInfo);
					}
				}
			
				NetMPIndTerStatisticsInfoData* pindtsidata = new NetMPIndTerStatisticsInfoData(indtsi);
				PostToTerThread(e_netmpindterstatisticsinfo, pindtsidata);
			}
			else
			{
				sr_printf(SR_PRINT_INFO, "netmp->mc, IndTerStatisticsInfoFromNetmp tsinfo_size is null.\n");
			}
			break;
		}
		case SRMsgId_RspMCCreateMPI:
		{
			SRMsgs::RspMCCreateMPI s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_INFO, "mp->mc, RspMCCreateMPI -- \n%s\n", s.Utf8DebugString().c_str());
	
			//���͸� �ն˴����߳�
			MPCreateMPIRsp* mpirsp = new MPCreateMPIRsp();
			mpirsp->m_isok = s.isok();
			mpirsp->m_mpid = s.mpid();
			mpirsp->m_confid = s.confid();
			mpirsp->m_channelid = s.channelid();
			mpirsp->m_relatednetmpid = s.relatednetmpid();
			if (s.has_failreason())
				mpirsp->m_failreason.assign(s.failreason());
			if (s.has_errorcode())
			{
				mpirsp->m_haserrorcode = true;
				mpirsp->m_errorcode = s.errorcode();
			}

			//m_mpinfomanager->m_mpinconf.insert(s.mpid());
			MPCreateMPIInfoData* pmpmpidata = new MPCreateMPIInfoData(mpirsp);
			PostToTerThread(e_rspmpcreatempi, pmpmpidata);
			break;
		}
		//case SRMsgId_RspMPDestoryMPI:
		//{
		//	SRMsgs::RspMPDestoryMPI s;
		//	SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
		//	if (false == isparseok)
		//	{
		//		sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
		//		return;
		//	}
		//	sr_printf(SR_PRINT_INFO, "RspMPDestoryMPI -- \n%s\n", s.Utf8DebugString().c_str());

		//	//���͸� �ն˴����߳�
		//	MPDestoryMPIRsp* mpidestoyrsp = new MPDestoryMPIRsp();
		//	mpidestoyrsp->m_isok = s.isok();
		//	mpidestoyrsp->m_mpid = s.mpid();
		//	mpidestoyrsp->m_confid = s.confid();
		//	if (s.has_failreason())
		//		mpidestoyrsp->m_failreason.assign(s.failreason());

		//	std::map<SR_uint32, MPInfo*>::const_iterator mpcitor = m_mpinfomanager->m_mpinfo.find(s.mpid());
		//	if (mpcitor == m_mpinfomanager->m_mpinfo.end())
		//		sr_printf(SR_PRINT_ERROR, "%s SRMsgId_RspMCCreateMPI error\n", __FUNCTION__);
		//	const MPInfo* pMpinfo = mpcitor->second;
		//	SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
		//	struct in_addr addrs; // IPv4��ַ�ṹ��
		//	addrs.s_addr = pMpinfo->m_ip;
		//	inet_ntop(AF_INET, (void *)&addrs, IPdotdec, sizeof(IPdotdec));
		//	mpidestoyrsp->m_localip.assign(IPdotdec);

		//	//m_mpinfomanager->m_mpinconf.insert(s.mpid());
		//	MPDestoryMPIInfoData* pmpmpidata = new MPDestoryMPIInfoData(mpidestoyrsp);
		//	PostToTerThread(e_rspmpdestroympi, pmpmpidata);
		//	break;
		//}
		case SRMsgId_RspMCCreateScreen:
		{
			SRMsgs::RspMCCreateScreen s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_INFO, "mp->mc, RspMCCreateScreen -- \n%s\n", s.Utf8DebugString().c_str());

			//���͸� �ն˴����߳�
			MPCreateScreenRsp* scrrsp = new MPCreateScreenRsp();
			scrrsp->m_isok = s.isok();
			scrrsp->m_mpid = s.mpid();
			scrrsp->m_confid = s.confid();
			scrrsp->m_channelid = s.channelid();
			scrrsp->m_screenid = s.screenid();
			scrrsp->m_screentype = s.screentype();
			scrrsp->m_relatednetmpid = s.relatednetmpid();
			if (s.has_failreason())
				scrrsp->m_failreason.assign(s.failreason());
			if (s.has_errorcode())
			{
				scrrsp->m_haserrorcode = true;
				scrrsp->m_errorcode = s.errorcode();
			}

			//m_mpinfomanager->m_mpinconf.insert(s.mpid());
			MPCreateScreenInfoData* pmpscreendata = new MPCreateScreenInfoData(scrrsp);
			PostToTerThread(e_rspmpcreatescreen, pmpscreendata);
			break;
		}
		//case SRMsgId_RspMCDestoryScreen:
		//{
		//	SRMsgs::RspMCDestoryScreen s;
		//	SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
		//	if (false == isparseok)
		//	{
		//		sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
		//		return;
		//	}
		//	sr_printf(SR_PRINT_INFO, "RspMCDestoryScreen -- \n%s\n", s.Utf8DebugString().c_str());

		//	//���͸� �ն˴����߳�
		//	MPDestoryScreenRsp* pmdsrsp = new MPDestoryScreenRsp();
		//	pmdsrsp->m_isok = s.isok();
		//	pmdsrsp->m_mpid = s.mpid();
		//	pmdsrsp->m_confid = s.confid();
		//	if (s.has_failreason())
		//		pmdsrsp->m_failreason.assign(s.failreason());

		//	std::map<SR_uint32, MPInfo*>::const_iterator mpcitor = m_mpinfomanager->m_mpinfo.find(s.mpid());
		//	if (mpcitor == m_mpinfomanager->m_mpinfo.end())
		//		sr_printf(SR_PRINT_ERROR, "%s SRMsgId_RspMCCreateScreen error\n", __FUNCTION__);
		//	const MPInfo* pMpinfo = mpcitor->second;
		//	SR_char IPdotdec[40] = { 0 }; //��ŵ��ʮ����IP��ַ
		//	struct in_addr addrs; // IPv4��ַ�ṹ��
		//	addrs.s_addr = pMpinfo->m_ip;
		//	inet_ntop(AF_INET, (void *)&addrs, IPdotdec, sizeof(IPdotdec));
		//	pmdsrsp->m_localip.assign(IPdotdec);

		//	//m_mpinfomanager->m_mpinconf.insert(s.mpid());
		//	MPDestoryScreenInfoData* pmpscreendata = new MPDestoryScreenInfoData(pmdsrsp);
		//	PostToTerThread(e_rspmpdestroyscreen, pmpscreendata);
		//	break;
		//}
		case SRMsgId_CmdMPSeleteVideo:
		{
			SRMsgs::CmdMPSeleteVideo s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "mp->mc, CmdMPSeleteVideo -- \n%s\n", s.Utf8DebugString().c_str());

			//message CmdMPSeleteVideo
			//{
			//	message TermInfo
			//	{
			//		optional uint32 terid = 1;
			//		optional uint32 channelid = 2;// �ն�ͨ��id
			//		optional uint32 videosize = 3;// 0:���� 1:level-1 2:level-2 3:level-3.......n:level-n
			//	}
			//	optional uint64		confid = 1;
			//	optional uint32		mcid = 2;//������Ϣ��mc_deviceid
			//	optional uint32     mpid = 3;//�������������ն�����mp deviceid
			//	optional uint32 	channelid = 4;//���������ն�����ý�崦��ʵ��ͨ��id
			//	optional uint32 	screenid = 5;//���������ն�������Ļid 
			//	repeated TermInfo  	seleteterinfo = 6;
			//}

			//���͸� �ն˴����߳�
			MPSeleteVideoCmd* mpsvcmd = new MPSeleteVideoCmd();
			mpsvcmd->m_confid = s.confid();
			mpsvcmd->m_mcid = s.mcid();
			mpsvcmd->m_mpid = s.mpid();
			mpsvcmd->m_channelid = s.channelid();
			mpsvcmd->m_screenid = s.screenid();

			for (SR_int32 i = 0; i < s.seleteterinfo_size(); ++i)
			{
				const SRMsgs::CmdMPSeleteVideo_TermInfo& cmd_seleteterinfo = s.seleteterinfo(i);
				SeleteTermInfo seleteterinfo_;
				seleteterinfo_.m_terid = cmd_seleteterinfo.terid();
				seleteterinfo_.m_channelid = cmd_seleteterinfo.channelid();
				seleteterinfo_.m_videosize = cmd_seleteterinfo.videosize();
				seleteterinfo_.m_srcid = cmd_seleteterinfo.srcid();
				mpsvcmd->m_mapSeleteTermInfo[cmd_seleteterinfo.terid()] = seleteterinfo_;
			}

			//m_mpinfomanager->m_mpinconf.insert(s.mpid());
			MPSeleteVideoCmdData* pmpsvcmddata = new MPSeleteVideoCmdData(mpsvcmd);
			PostToTerThread(e_cmdmpseletevideo, pmpsvcmddata);

			break;
		}
		case SRMsgId_IndMPLayoutInfo:
		{
			SRMsgs::IndMPLayoutInfo s;
			SR_bool isparseok = s.ParsePartialFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "mp->mc, IndMPLayoutInfo -- \n%s\n", s.Utf8DebugString().c_str());

			////������Ϣ����,MP-->MC
			//message IndMPLayoutInfo
			//{
			//	message PaneInfo
			//	{
			//		optional uint32 paneindex = 1;//����id,��0��ʼ
			//		optional uint32 contenttype = 2;//��������ʾ�������ͣ�0:�ն�; 1:voipλͼ; 2:��
			//		optional uint32 optype = 3;//�����в������ͣ�0:�Զ�; 1:ָ���ն�; 2:��������;3:�Զ���Ѳ.
			//		optional uint32 polltime = 4;//��������е���ʾ����Ϊ��3:�Զ���Ѳ������˴������Ϊ��ѯʱ��������λ����
			//		optional uint32 terid = 5;
			//		optional uint32 channelid = 6;
			//		optional string tername = 7;//�ն�����
			//	}
			//	message LayoutInfo
			//	{
			//		optional uint32 	screenid = 1;//�������ֶ�Ӧ��Ļid
			//		optional uint32		screentype = 2;//�������ֶ�Ӧ��Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ
			//		optional uint32 	layoutmode = 3;//��������ģʽ��0-�Զ���1-������2-2������3-3������4-4����������11-13������12-16������13-¼������14-20������15-25����
			//		repeated PaneInfo 	panes = 4;//������Ϣ
			//	}
			//	optional uint64		confid = 1;//���µĻ���
			//	optional uint32		mcid = 2;//������Ϣ��mc_deviceid
			//	optional uint32     mpid = 3;//������Ϣ��mp deviceid
			//	optional uint32 	channelid = 4;//�������ֶ�Ӧ��Ļ����ý�崦��ʵ��ͨ��
			//	repeated LayoutInfo layoutinfos = 5;//����������Ϣ
			//}

			//���͸� �ն˴����߳�
			MPLayoutinfoInd* mpliind = new MPLayoutinfoInd();
			mpliind->m_confid = s.confid();
			mpliind->m_mcid = s.mcid();
			mpliind->m_mpid = s.mpid();
			mpliind->m_channelid = s.channelid();

			for (SR_int32 i = 0; i < s.layoutinfos_size(); ++i)
			{
				const SRMsgs::IndMPLayoutInfo_LayoutInfo& ind_layoutinfo = s.layoutinfos(i);

				if (ind_layoutinfo.screenid() == 0)
				{
					continue;
				}

				LayoutInfo* playoutinfo = new LayoutInfo();
				playoutinfo->m_screenid = ind_layoutinfo.screenid();
				playoutinfo->m_screentype = ind_layoutinfo.screentype();
				playoutinfo->m_layoutmode = ind_layoutinfo.layoutmode();
				playoutinfo->m_chairfollow = ind_layoutinfo.chairfollow();
				for (SR_int32 j = 0; j < ind_layoutinfo.panes_size(); ++j)
				{
					const SRMsgs::IndMPLayoutInfo_PaneInfo& ind_paneinfo = ind_layoutinfo.panes(j);
					PaneInfo* ppaneinfo = new PaneInfo();
					ppaneinfo->m_paneindex = ind_paneinfo.paneindex();
					ppaneinfo->m_contenttype = ind_paneinfo.contenttype();
					ppaneinfo->m_optype = ind_paneinfo.optype();
					ppaneinfo->m_polltime = ind_paneinfo.polltime();
					ppaneinfo->m_terid = ind_paneinfo.terid();
					ppaneinfo->m_terchannelid = ind_paneinfo.channelid();
					ppaneinfo->m_tername = ind_paneinfo.tername();
					ppaneinfo->m_vsrcid = ind_paneinfo.vsrcid();
					
					if (ppaneinfo->m_optype == 3)
					{
						for (SR_int32 j = 0; j < ind_paneinfo.autopolllist_size(); j++)
						{
							const SRMsgs::IndMPLayoutInfo_AutoPollInfo & cmdmcautopoll = ind_paneinfo.autopolllist(j);
							Autopollinfo* cmdmpautopoll = new Autopollinfo();
							cmdmpautopoll->m_apid = cmdmcautopoll.apid();
							ppaneinfo->m_autopollinfo.insert(std::make_pair(cmdmcautopoll.apid(), cmdmpautopoll));
						}
					}
					playoutinfo->m_paneinfos[ind_paneinfo.paneindex()] = ppaneinfo;
					//playoutinfo->m_paneinfos.insert(ppaneinfo);
				}

				mpliind->m_layoutinfos.insert(playoutinfo);
			}

			if (mpliind->m_layoutinfos.size() > 0)
			{
				MPLayoutinfoIndData* pmpliinddata = new MPLayoutinfoIndData(mpliind);
				PostToTerThread(e_indmplayoutinfo, pmpliinddata);
			}
			else
			{
				// �Ƿ�����Ϣ�����д���
				sr_printf(SR_PRINT_ERROR, "mp->mc, IndMPLayoutInfo msg contain illegal field, do nothing.\n");

				if (mpliind != NULL)
				{
					delete mpliind;
				}
			}

			break;
		}
		case SRMsgId_RspCRSCreateConf:
		{
			SRMsgs::RspCRSCreateConf s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_INFO, "crs->mc, RspCRSCreateConf -- \n%s\n", s.Utf8DebugString().c_str());

			//message RspCRSCreateConf
			//{
			//	optional bool 		isok = 1;
			//	optional uint64		confid = 2;
			//	optional uint32		mcid = 3;//������Ϣ��mc_deviceid 
			//	optional uint32		crsid = 4;//���������crs_deviceid 
			//	optional uint32		crschannelid = 5;//���������crsͨ��id
			//	optional uint32     relatednetmpid = 6;//��CRSͨ��������netmpid
			//	optional string 	failreason = 7;//��ʧ�ܣ������ʧ��˵��	
			//}

			//���͸� �ն˴����߳�
			CRSCreateConfRsp* crsrsp = new CRSCreateConfRsp();
			crsrsp->m_isok = s.isok();
			crsrsp->m_confid = s.confid();
			crsrsp->m_crsid = s.crsid();
			crsrsp->m_crschannelid = s.crschannelid();
			crsrsp->m_relatednetmpid = s.relatednetmpid();
			if (s.has_failreason())
				crsrsp->m_failreason.assign(s.failreason());
			if (s.has_errorcode())
			{
				crsrsp->m_haserrorcode = true;
				crsrsp->m_errorcode = s.errorcode();
			}

			CRSCreateConfInfoData* pcrsdata = new CRSCreateConfInfoData(crsrsp);
			PostToTerThread(e_rspcrscreateconf, pcrsdata);

			break;
		}
		case SRMsgId_RspCRSStartRec:
		{
			SRMsgs::RspCRSStartRec s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_INFO, "crs->mc, RspCRSStartRec -- \n%s\n", s.Utf8DebugString().c_str());
			
			//���͸� �ն˴����߳�
			CRSStartRecRsp* crsrsp = new CRSStartRecRsp();
			crsrsp->m_isok = s.isok();
			crsrsp->m_confid = s.confid();
			crsrsp->m_crsid = s.crsid();
			crsrsp->m_crschannelid = s.crschannelid();
			crsrsp->m_channelid = s.channelid();
			crsrsp->m_screenid = s.screenid();
			crsrsp->m_screentype = s.screentype();
			if (s.has_filestorsvrip())
				crsrsp->m_filestorsvrip.assign(s.filestorsvrip());
			if (s.has_filestorpath())
				crsrsp->m_filestoragepath.assign(s.filestorpath());
			if (s.has_sdefilepath())
				crsrsp->m_sdefilepath.assign(s.sdefilepath());
			if (s.has_failreason())
				crsrsp->m_failreason.assign(s.failreason());
			if (s.has_relativepath())
				crsrsp->m_filerootpath.assign(s.relativepath());
			if (s.has_seqnum())
				crsrsp->m_seqnum = s.seqnum();
			if (s.has_errorcode())
			{
				crsrsp->m_haserrorcode = true;
				crsrsp->m_errorcode = s.errorcode();
			}

			CRSStartRecInfoData* pcrsdata = new CRSStartRecInfoData(crsrsp);
			PostToTerThread(e_rspcrsstartrec, pcrsdata);

			break;
		}
		case SRMsgId_RspCRSStopRec:
		{
			SRMsgs::RspCRSStopRec s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_INFO, "crs->mc, RspCRSStopRec -- \n%s\n", s.Utf8DebugString().c_str());

			//���͸� �ն˴����߳�
			CRSStopRecRsp* crsrsp = new CRSStopRecRsp();
			crsrsp->m_isok = s.isok();
			crsrsp->m_confid = s.confid();
			crsrsp->m_crsid = s.crsid();
			crsrsp->m_crschannelid = s.crschannelid();
			crsrsp->m_channelid = s.channelid();
			crsrsp->m_screenid = s.screenid();
			crsrsp->m_screentype = s.screentype();
			if (s.has_filestorsvrip())
				crsrsp->m_filestorsvrip.assign(s.filestorsvrip());
			if (s.has_filestorpath())
				crsrsp->m_filestoragepath.assign(s.filestorpath());
			if (s.has_sdefilepath())
				crsrsp->m_sdefilepath.assign(s.sdefilepath());
			if (s.has_failreason())
				crsrsp->m_failreason.assign(s.failreason());
			if (s.has_relativepath())
				crsrsp->m_filerootpath.assign(s.relativepath());
			if (s.has_seqnum())
				crsrsp->m_seqnum = s.seqnum();
			if (s.has_filesize())
				crsrsp->m_filesize = s.filesize();
			if (s.has_errorcode())
			{
				crsrsp->m_haserrorcode = true;
				crsrsp->m_errorcode = s.errorcode();
			}

			CRSStopRecInfoData* pcrsdata = new CRSStopRecInfoData(crsrsp);
			PostToTerThread(e_rspcrsstoprec, pcrsdata);

			break;
		}
		case SRMsgId_CmdCRSSeleteVideo:
		{
			SRMsgs::CmdCRSSeleteVideo s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "crs->mc, CmdCRSSeleteVideo -- \n%s\n", s.Utf8DebugString().c_str());

			//���͸� �ն˴����߳�
			CRSSeleteVideoCmd* crssvcmd = new CRSSeleteVideoCmd();
			crssvcmd->m_confid = s.confid();
			crssvcmd->m_mcid = s.mcid();
			crssvcmd->m_crsid = s.crsid();
			crssvcmd->m_crschannelid = s.crschannelid();

			for (SR_int32 i = 0; i < s.seleteterinfo_size(); ++i)
			{
				const SRMsgs::CmdCRSSeleteVideo_TermInfo& cmd_seleteterinfo = s.seleteterinfo(i);
				SeleteTermInfo seleteterinfo_;
				seleteterinfo_.m_terid = cmd_seleteterinfo.terid();
				seleteterinfo_.m_channelid = cmd_seleteterinfo.terid();
				seleteterinfo_.m_screenid = cmd_seleteterinfo.screenid();
				seleteterinfo_.m_videosize = cmd_seleteterinfo.videosize();
				seleteterinfo_.m_recvfps = cmd_seleteterinfo.recvfps();

				crssvcmd->m_mapSeleteTermInfo[cmd_seleteterinfo.terid()] = seleteterinfo_;
			}

			CRSSeleteVideoCmdData* pcrssvcmddata = new CRSSeleteVideoCmdData(crssvcmd);
			PostToTerThread(e_cmdcrsseletevideo, pcrssvcmddata);

			break;
		}
		case SRMsgId_IndCRSFileStorPathToMC:
		{
			SRMsgs::IndCRSFileStorPathToMC s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "crs->mc, IndCRSFileStorPathToMC -- \n%s\n", s.Utf8DebugString().c_str());
			if (s.has_recordstate())
			{
				//���͸� �ն˴����߳�
				CRSFileStorPathInd* crsind = new CRSFileStorPathInd();
				crsind->m_confid = s.confid();
				crsind->m_crsid = s.crsid();
				crsind->m_crschannelid = s.crschannelid();
				crsind->m_channelid = s.channelid();
				crsind->m_screenid = s.screenid();
				crsind->m_screentype = s.screentype();
				if (s.has_filestorsvrip())
					crsind->m_filestorsvrip.assign(s.filestorsvrip());
				if (s.has_filestorpath())
					crsind->m_filestoragepath.assign(s.filestorpath());
				if (s.has_filestorpath())
					crsind->m_sdefilepath.assign(s.sdefilepath());
				if (s.has_relativepath())
					crsind->m_filerootpath.assign(s.relativepath());
				if (s.has_recordstate())
					crsind->m_recordstate = s.recordstate();
				if (s.has_filesize())
					crsind->m_filesize = s.filesize();

				CRSFileStorPathInfoData* pcrsdata = new CRSFileStorPathInfoData(crsind);
				PostToTerThread(e_indcrsfilestorpath, pcrsdata);
			}
			else
			{
				sr_printf(SR_PRINT_ERROR, "crs->mc, IndCRSFileStorPathToMC not have recordstate.\n");
			}

			break;
		}
		case SRMsgId_RspCRSStartLive:
		{
			SRMsgs::RspCRSStartLive s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_INFO, "crs->mc, RspCRSStartLive -- \n%s\n", s.Utf8DebugString().c_str());

			//���͸� �ն˴����߳�
			CRSStartLiveRsp* crsrsp = new CRSStartLiveRsp();
			crsrsp->m_isok = s.isok();
			crsrsp->m_confid = s.confid();
			crsrsp->m_crsid = s.crsid();
			crsrsp->m_crschannelid = s.crschannelid();
			crsrsp->m_channelid = s.channelid();
			crsrsp->m_screenid = s.screenid();
			crsrsp->m_screentype = s.screentype();
			crsrsp->m_relatedsrsid = s.relatedsrsid();
			if (s.has_failreason())
				crsrsp->m_failreason.assign(s.failreason());
			if (s.has_seqnum())
				crsrsp->m_seqnum = s.seqnum();
			if (s.has_liveurl())
				crsrsp->m_liveurl = s.liveurl();
			if (s.has_errorcode())
			{
				crsrsp->m_haserrorcode = true;
				crsrsp->m_errorcode = s.errorcode();
			}

			crsrsp->m_livesvrtype = s.livesvrtype();
			for (int i = 0; i < s.liveaddrs_size(); i++)
			{
				const SRMsgs::RspCRSStartLive_LiveAddr& liveaddr = s.liveaddrs(i);
				if (liveaddr.type() != 0)
				{
					crsrsp->m_liveaddrs.insert(std::make_pair(liveaddr.type(), liveaddr.url()));
				}
			}

			CRSStartLiveInfoData* pcrsdata = new CRSStartLiveInfoData(crsrsp);
			PostToTerThread(e_rspcrsstartlive, pcrsdata);

			break;
		}
		case SRMsgId_IndCRSLiveStateToMC:
		{
			SRMsgs::IndCRSLiveStateToMC s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "crs->mc, IndCRSLiveStateToMC -- \n%s\n", s.Utf8DebugString().c_str());
			if (s.has_livestate())
			{
				// ���͸� �ն˴����߳�
				CRSLiveStateInd* crsind = new CRSLiveStateInd();
				crsind->m_confid = s.confid();
				crsind->m_crsid = s.crsid();
				crsind->m_crschannelid = s.crschannelid();
				crsind->m_channelid = s.channelid();
				crsind->m_screenid = s.screenid();
				crsind->m_screentype = s.screentype();
				crsind->m_livestate = s.livestate();
				
				if (s.has_statecont())
					crsind->m_statecont = s.statecont();
				if (s.has_errorcode())
				{
					crsind->m_haserrorcode = true;
					crsind->m_errorcode = s.errorcode();
				}

				CRSLiveStateInfoData* pcrsdata = new CRSLiveStateInfoData(crsind);
				PostToTerThread(e_indcrslivestate, pcrsdata);
			}
			else
			{
				sr_printf(SR_PRINT_ERROR, "crs->mc, IndCRSLiveStateToMC not have livestate.\n");
			}

			break;
		}
		//case SRMsgId_RspCRSStopLive:
		//{
		//	SRMsgs::RspCRSStopLive s;
		//	SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
		//	if (false == isparseok)
		//	{
		//		sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
		//		return;
		//	}
		//	sr_printf(SR_PRINT_INFO, "crs->mc, RspCRSStopLive -- \n%s\n", s.Utf8DebugString().c_str());

		//	//���͸� �ն˴����߳�
		//	CRSStopLiveRsp* crsrsp = new CRSStopLiveRsp();
		//	crsrsp->m_isok = s.isok();
		//	crsrsp->m_confid = s.confid();
		//	crsrsp->m_crsid = s.crsid();
		//	crsrsp->m_crschannelid = s.crschannelid();
		//	crsrsp->m_channelid = s.channelid();
		//	crsrsp->m_screenid = s.screenid();
		//	crsrsp->m_screentype = s.screentype();
		//	if (s.has_failreason())
		//		crsrsp->m_failreason.assign(s.failreason());
		//	if (s.has_seqnum())
		//		crsrsp->m_seqnum = s.seqnum();
		//	if (s.has_liveurl())
		//		crsrsp->m_liveurl = s.liveurl();

		//	CRSStopLiveInfoData* pcrsdata = new CRSStopLiveInfoData(crsrsp);
		//	PostToTerThread(e_rspcrsstoplive, pcrsdata);

		//	break;
		//}
		case SRMsgId_IndGWQueryMonitorRecToMC:
		{
			SRMsgs::IndGWQueryMonitorRecToMC s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "gw->mc, IndGWQueryMonitorRecToMC -- \n%s\n", s.Utf8DebugString().c_str());
			if (s.queryinfos_size() > 0)
			{
				// ���͸� �ն˴����߳�
				GWQueryMonitorRecInd* gwind = new GWQueryMonitorRecInd();
				gwind->m_confid = s.confid();
				gwind->m_gwid = s.gwid();
				for (int i = 0; i < s.queryinfos_size(); i++)
				{
					MonitorInfo* pmonitor = new MonitorInfo();

					const SRMsgs::IndGWQueryMonitorRecToMC_MonitorInfo& query_mr = s.queryinfos(i);
					pmonitor->m_devicecode = query_mr.devicecode();
					pmonitor->m_querystate = query_mr.querystate();

					const SRMsgs::IndGWQueryMonitorRecToMC_TimeInfo& totalts = query_mr.totaltimespan();
					pmonitor->m_totaltimespan.m_starttime = totalts.starttime();
					pmonitor->m_totaltimespan.m_endtime = totalts.endtime();

					// 0-�ļ�������,1-�ļ�����
					if (pmonitor->m_querystate != 0)
					{
						//const SRMsgs::IndGWQueryMonitorRecToMC_TimeInfo& totalts = query_mr.totaltimespan();
						//pmonitor->m_totaltimespan.m_starttime = totalts.starttime();
						//pmonitor->m_totaltimespan.m_endtime = totalts.endtime();

						for (int j = 0; j < query_mr.subtimespans_size(); j++)
						{
							const SRMsgs::IndGWQueryMonitorRecToMC_TimeInfo& subts = query_mr.subtimespans(j);
							TimeInfo* psubti = new TimeInfo();
							psubti->m_starttime = subts.starttime();
							psubti->m_endtime = subts.endtime();
							pmonitor->m_subtimespans.push_back(psubti);
						}
					}

					gwind->m_queryinfos.push_back(pmonitor);
				}

				GWQueryMonitorRecInfoData* pgwdata = new GWQueryMonitorRecInfoData(gwind);
				PostToTerThread(e_indgwquerymonitorrec, pgwdata);
			}
			else
			{
				sr_printf(SR_PRINT_ERROR, "gw->mc, IndGWQueryMonitorRecToMC not have queryinfos.\n");
			}

			break;
		}
		case SRMsgId_IndTerStatisticsInfoFromGW:
		{
			SRMsgs::IndTerStatisticsInfoFromGW s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_INFO, "gw->mc, IndTerStatisticsInfoFromGW -- \n%s\n", s.Utf8DebugString().c_str());

			if (s.tsinfo_size() > 0)
			{
				//���͸� �ն˴����߳�
				GWIndTerStatisticsInfo* indtsi = new GWIndTerStatisticsInfo();
				indtsi->m_confid = s.confid();
				indtsi->m_gwid = s.gwid();

				for (SR_int32 i = 0; i < s.tsinfo_size(); ++i)
				{
					const SRMsgs::IndTerStatisticsInfoFromGW_TerStatisticsInfo& ind_tsi_msg = s.tsinfo(i);
					if (ind_tsi_msg.terid() != 0)
					{
						TerStatisticsInfo* pTerStatisticsInfo = new TerStatisticsInfo();
						pTerStatisticsInfo->m_terid = ind_tsi_msg.terid();
						pTerStatisticsInfo->m_txbweresult = ind_tsi_msg.txbweresult();
						// pars txinfo
						for (SR_int32 j = 0; j < ind_tsi_msg.txinfo_size(); ++j)
						{
							const SRMsgs::IndTerStatisticsInfoFromGW_XfeInfo& txinfo = ind_tsi_msg.txinfo(j);
							SR_bool bhavetxinfo = false;
							TSXfeInfo* pTSTXInfo = new TSXfeInfo();
							for (SR_int32 jj = 0; jj < txinfo.ainfo_size(); ++jj)
							{
								const SRMsgs::IndTerStatisticsInfoFromGW_AudioInfo& audioinfo = txinfo.ainfo(jj);
								TSAudioInfo* pTSAudioInfo = new TSAudioInfo();
								pTSAudioInfo->m_atype = audioinfo.atype();
								pTSAudioInfo->m_aterid = audioinfo.aterid();
								pTSAudioInfo->m_baseinfo.m_format = audioinfo.baseinfo().format();
								pTSAudioInfo->m_baseinfo.m_bitrate = audioinfo.baseinfo().bitrate();
								pTSAudioInfo->m_advinfo.m_delay = audioinfo.advinfo().delay();
								pTSAudioInfo->m_advinfo.m_jitter = audioinfo.advinfo().jitter();
								pTSAudioInfo->m_advinfo.m_loss = audioinfo.advinfo().loss();

								pTSTXInfo->m_ainfo.push_back(pTSAudioInfo);
								bhavetxinfo = true;
							}
							for (SR_int32 jj = 0; jj < txinfo.vinfo_size(); ++jj)
							{
								const SRMsgs::IndTerStatisticsInfoFromGW_VideoInfo& videoinfo = txinfo.vinfo(jj);
								TSVideoInfo* pTSVideoInfo = new TSVideoInfo();
								pTSVideoInfo->m_vtype = videoinfo.vtype();
								pTSVideoInfo->m_vterid = videoinfo.vterid();
								pTSVideoInfo->m_baseinfo.m_format = videoinfo.baseinfo().format();
								pTSVideoInfo->m_baseinfo.m_bitrate = videoinfo.baseinfo().bitrate();
								pTSVideoInfo->m_advinfo.m_delay = videoinfo.advinfo().delay();
								pTSVideoInfo->m_advinfo.m_jitter = videoinfo.advinfo().jitter();
								pTSVideoInfo->m_advinfo.m_loss = videoinfo.advinfo().loss();

								for (SR_int32 jjj = 0; jjj < videoinfo.frameinfo_size(); ++jjj)
								{
									const SRMsgs::IndTerStatisticsInfoFromGW_FrameInfo& frameinfo = videoinfo.frameinfo(jjj);
									TSFrameInfo* pTSFrameInfo = new TSFrameInfo();
									pTSFrameInfo->m_fsw = frameinfo.fsw();
									pTSFrameInfo->m_fsh = frameinfo.fsh();
									pTSFrameInfo->m_fps = frameinfo.fps();
									pTSVideoInfo->m_frameinfo.push_back(pTSFrameInfo);
								}

								pTSTXInfo->m_vinfo.push_back(pTSVideoInfo);
								bhavetxinfo = true;
							}

							if (bhavetxinfo == false)
							{
								delete pTSTXInfo;
							}
							else
							{
								pTerStatisticsInfo->m_txinfo.push_back(pTSTXInfo);
							}
						}
						// pars rxinfo
						for (SR_int32 k = 0; k < ind_tsi_msg.rxinfo_size(); ++k)
						{
							const SRMsgs::IndTerStatisticsInfoFromGW_XfeInfo& rxinfo = ind_tsi_msg.rxinfo(k);
							SR_bool bhaverxinfo = false;
							TSXfeInfo* pTSRXInfo = new TSXfeInfo();
							for (SR_int32 kk = 0; kk < rxinfo.ainfo_size(); ++kk)
							{
								const SRMsgs::IndTerStatisticsInfoFromGW_AudioInfo& audioinfo = rxinfo.ainfo(kk);
								TSAudioInfo* pTSAudioInfo = new TSAudioInfo();
								pTSAudioInfo->m_atype = audioinfo.atype();
								pTSAudioInfo->m_aterid = audioinfo.aterid();
								pTSAudioInfo->m_baseinfo.m_format = audioinfo.baseinfo().format();
								pTSAudioInfo->m_baseinfo.m_bitrate = audioinfo.baseinfo().bitrate();
								pTSAudioInfo->m_advinfo.m_delay = audioinfo.advinfo().delay();
								pTSAudioInfo->m_advinfo.m_jitter = audioinfo.advinfo().jitter();
								pTSAudioInfo->m_advinfo.m_loss = audioinfo.advinfo().loss();

								pTSRXInfo->m_ainfo.push_back(pTSAudioInfo);
								bhaverxinfo = true;
							}
							for (SR_int32 kk = 0; kk < rxinfo.vinfo_size(); ++kk)
							{
								const SRMsgs::IndTerStatisticsInfoFromGW_VideoInfo& videoinfo = rxinfo.vinfo(kk);
								TSVideoInfo* pTSVideoInfo = new TSVideoInfo();
								pTSVideoInfo->m_vtype = videoinfo.vtype();
								pTSVideoInfo->m_vterid = videoinfo.vterid();
								pTSVideoInfo->m_baseinfo.m_format = videoinfo.baseinfo().format();
								pTSVideoInfo->m_baseinfo.m_bitrate = videoinfo.baseinfo().bitrate();
								pTSVideoInfo->m_advinfo.m_delay = videoinfo.advinfo().delay();
								pTSVideoInfo->m_advinfo.m_jitter = videoinfo.advinfo().jitter();
								pTSVideoInfo->m_advinfo.m_loss = videoinfo.advinfo().loss();

								for (SR_int32 kkk = 0; kkk < videoinfo.frameinfo_size(); ++kkk)
								{
									const SRMsgs::IndTerStatisticsInfoFromGW_FrameInfo& frameinfo = videoinfo.frameinfo(kkk);
									TSFrameInfo* pTSFrameInfo = new TSFrameInfo();
									pTSFrameInfo->m_fsw = frameinfo.fsw();
									pTSFrameInfo->m_fsh = frameinfo.fsh();
									pTSFrameInfo->m_fps = frameinfo.fps();
									pTSVideoInfo->m_frameinfo.push_back(pTSFrameInfo);
								}

								pTSRXInfo->m_vinfo.push_back(pTSVideoInfo);
								bhaverxinfo = true;
							}

							if (bhaverxinfo == false)
							{
								delete pTSRXInfo;
							}
							else
							{
								pTerStatisticsInfo->m_rxinfo.push_back(pTSRXInfo);
							}
						}
						indtsi->m_tsinfos.push_back(pTerStatisticsInfo);
					}
				}

				GWIndTerStatisticsInfoData* pindtsidata = new GWIndTerStatisticsInfoData(indtsi);
				PostToTerThread(e_gwindterstatisticsinfo, pindtsidata);
			}
			else
			{
				sr_printf(SR_PRINT_INFO, "gw->mc, IndTerStatisticsInfoFromGW tsinfo_size is null.\n");
			}
			break;
		}
		case SRMsgId_RspTerJoinConfFromRelayserver:
		{
			SRMsgs::RspTerJoinConfFromRelayserver s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "relaysvr->mc, RspTerJoinConfFromRelayserver -- \n%s\n", s.Utf8DebugString().c_str());

			// ���͸� �ն˴����߳�
			RelaySvrTJCRsp* relaysvrrsp = new RelaySvrTJCRsp();
			relaysvrrsp->m_isok = s.isok();
			relaysvrrsp->m_failreason = s.failreason();
			relaysvrrsp->m_confid = s.confid();
			relaysvrrsp->m_mcid = s.mcid();
			relaysvrrsp->m_relaysvrid = s.relaysvrid();
			relaysvrrsp->m_suid = s.suid();
			if (s.has_errorcode())
			{
				relaysvrrsp->m_haserrorcode = true;
				relaysvrrsp->m_errorcode = s.errorcode();
			}

			// Ŀǰֻ����relaysvr��������ַ
			const SRMsgs::RspTerJoinConfFromRelayserver_IPPORT& relaysvripport = s.relaysvraddrs();
			
			relaysvrrsp->m_relaysvraddr.m_nettype = relaysvripport.nettype();
			relaysvrrsp->m_relaysvraddr.m_ip = relaysvripport.ip();
			relaysvrrsp->m_relaysvraddr.m_port = relaysvripport.port();

			RelaySvrRspTJCInfoData* prsvrdata = new RelaySvrRspTJCInfoData(relaysvrrsp);
			PostToTerThread(e_relaysvr_rsp_terjoinconf, prsvrdata);

			break;
		}
		case SRMsgId_IndMCHeartBeatOfNetMP:
		{
			SRMsgs::IndMCHeartBeatOfNetMP s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "netmp->mc, IndMCHeartBeatOfNetMP -- \n%s\n", s.Utf8DebugString().c_str());

			break;
		}
		case SRMsgId_IndMPHeartBeatToMC:
		{
			SRMsgs::IndMPHeartBeatToMC s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "mp->mc, IndMPHeartBeatToMC -- \n%s\n", s.Utf8DebugString().c_str());

			break;
		}
		case SRMsgId_IndGWHeartBeatToMC:
		{
			SRMsgs::IndGWHeartBeatToMC s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "gw->mc, IndGWHeartBeatToMC -- \n%s\n", s.Utf8DebugString().c_str());

			break;
		}
		case SRMsgId_IndCRSHeartBeatToMC:
		{
			SRMsgs::IndCRSHeartBeatToMC s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "crs->mc, IndCRSHeartBeatToMC -- \n%s\n", s.Utf8DebugString().c_str());

			break;
		}
		case SRMsgId_IndRelayserverHeartBeatToMC:
		{
			SRMsgs::IndRelayserverHeartBeatToMC s;
			SR_bool isparseok = s.ParseFromArray(pmsgdata->m_buf + sizeof(S_ProtoMsgHeader), protoheader.m_msglen);
			if (false == isparseok)
			{
				sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x,ParseFromArray error\n", protoheader.m_msguid);
				return;
			}
			sr_printf(SR_PRINT_DEBUG, "relayserver->mc, IndRelayserverHeartBeatToMC -- \n%s\n", s.Utf8DebugString().c_str());

			break;
		}
		default:
		{
			sr_printf(SR_PRINT_ERROR, "protobuf_msguid=0x%x is unknow.\n", protoheader.m_msguid);
		}
	}	
}

//����DevMgr
SR_void DevMgrNetMPProcessThread::startUp()
{
	for(std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
		devs_itor != m_netmpinfomanager->m_devmgrInfos.end();++devs_itor)
	{
		DevSockInfo* pdsinfo = *devs_itor;
		if((NULL == pdsinfo->m_sockptr) && (e_DevMgr_sock_init == pdsinfo->m_devsockstat))
		{
			pdsinfo->m_sockptr = m_deviceMan->ConnectDevmgr(pdsinfo->m_ip.c_str(),pdsinfo->m_port);
			m_netmpinfomanager->m_current_dev_connectcnt++;//ÿ����һ�μ�1
			if(false == m_deviceMan->isConnecting())
			{
				sr_printf(SR_PRINT_ERROR, "startUp devmgr(ip=%s, port=%d) is failed.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port);
				pdsinfo->m_devsockstat = e_DevMgr_sock_error;
				pdsinfo->m_sockptr = NULL;
			}
			else
			{
				sr_printf(SR_PRINT_NORMAL, "startUp devmgr(ip=%s, port=%d) is connecting, m_sockptr=%p.\n", pdsinfo->m_ip.c_str(), pdsinfo->m_port, pdsinfo->m_sockptr);
				pdsinfo->m_devsockstat = e_DevMgr_sock_connecting;
				m_netmpinfomanager->m_current_devmgr_sockptr = pdsinfo->m_sockptr;
				return;
			}
		}
	}

	//��ʱû���ҵ����õ�devmgr,��ʱȥ��֮ǰ�����ϵ�
	for(std::set<DevSockInfo*>::iterator devs_itor = m_netmpinfomanager->m_devmgrInfos.begin();
		devs_itor != m_netmpinfomanager->m_devmgrInfos.end();++devs_itor)
	{
		DevSockInfo* pdsinfo = *devs_itor;
		pdsinfo->m_devsockstat = e_DevMgr_sock_init;
		pdsinfo->m_sockptr = NULL;
	}

	//m_netmpinfomanager->m_connect_dev_loopcnt++;//ÿ��������һ�ֺ��1
	sr_printf(SR_PRINT_WARN, "startUp all devmgr is failed, so continue search other devmgr.\n");

	//createTimer(MCCfgInfo::instance()->get_reconnect_devmgr_time(),e_dispoable_timer,e_connect_devmgr_timer,0ull);
	Msgid_Timerid_Pair* ppair = NULL;
	ppair = new Msgid_Timerid_Pair();
	if (ppair != NULL)
	{
		ppair->m_msgid = e_connect_devmgr_timer;
		ppair->m_ptimer = NULL;
		//��ʱ��ʱ��ǳ��̵�ʱ��,����֮�� ppair ��δ��ֵ ��Ӱ����
		ppair->m_ptimer = createTimer(MCCfgInfo::instance()->get_reconnect_devmgr_time(), e_dispoable_timer, e_connect_devmgr_timer, (SR_uint64)ppair);
		if (ppair->m_ptimer != NULL)
		{
			sr_printf(SR_PRINT_CRIT, "startUp all devmgr is failed,ppair=%p [msgid=0x%x, timerid=%p] push_back list for continue search other devmgr.\n", ppair, ppair->m_msgid, ppair->m_ptimer);
			m_connect_devmgr_timer_list.push_back(ppair);
		}
		else
		{
			sr_printf(SR_PRINT_CRIT, "startUp all devmgr is failed,createTimer is null.\n");
			delete ppair;
		}
	}
	else
	{
		sr_printf(SR_PRINT_CRIT, "startUp all devmgr is failed,new ppair is null.\n");
	}
}

SR_void DevMgrNetMPProcessThread::OnMessage(Message* msg)
{
	SR_uint32 messageid = msg->message_id;
	Simple_ScopedPtr autofree(msg->pdata);

	if(e_sock_rw_error == messageid)
	{
		onSockError(static_cast<SockErrorData*>(msg->pdata));
	}
	if(e_timerid == messageid)
	{
		this->onTimerArrive(static_cast<TimerData*>(msg->pdata));
	}
	else if(e_sock_connect_ing == messageid)// need nothing!
	{
	}
	else if(e_sock_connect_ok == messageid)
	{
		this->onConnectSuccess(static_cast<SockRelateData*>(msg->pdata));
	}
	else if(e_sock_recv_data == messageid)
	{
		this->onTcpDataRecv(static_cast<SockTcpData*>(msg->pdata));
	}
	else if (e_notify_to_startup == messageid)
	{
		this->startUp();
	}
}

} //namespace SRMC