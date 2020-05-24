#ifndef SRMCPROCESS_H
#define SRMCPROCESS_H

#include <set>
#include <map>
#include <string>
#include "mc_ter.pb.h"
#include "mc_ter_plus.pb.h"
#include "mc_netmp.pb.h"
#include "device_ser.pb.h"
#include "mc_mp.pb.h"
#include "mc_crs.pb.h"
#include "mc_relayserver.pb.h"
#include "asyncthread.h"
#include "sr_message.h"
#include "sr_queuemsgid.h"
#include "ini_configuration.h"
#include "sr_tcpheader.h"

#include "SRMediaCommon.h"
//#include "srDES.h"

#define MAX_BUFFER_MSG_SIZE (50u)
#define MAX_BUFFER_MSG_SIZE_DEVICE (200)

class DeviceManager;
class TimerManager;
namespace SRMC{

class MeetingRoom;
class ConfInfoData;
class NetMPCreateConfInfoData;
class MPCreateMPIInfoData;
class MPCreateScreenInfoData;
class SRTer;
class TerJoinConfReq;
class NetMPCreateConfRsp;
class BufferedProtoMsgPair;

#define POLL_NUM 5
#define MAX_PANNE_INDEX 25

typedef enum{
	e_Media_EA_UNKNOWN = 0x0,
	e_Media_EA_AES_128 = 0x1,
	e_Media_EA_SM4 = 0x2
}E_MediaEAType;

#define ENCRYPTION_KEY_LEN 16


typedef std::set<SR_uint32> TeridSet_t;
typedef std::map<SR_uint32, SR_uint32> TeridNetmpidMap_t;
typedef std::map<SR_uint32, SR_uint32> MPiChannelidMPidMap_t;
typedef std::map<SR_uint32, SR_uint32> ScridMPiChannelidMap_t;

typedef std::map<SR_uint32, MPCreateScreenRsp*> ScreenidMPcsrspMap_t;

typedef std::map<SR_uint32, SR_uint32> TeridSeqnumMap_t;


class ConnectedNetMPInfo
{
public:
	ConnectedNetMPInfo()
	{
		m_ters.clear();
		m_terswaitrsp.clear();
		m_terswaitreopenrsp.clear();
		m_portsinfo.clear();		
		m_pSocket = NULL;
		m_load2 = 0u;// Ĭ�ϵ�����Ϊ0
		m_load = 0u;
		m_netmpdeviceid = 0u;
		m_netmpgroupid = 0u;
		m_mapinternetips.clear();
		m_nettype = 0u;
		m_max_bandwidth = 0;
		m_max_terms = 0;
		m_ip.clear();
		m_ipandport.clear();
	}

	//~ConnectedNetMPInfo()
	//{
	//	m_ters.clear();
	//	m_terswaitrsp.clear();
	//	m_terswaitreopenrsp.clear();
	//	m_portsinfo.clear();
	//	m_pSocket = NULL;
	//	m_load2 = 0u;// Ĭ�ϵ�����Ϊ0
	//	m_load = 0u;
	//}

	SR_uint32   m_netmpdeviceid;
	SR_uint32   m_load;
	SR_uint32   m_load2;//�� netmp ���Ѿ��ٿ����ն˸���
	SR_uint32   m_netmpgroupid;
	SR_void*    m_pSocket;
	std::list<std::string> m_mapinternetips;
	SR_uint32   m_nettype;// �������ָ�netmp�ǲ������ַ�ʽע���
	SR_uint32   m_max_bandwidth;//netmp�ɳ��ص�������
	SR_uint32   m_max_terms;//netmp�ɳ��ص�����ն���
	std::string   m_ip;
	std::string   m_ipandport;
	//TODO: map �� value ��Ϊ ָ��
	std::map<SR_uint64,TeridSet_t> m_ters;//key��confid���ڸ�netmp�Ͽ�����ն�
	std::map<SR_uint64,TeridSet_t> m_terswaitrsp;//key��confid���ȴ���netmp���ض˿���Դ���ն�,suid
	std::map<SR_uint64,TeridSet_t> m_terswaitreopenrsp;//key��confid������Ǩ�Ƶȴ���netmp���ض˿���Դ���ն�
	std::map<SR_uint64,NetMPCreateConfRsp*> m_portsinfo; // key��confid

	SR_uint32 getAllTersNumInConf(SR_uint64 confid) const ;
	SR_uint32 getAllTersNum() const ;
	SR_uint32 getNetmpAlreadyTersNum() const; // ����netmp�ϵ������ն˸��� load2 (+MC����Ļ�δ����?)
};

class ConnectedMPInfo
{
public:
	ConnectedMPInfo()
	{
		m_terms.clear();
		m_terswaitmprsp.clear();
		m_waitmpreopenrspters.clear();
		m_mpiinfo.clear();
		m_screeninfo.clear();
		m_pSocket = NULL;
		m_load2 = 0u;// Ĭ�ϵ�����Ϊ0
		m_load = 0u;
	}

	~ConnectedMPInfo()
	{
		for (std::map<SR_uint64, TeridNetmpidMap_t>::iterator ter_itor = m_terms.begin(); ter_itor != m_terms.end(); ter_itor++)
		{
			TeridNetmpidMap_t* pternetmppairmap = NULL;
			pternetmppairmap = &(ter_itor->second);
			if (pternetmppairmap != NULL)
			{
				pternetmppairmap->clear();
			}
		}
		m_terms.clear();

		for (std::map<SR_uint64, TeridNetmpidMap_t>::iterator waitmprsp_ter_itor = m_terswaitmprsp.begin(); waitmprsp_ter_itor != m_terswaitmprsp.end(); waitmprsp_ter_itor++)
		{
			TeridNetmpidMap_t* pternetmppairmap = NULL;
			pternetmppairmap = &(waitmprsp_ter_itor->second);
			if (pternetmppairmap != NULL)
			{
				pternetmppairmap->clear();
			}
		}
		m_terswaitmprsp.clear();

		for (std::map<SR_uint64, TeridNetmpidMap_t>::iterator waitmpreopenrsp_ter_itor = m_waitmpreopenrspters.begin();
			waitmpreopenrsp_ter_itor != m_waitmpreopenrspters.end(); waitmpreopenrsp_ter_itor++)
		{
			TeridNetmpidMap_t* pternetmppairmap = NULL;
			pternetmppairmap = &(waitmpreopenrsp_ter_itor->second);
			if (pternetmppairmap != NULL)
			{
				pternetmppairmap->clear();
			}
		}
		m_waitmpreopenrspters.clear();

		for (std::map<SR_uint64, MPCreateMPIRsp*>::iterator mpcrtmpirsp_itor = m_mpiinfo.begin();
			mpcrtmpirsp_itor != m_mpiinfo.end(); mpcrtmpirsp_itor++)
		{
			MPCreateMPIRsp* pmpcrtmpirsp = NULL;
			pmpcrtmpirsp = mpcrtmpirsp_itor->second;
			if (pmpcrtmpirsp != NULL)
			{
				delete pmpcrtmpirsp;
				pmpcrtmpirsp = NULL;
			}
		}
		m_mpiinfo.clear();

		for (std::map<SR_uint64, ScreenidMPcsrspMap_t>::iterator mpcrtscrrsp_itor = m_screeninfo.begin();
			mpcrtscrrsp_itor != m_screeninfo.end(); mpcrtscrrsp_itor++)
		{
			for (ScreenidMPcsrspMap_t::iterator scrid_mpcrtrsp_itor = mpcrtscrrsp_itor->second.begin();
				scrid_mpcrtrsp_itor != mpcrtscrrsp_itor->second.end(); scrid_mpcrtrsp_itor++)
			{
				MPCreateScreenRsp* pmpcrtrsp = NULL;
				pmpcrtrsp = scrid_mpcrtrsp_itor->second;
				if (pmpcrtrsp != NULL)
				{
					delete pmpcrtrsp;
					pmpcrtrsp = NULL;
				}
			}
			mpcrtscrrsp_itor->second.clear();
		}
		m_screeninfo.clear();
		m_pSocket = NULL;
		m_load2 = 0u;// Ĭ�ϵ�����Ϊ0
		m_load = 0u;
	}
	SR_uint32   m_mpdeviceid;
	SR_uint32   m_load;// mp:���д�����netmp->mp��
	SR_uint32   m_load2;// mp:cpu����
	SR_void*    m_pSocket;
	//TODO: map �� value ��Ϊ ָ��
	std::map<SR_uint64, TeridNetmpidMap_t> m_terms;//key��confid���ڸ�mp�Ͽ�����ն�
	//std::map<SR_uint64, TeridSet_t> m_terswaitmprsp;//key��confid���ȴ���mp����ý��ʵ��ͨ�����ն�
	std::map<SR_uint64, TeridNetmpidMap_t> m_terswaitmprsp;//key��confid���ȴ���mp����ý��ʵ��ͨ�����ն�
	std::map<SR_uint64, TeridNetmpidMap_t> m_waitmpreopenrspters;//key��confid������Ǩ�Ƶȴ���mp����mpi��Դ���ն�
	std::map<SR_uint64, MPCreateMPIRsp*> m_mpiinfo; // key��confid����ʱ������һ��������MPI(������MPIʱ���迼�ǽ�MPCreateMPIRsp*ָ���װ��map�ṹ<key��mpichannelid,value:MPCreateMPIRsp*>)
	std::map<SR_uint64, ScreenidMPcsrspMap_t> m_screeninfo; // key��confid , <key��screenid,value:MPCreateMPIRsp*>

	SR_uint32 getAllTersNumInMPConf(SR_uint64 confid) const;
	SR_uint32 getAllTersNumInMP() const;
	SR_uint32 getMPAlreadyTersNum() const; // ����mp�ϵ������ն˸��� load2 (+MC����Ļ�δ����?)
	SR_uint32 getMPCurrentCpuLoad() const; // ����mp��cpu����
};

class ConnectedGWInfo
{
public:
	ConnectedGWInfo()
	{
		//m_terms.clear();
		m_pSocket = NULL;
		m_load2 = 0u;// Ĭ�ϵ�����Ϊ0
		m_load = 0u;
	}

	~ConnectedGWInfo()
	{
		//for (std::map<SR_uint64, TeridNetmpidMap_t>::iterator ter_itor = m_terms.begin(); ter_itor != m_terms.end(); ter_itor++)
		//{
		//	TeridNetmpidMap_t* pternetmppairmap = NULL;
		//	pternetmppairmap = &(ter_itor->second);
		//	if (pternetmppairmap != NULL)
		//	{
		//		pternetmppairmap->clear();
		//	}
		//}
		//m_terms.clear();

		
		m_pSocket = NULL;
		m_load2 = 0u;// Ĭ�ϵ�����Ϊ0
		m_load = 0u;
	}
	SR_uint32   m_gwdeviceid;
	SR_uint32   m_load;// gw:�ն���
	SR_uint32   m_load2;// gw:cpu����
	SR_void*    m_pSocket;
	////TODO: map �� value ��Ϊ ָ��
	//std::map<SR_uint64, TeridNetmpidMap_t> m_terms;//key��confid���ڸ�mp�Ͽ�����ն�

	//SR_uint32 getAllTersNumInGWConf(SR_uint64 confid) const;
	//SR_uint32 getAllTersNumInMP() const;
	//SR_uint32 getGWAlreadyTersNum() const; // ����mp�ϵ������ն˸��� load2 (+MC����Ļ�δ����?)
	////SR_uint32 getGWCurrentCpuLoad() const; // ����mp��cpu����
};

class ConnectedCRSInfo
{
public:
	ConnectedCRSInfo()
	{
		m_pSocket = NULL;
		m_load2 = 0u;// Ĭ�ϵ�����Ϊ0
		m_load = 0u;
		m_disktotalcapacity = 0u;
		m_diskremaincapacity = 0u;
	}

	~ConnectedCRSInfo()
	{
		m_pSocket = NULL;
		m_load2 = 0u;// Ĭ�ϵ�����Ϊ0
		m_load = 0u;
		m_disktotalcapacity = 0u;
		m_diskremaincapacity = 0u;
	}
	SR_uint32   m_crsdeviceid;
	SR_uint32   m_load;// crs:���̿ռ�ʹ����,����16λ��������,��16λ��ʣ������,��λ��GB��
	SR_uint32   m_load2;// crs:cpuʹ����
	SR_void*    m_pSocket;
	SR_uint32 	m_disktotalcapacity;// ����������
	SR_uint32 	m_diskremaincapacity;// ����ʣ������
};

class ConnectedSRSInfo
{
public:
	ConnectedSRSInfo()
	{
		m_srsdeviceid = 0u;
		m_strip.clear();
		m_port = 0u;
		m_isroot = 0u;
		m_txbw = 0u;
	}

	~ConnectedSRSInfo()
	{
		m_srsdeviceid = 0u;
		m_strip.clear();
		m_port = 0u;
		m_isroot = 0u;
		m_txbw = 0u;
	}
	SR_uint32   m_srsdeviceid;
	std::string m_strip;
	SR_uint32   m_port;
	SR_uint32   m_isroot;// �Ƿ��Ǹ��ڵ�,0-��,1-��
	SR_uint32   m_txbw;
};

class ConnectedRelaySvrInfo
{
public:
	ConnectedRelaySvrInfo()
	{
		m_pSocket = NULL;
		m_ternum = 0u;// Ĭ�ϵ�����Ϊ0
		m_tx = 0u;
	}

	~ConnectedRelaySvrInfo()
	{
		m_pSocket = NULL;
		m_ternum = 0u;// Ĭ�ϵ�����Ϊ0
		m_tx = 0u;
	}
	SR_uint32   m_relaysvrdevid;
	SR_uint32   m_ternum;// relayserver:�����ն���
	SR_uint32   m_tx;// relayserver:����(����)����
	SR_void*    m_pSocket;

	std::map<SR_uint64, TeridSet_t> m_ters;//key��confid���ڸ�relaysvr��Ԥ������ն�suid
	std::map<SR_uint64, TeridSet_t> m_terswaitrsp;//key��confid���ȴ���relaysvr���ض˿���Դ���ն�,suid
	std::map<SR_uint64, TeridSet_t> m_terswaitreopenrsp;//key��confid������Ǩ�Ƶȴ���relaysvr���ض˿���Դ���ն�
};

class ConnectedRelayMcInfo
{
public:
	ConnectedRelayMcInfo()
	{
		m_relaymcid = 0u;// Ĭ�ϵ�����Ϊ0
		m_groupid = 0u;// Ĭ�ϵ�����Ϊ0
		m_strip.clear();
		m_ip = 0u;// Ĭ�ϵ�����Ϊ0
		m_port = 0u;// Ĭ�ϵ�����Ϊ0
		m_ternum = 0u;// Ĭ�ϵ�����Ϊ0
		m_mapinternetips.clear();
	}

	~ConnectedRelayMcInfo()
	{
		m_relaymcid = 0u;// Ĭ�ϵ�����Ϊ0
		m_groupid = 0u;// Ĭ�ϵ�����Ϊ0
		m_strip.clear();
		m_ip = 0u;// Ĭ�ϵ�����Ϊ0
		m_port = 0u;// Ĭ�ϵ�����Ϊ0
		m_ternum = 0u;// Ĭ�ϵ�����Ϊ0
		m_mapinternetips.clear();
	}
	SR_uint32 m_relaymcid;
	SR_uint32 m_groupid;
	std::string m_strip;
	SR_uint32 m_ip;
	SR_uint32 m_port;
	SR_uint32 m_ternum;// relayserver:�����ն���
	std::list<std::string> m_mapinternetips;
};
class NetMPCreateConfRsp;

typedef std::map<SR_uint64,TeridSet_t> Confid_Ters_t;
//����
typedef std::set<SR_uint32> TerSuidSet;
typedef std::map<SR_uint64,TerSuidSet*> Confid_Suid_t;
typedef std::set<SR_uint32> terid_set_t;
typedef std::set<SR_uint32> crschanid_set_t;

typedef enum{
	e_DevMgr_sock_init = 8,
	e_DevMgr_sock_connecting = 16,
	e_DevMgr_sock_connect_ok = 32,
	e_DevMgr_sock_error = 64,
	e_DevMgr_sock_closed = 128
}E_DevSockStat;

class DevSockInfo
{
public:
	DevSockInfo(){
		m_ip.clear();
		m_port = (SR_uint16)0;
		m_devsockstat = e_DevMgr_sock_init;
		m_sockptr = (SR_void*)0;
	}
	SR_void* getSockPtr(){ return m_sockptr; }//TODO:mutex lock
	std::string   m_ip;//"192.168.1.1"��->��
	int     m_port;//������
	E_DevSockStat m_devsockstat;
	SR_void*	  m_sockptr;
};

class ThreadMessgeHandler
{
public:
	ThreadMessgeHandler(){}
	~ThreadMessgeHandler(){}
	virtual SR_void OnMessage(Message* msg) = 0;
	virtual CAsyncThread* GetSelfThread() = 0;
private:
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(ThreadMessgeHandler);
};
class TerMsgProcessThread : public ThreadMessgeHandler
{
public:
	enum tag_timerflag{
		e_detecters_timer = 0xcccc,
		e_heartbeattoters_timer = 0xbbbb,
		e_waitConfInfo_timeout_timer = 0xdddd,
		e_waitNetCreatRsp_timeout_timer = 0xeeee,
		e_waitMPCreatMPIRsp_timeout_timer = 0xeabe,
		e_waitMPCreatScrRsp_timeout_timer = 0xcabd,
		e_waitMPCreatDuoVideoScrRsp_timeout_timer = 0xabfd,
		e_waitCRSCreatConfRsp_timeout_timer = 0xabce,
		e_waitCRSStartRecRsp_timeout_timer = 0xabdc,
		e_waitCRSStopRecRsp_timeout_timer = 0xadbc,
		e_waitCRSStartLiveRsp_timeout_timer = 0xbabc,
		//e_waitCRSStopLiveRsp_timeout_timer = 0xbabd,
		e_waitRelaySvrRspTerJoinConf_timeout_timer = 0xeabc,
		e_conf_keepalive_timeout_timer = 0x1009,
		e_waitsecondvideorsp_timeout_timer = 0x0831,
		e_waitRCIreconnection_timeout_timer = 0x0832,// rollcallinitiator
		e_detecccsters_timer = 0x0438,
		e_heartbeattoccss_timer = 0x0530,
		e_sendlayouttoter_timer = 0x0531,
		e_waitLicenseInfotRsp_timeout_timer = 0x0532
	};
	TerMsgProcessThread();
	bool Init();
	virtual CAsyncThread* GetSelfThread(){ return m_selfThread; }
	virtual SR_void OnMessage(Message* msg);
	CAsyncThread* m_selfThread;
	TimerManager* m_timermanger;
	//srDES* m_pDesDec;

	//std::set<DevSockInfo*> m_confctrlsvrinfos;
	void* m_current_ccs_sockptr;
#ifdef LINUX	
	long       		m_ccs_recvheartbeart_clock;//����������ϵͳ�����ھ����ĺ�����
#else
	DWORD           m_ccs_recvheartbeart_clock;//����������ϵͳ�����ھ����ĺ����� 
#endif

	SR_char m_ccsheadcontex[HeaderContext_Size];
	std::map<SR_uint32, CompanyInfo*>m_compmaxlicences;
private:
	SR_char m_expringdata[30];// ϵͳ��Ȩʱ��
	SR_uint32 m_maxsrcount; // ϵͳ��Ȩ���SR�ն���
	SR_uint32 m_maxstdcount;// ϵͳ��Ȩ����׼�ն���
	SR_uint32 m_maxreccount;// ϵͳ��Ȩ���¼��·��
	SR_uint32 m_maxlivecount;// ϵͳ��Ȩ���ֱ��·��
	SR_uint32 m_maxternumperconf; // ϵͳ��Ȩ����������ն���
	SR_uint32 m_maxconfnum; // ϵͳ��Ȩ��������
	SR_uint32 m_maxmonitorcount;//ϵͳ��Ȩ�������
	SR_uint32 m_maxvoicecount;//ϵͳ��Ȩ��������ն���
	SR_char m_myversion[256];// �����������汾��
	SR_char m_stdsystime[256];//ϵͳ��Ȩ��׼�ն˵�ʱ��
	SR_char m_recsystime[256];//ϵͳ��Ȩ¼�Ƶ�ʱ��
	SR_char m_livesystime[256];//ϵͳ��Ȩֱ����ʱ��
	SR_char m_monitorsystime[256];//ϵͳ��Ȩ��ص�ʱ��
	SR_char m_voicesystime[256];//ϵͳ��Ȩ������ʱ��
	SR_uint32 m_cursrcount; // ��ǰϵͳSR�ն�����
	SR_uint32 m_curstdcount;// ��ǰϵͳ��׼�ն�����
	SR_uint32 m_curreccount;// ��ǰϵͳ¼����·��
	SR_uint32 m_curlivecount;// ��ǰϵͳֱ����·��
	SR_uint32 m_curmonitorcount;//��ǰϵͳ�����
	SR_uint32 m_curvoicecount;//��ǰϵͳ�����ն���
	//����״̬: uint64 confid = 1; -->��������ֻ����һ�� uint32 terid = 2; -->ÿ���ն�ֻ����һ��
	SR_bool parseAndSendTerMsgEx(SR_void* sockptr,const SR_char (&hcarr_raw)[HeaderContext_Size], SR_uint32 proto_msguid, const google::protobuf::Message* msg);
	inline MeetingRoom* getMeetRoom(SR_uint64 confid);//�жϻ���֪�����ٿ���ȡ��������

	//SR_uint32 getLayoutModeByTerNum(SR_uint32 uiTerNum);
	
	// uiMaxPaneNumֻ��ȡֵ��1~10��13��16��20��25��
	SR_uint32 getPaneNumByTerNum(SR_uint32 uiLayoutMode, SR_uint32 uiTerNum, SR_uint32 uiMaxPaneNum);
	SR_uint32 getLayoutModeByPaneNum(SR_uint32 uiPaneNum);
	//SR_uint32 getPaneNumByLayoutMode(SR_uint32 uiLayoutMode);
	//SR_uint32 getPaneNumByLayoutMode(SR_uint32 uiLayoutMode, SR_uint32 uiTerNum, SR_uint32 uiMaxPaneNum);

	// terJoinConfSuccess�Ĵ���
	// ǰ��:������֤��ȷ,�����Ѿ���ȡ�����õ�netmp
	// ����terid��channelid ��ʼ����ϯid
	//1) ���� terid <-> teruid ��Ӧ��ϵ
	//2) ֪ͨdevmgr�߳�
	//3) ֪ͨ���������������ն�
	//4) �Ƿ���Ҫ������ϯ,�жϵ�ǰ�����ն��Ƿ�Ϊ������,������ϯ֪ͨ���������ն�
	//5) �ж��Ƿ���Ҫ����p2p
	//6) ֪ͨDevMgrģ��
	//7) ֪ͨ�û���ʹ�õ�����netmp
	//8) ����m_netmpid_ternums��Ŀ-- delete
	SR_void terJoinConfSuccess(MeetingRoom* pmr, SR_uint32 netmpid, SR_uint32 mpid, SR_uint32 relatedmpichannelid, TerJoinConfReq* tjcf);
	SR_void updatevideoselectonterexit(MeetingRoom* pconftmp,SRMC::SRTer* pter);
	SR_void updatevideoselectonrepeat(MeetingRoom* pconftmp, SRMC::SRTer* pter);
	SR_void processTerRepeatJoinConf(const SRMsgs::ReqTerJoinConf* pReqJoinConf, const SRMsgs::IndTerExitConf *s, const SR_char(&pHeaderContexbuf)[HeaderContext_Size], SR_void* psockptr, SR_bool bneedrelaysvrstun);
	
	//һ��message����
	SR_void onSaveLicenceInfo(const LicenceInfoData* plid);//e_notify_to_save_licenceinfo
	SR_void onSaveCompLicenceInfo(const TotalCompLicenceInfoData* plid);//e_notify_to_save_licenceinfo
	SR_void onEndConf();
	SR_void onRegisterToDevmgr();
	SR_void onCheckConfCRSInfo();
	SR_void onRelaySockError(const SockErrorData* sed); //e_sock_error ter or relaymc
	SR_void onUpdateNetMPInfo(const NetMPInfoData* pnid);//e_update_netmpinfo_inconf
	SR_void onUpdateSystemCurLoad(const SystemCurLoadData* pscld);//e_update_sys_cur_load
	SR_void onTcpDataRecv(const SockTcpData* pstd);//e_sock_data
	SR_void onConfInfoFromDevmgr(const ConfInfoData* pcifd);//e_rspconfinfotomc
	SR_void onLicenceInfoFromDevmgr(const CompLicenceInfoData* plicd);//e_rsplicensetomc
	SR_void onNetMPCreateRsp(const NetMPCreateConfInfoData* pnccid);//e_rspnetmpcreateconf
	SR_void onMPCreateMPIRsp(const MPCreateMPIInfoData* pmpcmpiid);//e_rspmpcreatempi
	//SR_void onMPDestoryMPIRsp(const MPDestoryMPIInfoData* pmpdmpiid);//e_rspmpdestroympi	
	SR_void onMPCreateScreenRsp(const MPCreateScreenInfoData* pmpcsid);//e_rspmpcreatescreen
	//SR_void onMPDestoryScreenRsp(const MPDestoryScreenInfoData* pmdsid);//e_rspmpdestroyscreen
	SR_void onMPSeleteVideoCmd(const MPSeleteVideoCmdData* pmpsvcd);//e_cmdmpseletevideo
	SR_void onMPLayoutinfoInd(const MPLayoutinfoIndData* pmpliindd);//e_indmplayoutinfo
	SR_void onCRSCreateConfRsp(const CRSCreateConfInfoData* pcrsccid);//e_rspcrscreateconf
	SR_void onCRSStartRecRsp(const CRSStartRecInfoData* pcrssrid);//e_rspcrsstartrec
	SR_void onCRSStopRecRsp(const CRSStopRecInfoData* pcrssrid);//e_rspcrsstoprec
	SR_void onCRSSeleteVideoCmd(const CRSSeleteVideoCmdData* pcrssvcd);//e_cmdcrsseletevideo
	SR_void onCRSFileStorPathInd(const CRSFileStorPathInfoData* pcrsfspid);//e_indcrsfilestorpath
	SR_void onCRSStartLiveRsp(const CRSStartLiveInfoData* pcrsslid);//e_rspcrsstartlive
	SR_void onCRSLiveStateInd(const CRSLiveStateInfoData* pcrslsid);//e_indcrslivestate
	SR_void onGWQueryMonitorRecInd(const GWQueryMonitorRecInfoData* pgwqmrid);//e_indgwquerymonitorrec
	SR_void onNetMPIndTerStatisticsInfo(const NetMPIndTerStatisticsInfoData* pnitsid);//e_netmpindterstatisticsinfo
	SR_void onGWIndTerStatisticsInfo(const GWIndTerStatisticsInfoData* pgwitsid);//e_gwindterstatisticsinfo
	//SR_void onCRSStopLiveRsp(const CRSStopLiveInfoData* pcrsslid);//e_rspcrsstoplive
	SR_void onRelaysvrRspTerJoinConf(const RelaySvrRspTJCInfoData* prsvrrspid);//e_relaysvr_rsp_terjoinconf
	SR_void oneConfErrorTransferToSingleNetmp(MeetingRoom* pmr, terid_set_t* pterset, SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype);
	SR_uint32 haveMpWaitNetmpReopen(MeetingRoom* prm);
	SR_uint32 haveCRSWaitNetmpReopen(MeetingRoom* prm);
	SR_void onNetMPSockErrorTransferToExsitConfSameTypeNetMP(MeetingRoom* prm, terid_set_t* pterset, SR_uint32 errornetmpid, SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype);
	SR_void onNetMPSockErrorTransferToNewSameTypeNetMP(MeetingRoom* prm, terid_set_t* pterset, SR_uint32 errornetmpid, SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype, std::string errornetmpip);
	SR_void onNetMPSockErrorTransferToOtherSameTypeNetMP(MeetingRoom* prm, terid_set_t* pterset, SR_uint32 errornetmpid, SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype, std::string errornetmpip);
	SR_void onNetMPSockErrorTransferToExsitConfOtherNetMP(MeetingRoom* prm, terid_set_t* pterset, SR_uint32 errornetmpid, SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype);
	SR_void onNetMPSockErrorTransferToOtherNetMP(MeetingRoom* prm, terid_set_t* pterset, SR_uint32 errornetmpid, SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype, std::string errornetmpip);
	SR_void onNetMPSockErrorTransferToNewNetMP(MeetingRoom* prm, terid_set_t* pterset, SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype);
	SR_void onNetMPSockErrorTransferToOtherNetMP(MeetingRoom* prm, terid_set_t* pterset, SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype);
	SR_void onNetMPSockError(const NetMPSockErrorData* pnsed);
	SR_void onDevMgrSockError(const DevMgrSockErrorData* pdsed);
	SR_void onNetMPConnected(const ConnectedNetMPData* pcnd);
	SR_void onMPConnected(const ConnectedMPData* pcnd);
	SR_void onUpdateMPInfo(const MPInfoData* pnid);
	SR_void onMPSockError(const MPSockErrorData* pnsed);
	SR_void onGWConnected(const ConnectedGWData* pcnd);
	SR_void onUpdateGWInfo(const GWInfoData* pgwid);
	SR_void onGWSockError(const GWSockErrorData* pnsed);
	SR_void onCRSConnected(const ConnectedCRSData* pccrsd);
	SR_void onUpdateCRSInfo(const CRSInfoData* pcrsid);
	SR_void onCRSSockError(const CRSSockErrorData* pcrssed);
	SR_void onUpdateSRSInfo(const SRSInfoData* psrsid);

	SR_void onRelayServerConnected(const ConnectedRelayServerData* pcrsd);
	SR_void onUpdateRelayServerInfo(const RelayServerInfoData* prsid);
	SR_void onRelayServerSockError(const RelayServerSockErrorData* prssed);

	SR_void onUpdateRelayMcInfo(const RelayMcInfoData* prmid);
	SR_bool oneConfErrorTransferToNewMP(MeetingRoom* pmr, SR_uint32 errormpid, TeridNetmpidMap_t* pterset);
	SR_void onMPSockErrorTransferToExsitConfMP(MeetingRoom* pmr, SR_uint32 errormpid, TeridNetmpidMap_t* pterset);
	SR_bool onMPSockErrorTransferToNewMP(MeetingRoom* pmr, SR_uint32 errormpid, TeridNetmpidMap_t* pterset);
	SR_void onMPSockErrorTransferToOtherMP(MeetingRoom* pmr, SR_uint32 errormpid, TeridNetmpidMap_t* pterset);

	SR_bool oneConfErrorTransferToNewCRS(MeetingRoom* pmr, SR_uint32 errorcrsid/*, TeridNetmpidMap_t* pterset*/);
	SR_void onCompanyUseLicenceACK(MeetingRoom* pmr, TerJoinConfReq* s,SR_uint32 uilicencetype = 0);
	SR_uint32 onCompareComptimes(SR_uint32 compid, SR_uint32 licencetype);
	//����message���� protobuf��Ϣ����
	SR_void processReqTerJoinConf(const SRMsgs::ReqTerJoinConf* s, const SR_char(&hcarr_raw)[HeaderContext_Size], SR_void* psockptr, SR_uint32 iCurconfcnt, SR_uint32 iCurreccnt, SR_uint32 iCurlivecnt,
		SR_bool bneedrelaysvrstun, SR_bool isSimulateReq = false);
	SR_void processReqSendAssistVideo(MeetingRoom* pconftmp,SRTer* pter,const SRMsgs::ReqSendAssistVideo *s);
	//SR_void processReqMCCreateScreen(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::ReqMCCreateScreen *s);
	SR_void processReqMCCreateScreen(MeetingRoom* pconftmp, SRTer* pter, SRMsgs::ReqMCCreateScreen *s);
	SR_void processCmdMCDestoryScreen(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCDestoryScreen *s);
	SR_void processIndTerExitConf(const SRMsgs::IndTerExitConf *s);
	SR_void processIndTerExitConfEx(MeetingRoom* pconftmp,SRTer* pter,const SRMsgs::IndTerExitConf *s);
	SR_void processIndAssistVideoStart(MeetingRoom* pconftmp,SRTer* pter,const SRMsgs::IndAssistVideoStart *s);
	SR_void processIndAssistVideoClose(MeetingRoom* pconftmp,SRTer* pter,const SRMsgs::IndAssistVideoClose *s);
	SR_void processIndStdTerSendMediaFormat(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::IndStdTerSendMediaFormat *s);
	SR_void processIndStdTerRecvMediaFormat(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::IndStdTerRecvMediaFormat *s);

	SR_void processCmdMCUpdateParticipantsOrder(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCUpdateParticipantsOrder *s, SR_uint32 uisponsortype);////���²λ����ն�suid�Լ��ն�����order_no

	SR_void processCmdMCUpdateAutoPollInfo(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCUpdateAutoPollInfo *s, SR_uint32 uisponsortype);//������ѯ�б�

	SR_void processReqConfTerLists(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::ReqConfTerLists *s, SR_uint32 uisponsortype);
	SR_void processReqLayoutInfo(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::ReqLayoutInfo *s, SR_uint32 uisponsortype);

	SR_void processCmdMCAddParticipants(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCAddParticipants *s, SR_uint32 uisponsortype);
	SR_void processCmdMCDelParticipants(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCDelParticipants *s, SR_uint32 uisponsortype);
	SR_void processCmdMCInvitParticipants(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCInvitParticipants *s, SR_uint32 uisponsortype);

	SR_void processCmdSetWhitelist(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdSetWhitelist *s, SR_uint32 uisponsortype);

	//SR_void processCmdChairDropTer(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChairDropTer *s);
	SR_void processCmdChairDropTer(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChairDropTer *s, SR_uint32 uisponsortype);
	SR_void processCmdChairMuteAll(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChairMuteAll *s, SR_uint32 uisponsortype);
	SR_void processCmdChairMixSoundAll(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChairMixSoundAll *s, SR_uint32 uisponsortype);
	SR_void processCmdChairLockConf(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChairLockConf *s, SR_uint32 uisponsortype);
	SR_void processCmdChairUnLockConf(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChairUnLockConf *s, SR_uint32 uisponsortype);
	SR_void processCmdChairLockVideo(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChairLockVideo *s, SR_uint32 uisponsortype);
	SR_void processCmdChairUnLockVideo(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChairUnLockVideo *s, SR_uint32 uisponsortype);
	SR_void processCmdForceMute(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdForceMute *s, SR_uint32 uisponsortype);
	SR_void processCmdChangeConfMode(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChangeConfMode *s, SR_uint32 uisponsortype);
	SR_void processCmdSetStdTerSeeingStyle(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdSetStdTerSeeingStyle *s, SR_uint32 uisponsortype);
	SR_void processCmdSetChairman(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdSetChairman *s, SR_uint32 uisponsortype);
	SR_void processCmdTextToOtherTer(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdTextToOtherTer *s, SR_uint32 uisponsortype);
	SR_void processCmdPermissionRaiseHand(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdPermissionRaiseHand *s, SR_uint32 uisponsortype);
	SR_void processCmdMCPutAllHandDown(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCPutAllHandDown *s, SR_uint32 uisponsortype);
	SR_void processCmdMCTransferSpecialtype(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCTransferSpecialtype *s);
	SR_void processCmdMsgToOtherTer(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMsgToOtherTer *s);// ��ʱIM���ܽӿ�
	SR_void processCmdSetCallWaitingState(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdSetCallWaitingState *s, SR_uint32 uisponsortype);// ���ú��еȴ�״̬
	SR_void processReqConfGroupMeetingRoomTerLists(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::ReqConfGroupMeetingRoomTerLists *s, SR_uint32 uisponsortype);// ��ȡ�Ⱥ�������ͷֻ᳡������ն��б�
	SR_void processCmdTerJoinGroupMeetingRoom(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdTerJoinGroupMeetingRoom *s, SR_uint32 uisponsortype);// ��ȡ�Ⱥ�������ͷֻ᳡������ն��б�
	SR_void processCmdTerExitGroupMeetingRoom(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdTerExitGroupMeetingRoom *s, SR_uint32 uisponsortype);
	SR_void processIndExitGroupMeetingRoom(const SRMsgs::IndExitGroupMeetingRoom *s);
	SR_void processIndExitGroupMeetingRoomEx(MeetingRoom* pconftmp, const SRMsgs::IndExitGroupMeetingRoom *s);
	SR_void processReqConfRollCallInfo(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::ReqConfRollCallInfo *s, SR_uint32 uisponsortype);// ��ȡ������Ϣ
	SR_void processCmdMCSetConfRollCallList(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCSetConfRollCallList *s, SR_uint32 uisponsortype);// ���õ�������
	SR_void processCmdMCSetConfRollCallState(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCSetConfRollCallState *s, SR_uint32 uisponsortype);// ���õ���״̬
	SR_void processCmdMCRollCallTer(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCRollCallTer *s, SR_uint32 uisponsortype);// ����ĳ�ն�
	SR_void processCmdMCRollCallTerResult(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCRollCallTerResult *s, SR_uint32 uisponsortype);// ����ĳ�ն�	
	SR_void processCmdChangePreset(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChangePreset *s);// �ն˸���Ԥ��λ
	SR_void processCmdFilterNoVideo(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdFilterNoVideo *s, SR_uint32 uisponsortype);// ���ù�������Ƶ�ն�
	SR_void processCmdStopSendAssistVideo(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdStopSendAssistVideo *s, SR_uint32 uisponsortype);// ֪ͨ������ֹͣ����

	SR_void processCmdTerSelectAssistVideo(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdTerSelectAssistVideo *s);
	SR_void processCmdTerSelectVideo(MeetingRoom* pconftmp,SRTer* pter,const SRMsgs::CmdTerSelectVideo *s);
	//SR_void processCmdMCSetMultiScreen(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCSetMultiScreen *s);
	SR_void processCmdMCSetMultiScreen(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCSetMultiScreen *s, SR_uint32 uisponsortype);
	//SR_void processCmdMCSetMultiScreen(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCSetMultiScreen *s, SR_uint32 uisponsortype, SR_uint32 simulatescreentype);
	SR_void processCmdMCAddSubtitle(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCAddSubtitle *s, SR_uint32 uisponsortype);
	SR_void processCmdMCStopSubtitle(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCStopSubtitle *s, SR_uint32 uisponsortype);
	SR_void processCmdMCDisplayTername(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCDisplayTername *s, SR_uint32 uisponsortype);
	SR_void processCmdMCHideTername(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCHideTername *s, SR_uint32 uisponsortype);
	SR_void processCmdChairEndConf(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChairEndConf* s, SR_uint32 uisponsortype);
	SR_void processRspSendAssistVideoProxy(MeetingRoom* pconftmp,SRTer* pter,const SRMsgs::RspSendAssistVideoProxy *s);
	SR_void processCmdMCStartRec(MeetingRoom* pconftmp, SRTer* pter, SRMsgs::CmdMCStartRec *s, SR_uint32 uisponsortype);
	SR_void processCmdMCStopRec(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCStopRec *s, SR_uint32 uisponsortype);
	SR_void processCmdMCStartLive(MeetingRoom* pconftmp, SRTer* pter,SRMsgs::CmdMCStartLive *s, SR_uint32 uisponsortype);
	SR_void processCmdMCStopLive(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCStopLive *s, SR_uint32 uisponsortype);
	SR_void processIndLiveSettingToMC(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::IndLiveSettingToMC *s, SR_uint32 uisponsortype);
	SR_void processCmdMCSyncMonitorInfo(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCSyncMonitorInfo *s, SR_uint32 uisponsortype);
	SR_void processCmdMCQueryMonitorRec(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCQueryMonitorRec *s, SR_uint32 uisponsortype);
	SR_void processCmdMCPlaybackMonitorRec(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdMCPlaybackMonitorRec *s, SR_uint32 uisponsortype);
	SR_void processIndTerPlaybackMonitorRecStateToMC(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::IndTerPlaybackMonitorRecStateToMC *s);
	SR_void OnCmdMCStartLive(MeetingRoom* pconftmp, const SRMsgs::CmdMCStartLive *s);
	SR_void OnCmdMCStartRec(MeetingRoom* pconftmp, const SRMsgs::CmdMCStartRec *s);
	SR_void processCmdMPSelectVideo(SR_uint64 confid, SR_uint32 mpichannelid, const MPSeleteVideoCmdData* pmpsvcd);
	SR_void processIndMPLayoutInfo(SR_uint64 confid, SR_uint32 mpichannelid, const MPLayoutinfoIndData* pmpliindd);
	//SR_void processCmdCRSSeleteVideo(SR_uint64 confid, SR_uint32 crschannelid, const CRSSeleteVideoCmdData* pcrssvcd);
	SR_void processCmdCRSSeleteVideo(SR_uint64 confid, SR_uint32 crschannelid, CRSSeleteVideoCmd* pcrssvcd);
	SR_void  processNetMPErrorTerExitConf(const SRMsgs::IndTerExitConf *s,SR_uint32 erronetmpid);
	SR_void processIndNewAudioSourceJoined(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::IndNewAudioSourceJoined *s);
	SR_void processIndAudioSourceLeave(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::IndAudioSourceLeave *s);
	SR_void processIndNewVideoSourceJoined(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::IndNewVideoSourceJoined *s);
	SR_void processIndVideoSourceLeave(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::IndVideoSourceLeave *s);
	SR_void processCmdChangeVideoSourceName(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChangeVideoSourceName *s);
	SR_void processCmdChangeVideoSourcePriority(MeetingRoom* pconftmp, SRTer* pter, const SRMsgs::CmdChangeVideoSourcePriority* s);
	
	SR_void  startP2P(MeetingRoom* pmr);
	SR_void  isWaitConfInfoTimeout(SR_uint64 confid);
	SR_void* createTimer(SR_uint32 millisecond, SR_int32 timertype, SR_uint32 timerflag, SR_uint64 timerdata);
	SR_void* createDispoableTimer(SR_uint64 millisecond, SR_uint32 timerflag, SR_uint64 timerdata);
	SR_bool deleteTimer(SR_void* timerid);
	SR_void  detecTersHeartBeat();
	SR_void  sendHeartBeatToTers();
	SR_void  syncConfinfoToDevmgr();
	SR_void  sendHeartBeatToDevmgr();
	SR_void* m_sendLayoutToTerTime;
	SR_void* m_periodtimer_detecTersHeart;
	SR_void* m_periodtimer_sendHeartBeat;
	SR_void* m_periodtimer_detecCCSHeart;
	SR_void* m_periodtimer_sendHeartBeatToCCS;
	SR_void sendLayoutToTers(MeetingRoom* pconftmp);
	SR_void  detecCCSsHeartBeat();
	SR_void  sendHeartBeatToCCSs();
	std::map<SR_uint64,MeetingRoom*> m_rooms;//confid ->:key,û�и�key��ʾ�û��鲻����
	std::map< SR_uint64,std::set<SR_uint32> > m_wait_reopen_ters;//�ȴ�reopen����
	//std::map< SR_uint64, TeridNetmpidMap_t > m_wait_mp_reopen_ters;//�������Ǩ�Ƶȴ�reopen mp���ն˶���
	std::map<SR_uint32,ConnectedNetMPInfo*> m_connectednetmpinfos;// netmpid ->:key
	std::map<SR_uint32, ConnectedMPInfo*> m_connectedmpinfos;// mpid ->:key
	std::map<SR_uint32, ConnectedGWInfo*> m_connectedgwinfos;// gwid ->:key
	std::map<SR_uint32, ConnectedCRSInfo*> m_connectedcrsinfos;// crsid ->:key
	std::map<SR_uint32, ConnectedSRSInfo*> m_connectedsrsinfos;// srsid ->:key
	std::map<SR_uint32, ConnectedRelaySvrInfo*> m_connectedrelaysvrinfos;// relayserverid ->:key
	std::map<SR_uint32, ConnectedRelayMcInfo*> m_connectedrelaymcinfos;// relaymcid ->:key
	std::map<std::string, SR_uint32> m_connectnetmpiplevels;
	std::map<std::string, SR_uint32> m_connectnetmpipandportlevels;
	SR_uint32 getLightestNetMPByTermGroupid(SR_uint32 tergroupid);
	SR_uint32 getLightestNetMPByTermGroupidList(std::list<SR_uint32> &tergroupidlist);
	SR_bool CheckIPInRealymcMapips(SR_uint32 tergroupid, std::string& strterip);
	SR_uint32 getLightestRealymcidByTerip(SR_uint32 tergroupid, std::string& strterip);
	SR_bool CheckRealymcHaveMapips(SR_uint32 relaymcid);
	SR_uint32 CheckRealymcHaveAgeMapips(SR_uint32 relaymcid);
	SR_uint32 getLightestNetMPByTermGroupidandIP(SR_uint32 tergroupid, std::string& strterip);
	SR_uint32 getLightestHaveMapipsNetMPByTermGroupidandRelaymcid(MeetingRoom* pmr,SR_uint32 tergroupid, SR_uint32 relaymcid);
	SR_uint32 getLightestNetMPByTermGroupidset(std::map<SR_uint32, SR_uint32> &tergroupidmap);
	SR_uint32 getLightestNetMP(SR_uint32 currenterrornetmpid = 0);
	SR_uint32 getLightestNetMPExcludeConf(SR_uint64 confid);// �ų��ڸû������Ѿ����������netmp
	SR_void netmpInfOnTerExitConf(SR_uint32 netmpid,SR_uint64 confid,SR_uint32 terid);
	SR_uint32 getTerNumsOnNet(SR_uint32 netmpid);
	SR_uint32 getTerNumsOnConfidNetMP(SR_uint32 netmpid,SR_uint64 confid);

	SR_uint32 getTerNumsOnMP(SR_uint32 mpid);
	SR_uint32 getTerNumsOnConfidMP(SR_uint32 mpid, SR_uint64 confid);

	SR_void netmpInfoOnConfExit(SR_uint64 confid);
	SR_void mpInfoOnConfExit(SR_uint64 confid);
	SR_void mpInfoOnTerExitConf(SR_uint32 mpid, SR_uint64 confid, SR_uint32 terid);
	//���ǰ���� m_connectednetmpinfos ��Ϣ(���ϻ�ɹ������)
	SR_void removeWaitNetmpRspter(SR_uint32 netmpid, SR_uint64 confid, SR_uint32 suid);//��suid�ն˴�m_connectednetmpinfos����ĵȴ��������Ƴ���
	SR_void emptyWaitRspSet(SR_uint32 netmpid,SR_uint64 confid);//��� m_connectednetmpinfos����ĵȴ�����
	SR_void emptyWaitReopen(SR_uint32 netmpid,SR_uint64 confid);
	SR_uint32 getTerJoniWaitNetMPRsp(SR_uint64 confid);//���ػ���ȴ�netmp �ĵ�һ��netmpid
	SR_uint32 chooseMapipsNewNetMP(SR_uint32 errornetmpid, SR_uint32 errornetmpgroupid);//ѡ��һ���е�netmp(������ӳ���ַ�б�),����û���ٿ��κλ���
	SR_uint32 chooseMapipsSameTypeNewNetMP(SR_uint32 errornetmpid, SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype);//ѡ��һ���е���ͬnettype��netmp(������ӳ���ַ�б�),����û���ٿ��κλ���
	SR_uint32 chooseNonMapipsNewNetMP(SR_uint32 errornetmpid, SR_uint32 errornetmpgroupid);//ѡ��һ���е�netmp(û������ӳ���ַ�б�),����û���ٿ��κλ���
	SR_uint32 chooseNonMapipsSameTypeNewNetMP(SR_uint32 errornetmpid, SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype, std::string errornetmpip);//ѡ��һ���е���ͬnettype��netmp(û������ӳ���ַ�б�),����û���ٿ��κλ���
	SR_uint32 chooseNewNetMP(SR_uint32 errornetmpgroupid);//ѡ��һ���е�netmp,����û���ٿ��κλ���
	SR_uint32 chooseNewSameTypeNetMP(SR_uint32 errornetmpgroupid, SR_uint32 errornetmpnettype); //ѡ��һ���е���ͬnettype��netmp, ����û���ٿ��κλ���
	SR_uint32 chooseSameGroupMinipLevelNewNetMP(MeetingRoom* pmr, SR_uint32 tergroupid, SR_uint32 bTerRelatedRelaymcHaveAgeMapips, SR_uint32 ter_related_relaymcid, std::set<std::string> confnetmpips);
	SR_void* getNetMPsocketptr(SR_uint32 netmpid);
	SR_bool isConfEmpty(SR_uint64 confid);//�û����Ƿ��ڿ���״̬,����û���ն�(�������ڼ��������ն�),���鲻���ڷ���true

	SR_uint32 getLightestMPExcludeConf(SR_uint64 confid, SR_uint32 currenterrormpid = 0);// �ų��ڸû������Ѿ����������mp
	SR_uint32 chooseNewMP(SR_uint32 currenterrormpid = 0);//ѡ��һ���е�mp,����û���ٿ��κλ���
	SR_void* getMPsocketptr(SR_uint32 mpid);
	SR_void ClearWaitMPRspSet(SR_uint32 mpid, SR_uint64 confid);//��� m_connectednetmpinfos����ĵȴ�����

	SR_uint32 getLightestGW(SR_uint32 currenterrorgwid = 0);
	//SR_uint32 chooseNewGW(SR_uint32 currenterrorgwid = 0);//ѡ��һ���е�gw,����û���ٿ��κλ���
	SR_void* getGWsocketptr(SR_uint32 gwid);

	//SR_uint32 getLightestCRS(SR_uint32 currenterrorcrsid = 0);
	SR_uint32 getLightestCRSExcludeConf(SR_uint64 confid, SR_uint32 currenterrorcrsid = 0);// �ų��ڸû������Ѿ����������crs
	SR_void* getCRSsocketptr(SR_uint32 crsid);

	SR_uint32 getLightestSRS(SR_uint32 currenterrorsrsid = 0);

	SR_uint32 getLightestRelaySvr(SR_uint32 currenterrorrelaysvrid = 0);
	SR_void* getRelaySvrsocketptr(SR_uint32 relaysvrid);
	SR_void removeWaitRelayserverRspter(SR_uint32 relaysvrid, SR_uint64 confid, SR_uint32 suid);//��suid�ն˴�m_connectedrelaysvrinfos����ĵȴ��������Ƴ���

	SR_void SaveTerJoinConfReq(const SRMsgs::ReqTerJoinConf* s, const SR_char(&hcarr_raw)[HeaderContext_Size], SR_void* psockptr, std::list<TerJoinConfReq*> *savelist, SR_uint32 savetype, SR_uint32 netmpid, SR_uint32 mpid, SR_uint32 relaysvrid, SR_bool bneedrelaysvrstun, SR_uint32 groupid);
	
	SR_uint32  CheckCRSState(SR_uint64 confid);
	SR_uint32  CheckCRSChannelState(SR_uint64 confid, SR_uint32 crschannelid);

	SR_void SyncGWMixScreenToRecAndLiveScreen(MeetingRoom* pmr, SR_bool bdefault = false);
	// ����CRSΪĳ���鴴����Ӧ��ͨ��
	SR_void ReqCRSCreateConfChannel(MeetingRoom* pmr, SR_uint32 choosenetmpid, SR_bool bstartrec = false, SR_uint32 bstartlive = false);
};

//#define NETMP_SOCK_STATE_INITE      (2)
//#define NETMP_SOCK_STATE_CONNECTING (4)
//#define NETMP_SOCK_STATE_CONNECTED  (8)
//#define NETMP_SOCK_STATE_CLOSED     (16)

typedef enum{
	e_Netmp_sock_inite = 0x1,
	e_Netmp_sock_connecting = 0x2,
	e_Netmp_sock_connect_ok = 0x3,
	e_Netmp_sock_closed = 0x4,
}E_NetMPSockStat;

typedef enum{
	e_Mp_sock_inite = 0x1,
	e_Mp_sock_connecting = 0x2,
	e_Mp_sock_connect_ok = 0x3,
	e_Mp_sock_closed = 0x4,
}E_MPSockStat;

typedef enum{
	e_GW_sock_inite = 0x1,
	e_GW_sock_connecting = 0x2,
	e_GW_sock_connect_ok = 0x3,
	e_GW_sock_closed = 0x4,
}E_GWSockStat;

typedef enum{
	e_dev_sock_inite = 0x1,
	e_dev_sock_connecting = 0x2,
	e_dev_sock_connect_ok = 0x3,
	e_dev_sock_closed = 0x4,
}E_DEVSockStat;

typedef enum{
	e_Seeing_Style_FollowCtrller = 0x0,
	e_Seeing_Style_OnlyChairman = 0x1,
	e_Seeing_Style_MixScreen = 0x2
}E_STDTERSEESINGSTYLE;

typedef enum{
	e_Conf_Mode_Free = 0x0,
	e_Conf_Mode_ChairMan = 0x1
}E_ConfMode;

typedef enum{
	e_Layout_Type_Transfer = 0x0,
	e_Layout_Type_Mixed = 0x1
}E_LayoutType;

typedef enum{
	e_Layout_Mode_Auto = 0,
	e_Layout_Mode_1 = 1,
	e_Layout_Mode_2 = 2,
	e_Layout_Mode_3 = 3,
	e_Layout_Mode_4 = 4,
	e_Layout_Mode_5 = 5,
	e_Layout_Mode_6 = 6,
	e_Layout_Mode_7 = 7,
	e_Layout_Mode_8 = 8,
	e_Layout_Mode_9 = 9,
	e_Layout_Mode_10 = 10,
	e_Layout_Mode_13 = 11,
	e_Layout_Mode_16 = 12,
	e_Layout_Mode_REC = 13,
	e_Layout_Mode_20 = 14,
	e_Layout_Mode_25 = 15,
	e_Layout_Mode_Speech_4_1 = 16, // �ݽ���ģʽ4+1����
	e_Layout_Mode_PIP_RD = 17 // ���л�,Сͼ�������²���,PIP_RightDown��PIP_LeftDown��PIP_LeftUp��PIP_RightUp
}E_LayoutMode;

typedef enum{
	e_Licence_Type_CONF = 0x1,//����
	e_Licence_Type_SR = 0x2,//SR�ͻ���
	e_Licence_Type_STD = 0x3,//������׼�ն˵�TER,MCU��΢�ţ�STREAM_LIVE��RTSP��,TRUNK��sip trunk)
	e_Licence_Type_MONITOR = 0x4,//�������ʵʱ��,���¼��
	e_Licence_Type_PSTN = 0x5,//�����ն�
	e_Licence_Type_LIVE = 0x6,//ֱ��
	e_Licence_Type_REC = 0x7,//¼��
	e_Licence_Type_ANALY = 0x8,//����ʶ��
	e_Licence_Type_ASR = 0x9//����ʶ��
}E_LicenceType;

typedef enum{
	e_Screen_Type_Mixed = 0x1,
	e_Screen_Type_Transcode = 0x2,
	e_Screen_Type_Duovideo = 0x3,
	e_Screen_Type_RecAndLive = 0x4,
	e_Screen_Type_ChairLock = 0x5
}E_ScreenType;

//typedef enum{
//	e_Screen_State_Init = 0x0,
//	e_Screen_State_OK = 0x1,
//	e_Screen_State_Transfer = 0x2,
//	e_Screen_State_Failover = 0x3
//}E_ScreenState;

typedef enum{
	e_Creat_Sponsor_TER = 0x1,
	e_Creat_Sponsor_WEB = 0x2,
	e_Creat_Sponsor_MC = 0x3
}E_CreatSponsorType;

//typedef enum{
//	e_Term_Type_MPI = 0x0,
//	e_Term_Type_Mobile = 0x1,
//	e_Term_Type_PC = 0x2,
//	e_Term_Type_TV = 0x3
//}E_TermType;

//typedef enum{
//	e_Audio_Fmt_G711U = 0x0,
//	e_Audio_Fmt_G711A = 0x1,
//	e_Audio_Fmt_G722 = 0x2,
//	e_Audio_Fmt_G7221 = 0x3,
//	e_Audio_Fmt_G723 = 0x4,
//	e_Audio_Fmt_G728 = 0x5,
//	e_Audio_Fmt_G729 = 0x6,
//	e_Audio_Fmt_G729A = 0x7,
//	e_Audio_Fmt_AAC_LC = 0x8,
//	e_Audio_Fmt_AAC_LD = 0x9,
//	e_Audio_Fmt_OPUS = 0xA
//}E_AUDIO_FMT;
//
//typedef enum{
//	e_Audio_Sampl_Freq_8000 = 8000,
//	e_Audio_Sampl_Freq_11025 = 11025,
//	e_Audio_Sampl_Freq_12000 = 12000,
//	e_Audio_Sampl_Freq_16000 = 16000,
//	e_Audio_Sampl_Freq_22050 = 22050,
//	e_Audio_Sampl_Freq_24000 = 24000,
//	e_Audio_Sampl_Freq_32000 = 32000,
//	e_Audio_Sampl_Freq_44100 = 44100,
//	e_Audio_Sampl_Freq_48000 = 48000,
//	e_Audio_Sampl_Freq_64000 = 64000,
//	e_Audio_Sampl_Freq_88200 = 88200,
//	e_Audio_Sampl_Freq_96000 = 96000
//}E_AUDIO_SAMPL_FREQ;
//
//typedef enum{
//	e_Audio_Channel_Num_1 = 0x1,
//	e_Audio_Channel_Num_2 = 0x2
//}E_AUDIO_CHAN_NUM;
//
//typedef enum{
//	e_Video_Fmt_H261 = 0x0,
//	e_Video_Fmt_H263 = 0x1,
//	e_Video_Fmt_H263p = 0x2,
//	e_Video_Fmt_H264 = 0x3,
//	e_Video_Fmt_H265 = 0x4
//}E_VIDEO_FMT;
//
typedef enum{
	e_Payload_Type_0 = 0, // G711U
	e_Payload_Type_3 = 3, // GSM
	e_Payload_Type_4 = 4, // G723
	e_Payload_Type_8 = 8, // G711A
	e_Payload_Type_9 = 9, // G722
	e_Payload_Type_15 = 15, // G728
	e_Payload_Type_18 = 18, // G729
	e_Payload_Type_31 = 31, // H261
	e_Payload_Type_34 = 34, // H263
	e_Payload_Type_SR_OPUS_96 = 96, // suirui OPUS
	e_Payload_Type_SR_H264_98 = 98, // suirui H264
	e_Payload_Type_SR_AAC_LC_106 = 106, // suirui AAC_LC
	e_Payload_Type_SR_H265_108 = 108, // suirui H265
}E_PAYLOAD_TYPE;

// �ն����ͱ�������ܹ�16bit, 0x0000����
// ��15bit--��8bit��8λ��������ϵͳ,0x01~0xff,��0x01~0x3f��Windows ��0x40~0x7f��linux��0x80~0x8f��ƻ����0x90~0x9f����׿��
// 0x01~0x3f��Windows
//             -- 0x01:Win 10��0x02:Win 8��0x03:Win 7��0x04:Windows vista��0x05:Windows xp��(windows 2003/2000��windows server 2016/2012/2008/2005��Windows Mobile��Windows Phone������ϵͳ�������õ�ʱ�ټӶ���)
// 0x40~0x7f��linux
//             -- 0x40:Ubuntu��0x41:Centos��0x42:Redhat��0x43:Kylin��0x44:RedFlag��(SUSE ��OpenSUSE��Fedora��Debian��Redhat Enterprise Linux������ϵͳ�������õ�ʱ�ټӶ���)
// 0x80~0x8f��ƻ��
//             -- 0x80:Mac OS��0x81:iOS
// 0x90~0x9f����׿
//             -- 0x90:Android������պ���Ҫ���ְ�׿�汾�����Խ���Χ������BF����CF�����Դ˴�����������һ�η�����չ��
// ��7bit--��4bit�м�4λ����CPU�ܹ�,0x0~0xf,��0x0��UNKNOW��0x1��X86��0x2��ARM��(0x3��MIPS��0x4��PowerPC����ʱδʹ�ø�Ӳ��)......0xe��ARM���䡢0xf��X86���䣻
// ��3bit--��0bit��4λ������Ʒ��̬,0x0~0xf,��0x0��UNKNOW��0x1���ƶ��ˣ��ֻ���ƽ�壩��0x2��PC��0x3��TV��

// ������
//      0x0112��ʾWin 10����ϵͳ��x86�ܹ�cpu����Ʒ��̬��pc��ʽ
//      0x4013��ʾUbuntu����ϵͳ��x86�ܹ�cpu����Ʒ��̬��TV��ʽ---��Ӧ��˾��ƷUbox
//      0x4322��ʾ����Keylin����ϵͳ��ARM�ܹ�cpu����Ʒ��̬��pc��ʽ
//      0x8121��ʾƻ��ios����ϵͳ��ARM�ܹ�cpu����Ʒ��̬���ƶ�����ʽ
//      0x9023��ʾ��׿����ϵͳ��ARM�ܹ�cpu����Ʒ��̬��TV��ʽ---��Ӧ��˾��Ʒ3288
typedef enum{
	e_TermType_UNKNOW = 0x0000, // δ֪,����
	e_TermType_UNKNOW_Mobile = 0x0001, // ����ϵͳ�ʹ�����δ֪,�ƶ���
	e_TermType_UNKNOW_PC = 0x0002, // ����ϵͳ�ʹ�����δ֪,PC
	e_TermType_UNKNOW_TV = 0x0003, // ����ϵͳ�ʹ�����δ֪,TV

	e_TermType_Win10_x86_Mobile = 0x0111, // Win10����ϵͳ,x86������,�ƶ���
	e_TermType_Win10_x86_PC = 0x0112, // Win10����ϵͳ,x86������,PC	
	e_TermType_Win10_x86_TV = 0x0113, // Win10����ϵͳ,x86������,TV

	e_TermType_Win8_x86_Mobile = 0x0211, // Win8����ϵͳ,x86������,�ƶ���
	e_TermType_Win8_x86_PC = 0x0212, // Win8����ϵͳ,x86������,PC	
	e_TermType_Win8_x86_TV = 0x0213, // Win8����ϵͳ,x86������,TV

	e_TermType_Win7_x86_Mobile = 0x0311, // Win7����ϵͳ,x86������,�ƶ���
	e_TermType_Win7_x86_PC = 0x0312, // Win7����ϵͳ,x86������,PC	
	e_TermType_Win7_x86_TV = 0x0313, // Win7����ϵͳ,x86������,TV

	e_TermType_WindowsXP_x86_Mobile = 0x0511, // WindowsXP����ϵͳ,x86������,�ƶ���
	e_TermType_WindowsXP_x86_PC = 0x0512, // WindowsXP����ϵͳ,x86������,PC	
	e_TermType_WindowsXP_x86_TV = 0x0513, // WindowsXP����ϵͳ,x86������,TV

	e_TermType_Ubuntu_x86_Mobile = 0x4011, // Ubuntu����ϵͳ,x86������,�ƶ���
	e_TermType_Ubuntu_x86_PC = 0x4012, // Ubuntu����ϵͳ,x86������,PC	
	e_TermType_Ubuntu_x86_TV = 0x4013, // Ubuntu����ϵͳ,x86������,TV---��Ӧ��˾��Ʒ������Ubox
	e_TermType_Ubuntu_x86_TV_LOW = 0x40f3, // Ubuntu����ϵͳ,����x86������,TV---��Ӧ��˾��Ʒ������Ĵ�Ubox

	e_TermType_Centos_x86_Mobile = 0x4111, // Centos����ϵͳ,x86������,�ƶ���
	e_TermType_Centos_x86_PC = 0x4112, // Centos����ϵͳ,x86������,PC	
	e_TermType_Centos_x86_TV = 0x4113, // Centos����ϵͳ,x86������,TV

	e_TermType_Kylin_ARM_Mobile = 0x4321, // Kylin����ϵͳ,ARM������,�ƶ���
	e_TermType_Kylin_ARM_PC = 0x4322, // Kylin����ϵͳ,ARM������,PC	
	e_TermType_Kylin_ARM_TV = 0x4323, // Kylin����ϵͳ,ARM������,TV

	e_TermType_MacOS_x86_PC = 0x8012, // Mac OS����ϵͳ,x86������,PC
	e_TermType_MacOS_ARM_PC = 0x8022, // Mac OS����ϵͳ,ARM������,PC
	e_TermType_IOS_ARM_Mobile = 0x8121, // ios����ϵͳ,ARM������,�ƶ���

	e_TermType_Android_ARM_Mobile = 0x9021, // Android����ϵͳ,ARM������,�ƶ���
	e_TermType_Android_ARM_PC = 0x9022, // Android����ϵͳ,ARM������,PC	
	e_TermType_Android_ARM_TV = 0x9023, // Android����ϵͳ,ARM������,TV---��Ӧ��˾��Ʒ3288
}E_TermType;

// ��׼�ն����ͱ�������ܹ�16bit
// ��15bit--��8bit��8λ�����Խ�Э������,0-�Զ���1-sip��2-H.323��3-GB28181��4-ΪPSTNԤ����5-RTSP
// ��7bit--��0bit��8λ������Ʒ��̬,0-�նˡ�1-MCU��2-���ʵʱ����3-���¼��4-PSTN��5-΢����ҳ�ͻ��ˡ�6-��ý��ʵʱ(ֱ��)����7-TRUNK
typedef enum{
	e_StdTermType_AUTO_TER = 0x0000, // Э���Զ�,�ն�
	e_StdTermType_AUTO_MCU = 0x0001, // Э���Զ�,MCU
	e_StdTermType_AUTO_MONITOR_LIVE = 0x0002, // Э���Զ�,���ʵʱ��
	e_StdTermType_AUTO_MONITOR_REC = 0x0003, // Э���Զ�,���¼��
	e_StdTermType_AUTO_PSTN = 0x0004, // Э���Զ�,95013��������PSTN����
	e_StdTermType_AUTO_VX = 0x0005, // Э���Զ�,΢����ҳ�ͻ���
	e_StdTermType_AUTO_STREAM_LIVE = 0x0006, // Э���Զ�,��ý��ʵʱ(ֱ��)��
	e_StdTermType_AUTO_TRUNK = 0x0007, // Э���Զ�,TRUNK

	e_StdTermType_SIP_TER = 0x0100, // SIPЭ��,�ն�
	e_StdTermType_SIP_MCU = 0x0101, // SIPЭ��,MCU
	e_StdTermType_SIP_PSTN = 0x0104, // SIPЭ��,PSTN
	e_StdTermType_SIP_TRUNK = 0x0107, // SIPЭ��,TRUNK

	e_StdTermType_H323_TER = 0x0200, // H323Э��,�ն�
	e_StdTermType_H323_MCU = 0x0201, // H323Э��,MCU
	e_StdTermType_H323_PSTN = 0x0204, // H323Э��,PSTN
	e_StdTermType_H323_TRUNK = 0x0207, // H323Э��,TRUNK

	e_StdTermType_GB28181_MONITOR_LIVE = 0x0302, // GB28181Э��,���ʵʱ��
	e_StdTermType_GB28181_MONITOR_REC = 0x0303, // GB28181Э��,���¼��
	e_StdTermType_RTSP_STREAM_LIVE = 0x0506, // RTSPЭ��,��ý��ʵʱ(ֱ��)��
}E_StdTermType;
// ��Ʒ�ͺţ��ܹ�32bit
// ��31bit--��16bit��16λ�������ұ���,0-δ֪,0x0001-SR HUIJIAN��������0x0002-SR ZOOM������Ŀ��0x0003-Polycom����ͨ��0x0004-huawei��Ϊ��0x0005-Cisco˼�ơ�0x0006-KEDA�ƴ0x0007-ZHONGXING���ˡ�0x0008-Yealink����
// ��15bit--��0bit��16λ������Ʒ�ͺ�,0-δ֪�������Բ�Ʒͳһ����Ϊ׼
typedef enum{
	e_ProductType_UNKNOW = 0x00000000, // ����δ֪,�ͺ�δ֪
	e_ProductType_SR_HJ = 0x00010000, // SR HUIJIAN������,�ͺ�δ֪
	e_ProductType_SR_HJ_3A = 0x00010001, // SR HUIJIAN������,3A
	e_ProductType_SR_HJ_3C_PRO = 0x00010002, // SR HUIJIAN������,3C PRO
	e_ProductType_SR_HJ_4A = 0x00010003, // SR HUIJIAN������,4A
	e_ProductType_SR_HJ_4C = 0x00010004, // SR HUIJIAN������,4C
	e_ProductType_SR_HJ_5E_PRO = 0x00010005, // SR HUIJIAN������,5E PRO
	e_ProductType_SR_HJ_6A = 0x00010006, // SR HUIJIAN������,6A
	e_ProductType_SR_HJ_6E = 0x00010007, // SR HUIJIAN������,6E
	e_ProductType_SR_HJ_7A = 0x00010008, // SR HUIJIAN������,7A

	e_ProductType_SR_ZM = 0x00020000, // SR ZOOM������Ŀ,�ͺ�δ֪

	e_ProductType_Polycom = 0x00030000, // Polycom����ͨ,�ͺ�δ֪
	e_ProductType_Polycom_RPD = 0x00030001, // Polycom����ͨ,RPD
	e_ProductType_Polycom_PVX = 0x00030002, // Polycom����ͨ,PVX

	e_ProductType_HUAWEI = 0x00040000, // huawei��Ϊ,�ͺ�δ֪
	e_ProductType_Cisco = 0x00050000, // Cisco˼��,�ͺ�δ֪
	e_ProductType_KEDA = 0x00060000, // KEDA�ƴ�,�ͺ�δ֪
	e_ProductType_ZHONGXING = 0x00070000, // ZHONGXING����,�ͺ�δ֪
	e_ProductType_Yealink = 0x00080000, // Yealink����,�ͺ�δ֪
}E_ProductType;

typedef enum{
	e_Term_From_Platform = 0x0, // ����ƽ̨(��������RP�ն�)
	e_Term_From_Gateway = 0x1 // ��������(H.323�նˡ�SIP�ն�)
}E_TermFromType;

typedef enum{
	e_UserType_ANON = 0x0, // ��ʱ(����)�û�
	e_UserType_REGD = 0x1, // ��ʽ(ע��)�û�
	e_UserType_UBOX = 0x2, // U-box�û�
	e_UserType_STDTER = 0x3, // ��׼�ն�(H.323/sip)
	e_UserType_MONITOR_REALTIME = 0x4, // 4-���ǰ��ʵʱ��
	e_UserType_NON_LOCALDOMAIN = 0x5, // 5-�Ǳ����û�
	e_UserType_3288 = 0x6, // 6-3288�ն�
	e_UserType_MONITOR_PLAYBACK = 0x7, // 7-���ǰ��¼��
	e_UserType_VX_WEBRTC = 0x8 // 9-΢��webrtc��ҳ�ͻ���
}E_UserType;

typedef enum{
	e_SpecialType_NORM = 0x0, // ��ͨ�ն�����
	e_SpecialType_SHARE = 0x1, // Ͷ���ն�����
}E_SpecialType;

typedef enum{
	e_conf_state_ended = 0x0,// ��ʼ(����)״̬
	e_conf_state_starting = 0x1,
	e_conf_state_waitnetmp = 0x2,
	e_conf_state_waitmp = 0x3,
	e_conf_state_started = 0x4,// �����Ѿ��ٿ�
	e_conf_state_ending = 0x5// �������ڽ���
}E_ConfStat;

typedef enum{
	e_gmrtype_main = 0x0,// ���᳡����
	e_gmrtype_waitingroom = 0x1,// �Ⱥ�������
	e_gmrtype_venue = 0x2// �ֻ᳡����
}E_GroupMeetingRoomType;
typedef enum{
	e_conf_state_duovideo = 0x1,//����
}E_ConfStatusType;
typedef enum{
	e_sub_state_avpause = 0x0,//�����Ƿ���ͣ(����������ǰ����)
	e_sub_state_avlabel = 0x1,// ������ע(����������ǰ����)
	e_sub_state_avchairman = 0x2//ֻ�������˿��Է�����
}E_SubStatusType;
class TerInfo
{
public:
	TerInfo()
	{
		m_domainname.clear();
		m_tersuid = 0;
		m_tername.clear();
		m_termtype = e_TermType_UNKNOW;
		m_fromtype = e_Term_From_Platform;
		m_producttype = 0;
	}

	~TerInfo()
	{
		m_domainname.clear();
		m_tersuid = 0;
		m_tername.clear();
		m_termtype = e_TermType_UNKNOW;
		m_fromtype = e_Term_From_Platform;
		m_producttype = 0;
	}

	std::string m_domainname;//�ն�����������
	SR_uint32 	m_tersuid;//�ն˵�ϵͳ�û�id
	std::string m_tername;//�ն�����
	SR_uint32 	m_termtype;//�ն����ͣ�16bit�������ֵ.fromtypeΪ0ʱ,8bit os + 4bit cpu + 4bit��Ʒ��̬;fromtypeΪ1ʱ,8bit Э������ + 8bit��Ʒ��̬;
	SR_uint32 	m_fromtype;//�ն��������0-��������ƽ̨(����������RP�ն�)��1-������������(��H.323�նˡ�SIP�նˡ����)
	SR_uint32   m_producttype;//��Ʒ�ͺ�.32bit�������ֵ.16bit ���ұ��� + 16bit��Ʒ�ͺű���
};

class GroupMeetingRoomInfo
{
public:
	GroupMeetingRoomInfo()
	{
		m_gmrdomainname.clear();
		m_gmrconfid = 0;
		m_gmrid = 0;
		m_gmrtype = 0;
		m_gmrname.clear();
	}

	~GroupMeetingRoomInfo()
	{
		m_gmrdomainname.clear();
		m_gmrconfid = 0;
		m_gmrid = 0;
		m_gmrtype = 0;
		m_gmrname.clear();
	}

	std::string m_gmrdomainname;// �������������Ӧ������
	SR_uint64   m_gmrconfid;// ���������Ļ����
	SR_uint32 	m_gmrid;//����ķ����,0-Ĭ��ֵ,��������顢1-�Ⱥ������顢2++��Ӧ�ֻ᳡�����
	SR_uint32	m_gmrtype;//�����������,0-Ĭ��ֵ,��������顢1-�Ⱥ������顢2-�ֻ᳡����
	std::string m_gmrname;//�������ķ�����,���᳡�ķ������ǻ�������
};

typedef std::set<SR_uint32> TeridSet_t;
class NetMPInfo
{
public:
	NetMPInfo()
	{
		m_netmpdeviceid = 0;
		m_ip = 0;
		m_port = 0;
		m_load = 0;
		m_load2 = 0;
		m_groupid = 0;
		m_connectionstate = e_Netmp_sock_inite;
		m_mapinternetips.clear();
		m_nettype = 0;
		m_max_bandwidth = 0;
		m_max_terms = 0;
	}
	~NetMPInfo()
	{
		m_netmpdeviceid = 0;
		m_ip = 0;
		m_port = 0;
		m_load = 0;
		m_load2 = 0;
		m_groupid = 0;
		m_connectionstate = e_Netmp_sock_inite;
		m_mapinternetips.clear();
		m_nettype = 0;
		m_max_bandwidth = 0;
		m_max_terms = 0;
	}
	SR_uint32   m_netmpdeviceid;
	SR_uint32   m_ip;// netmp��������ַ
	SR_uint32   m_port;// netmp������port
	SR_uint32 	m_load;// netmp�Ĵ���
	SR_uint32 	m_load2;// netmp�ϵ��ն˸���
	SR_uint32 	m_groupid;// netmp���������id
	SR_uint32   m_connectionstate;// sock����״̬
	std::list<std::string> m_mapinternetips;
	SR_uint32   m_nettype;// �������ָ�netmp�ǲ������ַ�ʽע���
	SR_uint32   m_max_bandwidth;//netmp�ɳ��ص�������
	SR_uint32   m_max_terms;//netmp�ɳ��ص�����ն���
};

class MPInfo
{
public:
	MPInfo(){ m_connectionstate = e_Mp_sock_inite; }
	SR_uint32   m_mpdeviceid;
	SR_uint32   m_ip;// mp��������ַ
	SR_uint32   m_port;// mp������port
	SR_uint32 	m_load;// mp:���д�����netmp->mp��
	SR_uint32 	m_load2;// mp:cpu����
	SR_uint32   m_connectionstate;// sock����״̬
};

class GWInfo
{
public:
	GWInfo(){ m_connectionstate = e_GW_sock_inite; }
	SR_uint32   m_gwdeviceid;
	SR_uint32   m_ip;// gw��������ַ
	SR_uint32   m_port;// gw������port
	SR_uint32 	m_load;// gw:�ն���
	SR_uint32 	m_load2;// gw:cpu����
	SR_uint32   m_connectionstate;// sock����״̬
};

class CRSInfo
{
public:
	CRSInfo(){ m_connectionstate = e_dev_sock_inite; }
	SR_uint32   m_crsdeviceid;
	SR_uint32   m_ip;// crs��������ַ
	SR_uint32   m_port;// crs������port
	SR_uint32 	m_load;// crs:���̿ռ�ʹ����,����16λ��������,��16λ��ʣ������,��λ��GB��
	SR_uint32 	m_load2;// crs:cpuʹ����
	SR_uint32   m_connectionstate;// sock����״̬
	SR_uint32 	m_disktotalcapacity;// ����������
	SR_uint32 	m_diskremaincapacity;// ����ʣ������
};

class SRSInfo
{
public:
	SRSInfo()
	{ 
	}
	SR_uint32   m_srsdeviceid;
	SR_uint32   m_ip;// srs��������ַ
	SR_uint32   m_port;// srs������port
	SR_uint32 	m_load;// srs:�Ƿ��Ǹ��ڵ�,0-��,1-��
	SR_uint32 	m_load2;// srs:��ʱ����ע
	SR_uint32   m_connectionstate;// sock����״̬
	time_t m_srsdevtime;
	std::string m_strip;
};

class RelayServerInfo
{
public:
	RelayServerInfo(){ m_connectionstate = e_dev_sock_inite; }
	SR_uint32   m_relaysvrdevid;
	SR_uint32   m_ip;// relayserver��������ַ
	SR_uint32   m_port;// relayserver������port
	SR_uint32 	m_ternum;// relayserver:�����ն���
	SR_uint32 	m_tx;// relayserver:����(����)����
	SR_uint32   m_connectionstate;// sock����״̬
};

class RelayMcInfo
{
public:
	RelayMcInfo()
	{
		m_relaymcdevid = 0;
		m_ip = 0;
		m_port = 0;
		m_ternum = 0;
		m_groupid = 0;
		m_mapinternetips.clear();
	}
	~RelayMcInfo()
	{
		m_relaymcdevid = 0;
		m_ip = 0;
		m_port = 0;
		m_ternum = 0;
		m_groupid = 0;
		m_mapinternetips.clear();
	}
	SR_uint32   m_relaymcdevid;
	SR_uint32   m_ip;// relaymc��������ַ
	SR_uint32   m_port;// relaymc������port
	SR_uint32 	m_ternum;// relaymc:�����ն���
	SR_uint32 	m_groupid;// relaymc���������id
	//SR_uint32   m_connectionstate;// sock����״̬
	time_t m_relaymcdevtime;
	std::list<std::string> m_mapinternetips;
};
//typedef enum{
//	e_DevMgr_sock_init = 8,
//	e_DevMgr_sock_connecting = 16,
//	e_DevMgr_sock_connect_ok = 32,
//	e_DevMgr_sock_error = 64,
//	e_DevMgr_sock_closed = 128
//}E_DevSockStat;

class SubtitleInfo
{
public:
	SubtitleInfo()
	{
		m_subtitlemsg.clear();
		m_displaytime = 0;
		m_msgpos = 0;
		m_msgcolor.clear();
		m_msgbgcolor.clear();
		m_msgfontsize = 0;
		m_enablemsgbg = true;
		m_enablemsgroll = true;
		m_rollspeed = 0;
	}

	~SubtitleInfo()
	{
		m_subtitlemsg.clear();
		m_displaytime = 0;
		m_msgpos = 0;
		m_msgcolor.clear();
		m_msgbgcolor.clear();
		m_msgfontsize = 0;
		m_enablemsgbg = true;
		m_enablemsgroll = true;
		m_rollspeed = 0;
	}
	
	std::string m_subtitlemsg;//������Ļ����	
	SR_uint32 	m_displaytime;//��Ļ����ʱ�䣬��λ���룬����Ϊ0����������ʾ
	SR_uint32	m_msgpos;//������Ļ��λ�ã�1-�ϡ�2-�С�3-��;�᳡����λ�ã�1-���ϡ�2-���ϡ�3-���ϡ�4-���¡�5-���¡�6-����
	std::string m_msgcolor;//��Ϣ������ɫ:RGB��ɫֵ,6λʮ������ֵ,��:��ɫ #000000����ɫ #FFFFFF����ɫ #FF0000����ɫ #00FF00����ɫ #0000FF����ɫ #FFFF00
	std::string m_msgbgcolor;//���屳����ɫ:RGB��ɫֵ,6λʮ������ֵ,��:��ɫ #000000����ɫ #FFFFFF����ɫ #FF0000����ɫ #00FF00����ɫ #0000FF����ɫ #FFFF00
	SR_uint32 	m_msgfontsize;//�����С��1-��2-�С�3-С
	SR_bool 	m_enablemsgbg;//ʹ�ܹ�����Ļ����
	SR_bool 	m_enablemsgroll;//��Ļ�Ƿ������ʾ:true-������ʾ(Ĭ��)��false-�̶���ʾ
	SR_uint32 	m_rollspeed;//enablemsgrollΪtrueʱ�Ĺ����ٶȣ�1-������2-һ��(Ĭ��)��3-�Կ졢4-����

};

class ScreenPaneInfo
{
public:
	//ScreenPaneInfo();
	//~ScreenPaneInfo();

	ScreenPaneInfo()
	{
		m_paneindex = 0u;
		m_contenttype = 0u;
		m_optype = 1u; // ���� 0:�Զ�;����Ĭ��ֵ1:ָ���ն�;
		m_polltime = 10u;
		m_terid = 0u;
		m_terchannelid = 0u;
		m_tername.clear();
		m_autopollinfo.clear();
		m_vsrcid = -1;
	}

	~ScreenPaneInfo()
	{
		m_paneindex = 0u;
		m_contenttype = 0u;
		m_optype = 1u; // ���� 0:�Զ�;����Ĭ��ֵ1:ָ���ն�;
		m_polltime = 10u;
		m_terid = 0u;
		m_terchannelid = 0u;
		m_tername.clear();
		for (std::map<SR_uint32, Autopollinfo*>::iterator spinfo_itor = m_autopollinfo.begin();
			spinfo_itor != m_autopollinfo.end(); spinfo_itor++)
		{
			Autopollinfo* autopollinfo = NULL;
			autopollinfo = spinfo_itor->second;
			if (autopollinfo != NULL)
			{
				delete autopollinfo;
				autopollinfo = NULL;
			}
		}
		m_autopollinfo.clear();
		m_vsrcid = -1;		
	}

	SR_uint32 m_paneindex;//����id
	SR_uint32 m_contenttype;//��������ʾ�������ͣ�0:�ն�; 1:voipλͼ; 2:��; 3:˫��
	SR_uint32 m_optype;//�����в������ͣ�1:ָ���ն�; 2:��������;3:�Զ���Ѳ.
	SR_uint32 m_polltime;//��������е���ʾ����Ϊ��3:�Զ���Ѳ������˴������Ϊ��ѯʱ��������λ����
	SR_uint32 m_terid;//�������������ն˶�Ӧ��id 
	SR_uint32 m_terchannelid;//�������������ն˶�Ӧ��ͨ��id
	std::string m_tername;//�������������ն˶�Ӧ������
	std::map<SR_uint32, Autopollinfo*> m_autopollinfo;//�Զ���ѯ�б���Ϣ
	SR_int32  m_vsrcid; // sint32���ͱ�int32�ڱ��븺��ʱЧ�ʸ���,�ն���ƵԴ���,-1-û������ͷ,0-0������ͷ��1- 1������ͷ��2-2������ͷ������31-31������ͷ,Ŀǰ���֧��32������ͷ
};

class ConfScreenInfo
{
public:
	//ConfScreenInfo();
	//~ConfScreenInfo();

	ConfScreenInfo()
	{
		//m_screenstate = e_Screen_State_Init;

		m_textpos = 0u;
		m_textFontsize = 0u;
		m_textcolor.assign("#000000"); // ��ɫ
		m_enableTextBg = false;
		m_textBgColor.assign("#FFFFFF"); // ��ɫ

		m_enableTername = false;

		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_creatsponsortype = e_Creat_Sponsor_TER;
		m_reqterid = 0u;
		m_terreqseqnum = 0u;
		m_mcreqseqnum = 0u;
		m_chairfollow = 1;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_relatednetmpid = 0u;
		m_voiceActiveInd = false;
		m_voiceActiveColor.assign("#FFFF00"); // ��ɫ
		m_chairFeccColor.assign("#FF0000"); // ��ɫ

		m_isok = false;
		m_failreason.clear();

		m_select_video.clear();
		m_be_selected.clear();
		m_last_send_level_count.clear();
		m_last_recvfps = 0u;

		m_isErrorTransferScreen = false;
		m_uiTransferFromMPid = 0u;

		m_layouttype = e_Layout_Type_Transfer;
		m_layoutmode = e_Layout_Mode_Auto;
		m_curpanenum = e_Layout_Mode_1;

		for (SR_uint32 i = 0; i <MAX_PANNE_INDEX; i++)
		{
			if (((ScreenPaneInfo*)m_screenpaneinfo[i]) != NULL)
			{
				delete (((ScreenPaneInfo*)m_screenpaneinfo[i]));
			}

			m_screenpaneinfo[i] = NULL;
			ScreenPaneInfo* pSPinfo = new ScreenPaneInfo();
			pSPinfo->m_paneindex = i;//����id
			pSPinfo->m_contenttype = 0u;//��������ʾ�������ͣ�0:�ն�; 1:voipλͼ; 2:��; 3:˫��
			pSPinfo->m_optype = 1;//�����в������ͣ�0:�Զ�; 1:ָ���ն�; 2:��������;3:�Զ���Ѳ.  // ���� 0:�Զ�;����Ĭ��ֵ1:ָ���ն�;
			pSPinfo->m_polltime = 10;//��������е���ʾ����Ϊ��3:�Զ���Ѳ������˴������Ϊ��ѯʱ��������λ����
			pSPinfo->m_terid = 0;//�������������ն˶�Ӧ��id 
			pSPinfo->m_terchannelid = 0;//�������������ն˶�Ӧ��ͨ��id
			pSPinfo->m_tername.clear();//�������������ն˶�Ӧ������
			pSPinfo->m_vsrcid = -1;

			m_screenpaneinfo[i] = pSPinfo;
		}

		//m_screenpaneinfo.clear();

		//m_isNetMpErrorTransfer = false;
		//m_uiTransferFromNetMPid = 0u;

		m_bWaitNetmpReopen = false;
		m_steplength = 1;//������ϯģʽ��ת��������ϯ�Ĳ���
		m_confpolllist.clear();
	}

	~ConfScreenInfo()
	{
		//m_screenstate = e_Screen_State_Init;

		m_textpos = 0u;
		m_textFontsize = 0u;
		m_textcolor.assign("#000000"); // ��ɫ
		m_enableTextBg = false;
		m_textBgColor.assign("#FFFFFF"); // ��ɫ

		m_enableTername = false;

		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_creatsponsortype = e_Creat_Sponsor_TER;
		m_reqterid = 0u;
		m_terreqseqnum = 0u;
		m_mcreqseqnum = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_relatednetmpid = 0u;
		m_voiceActiveInd = false;
		m_voiceActiveColor.assign("#FFFF00"); // ��ɫ
		m_chairFeccColor.assign("#FF0000"); // ��ɫ

		m_isok = false;
		m_failreason.clear();

		m_select_video.clear();
		m_be_selected.clear();
		m_last_send_level_count.clear();
		m_last_recvfps = 0u;

		m_isErrorTransferScreen = false;
		m_uiTransferFromMPid = 0u;

		m_layouttype = e_Layout_Type_Transfer;
		m_layoutmode = e_Layout_Mode_Auto;
		m_curpanenum = e_Layout_Mode_1;
		m_chairfollow = 1;

		for (std::map<SR_uint32, ScreenPaneInfo*>::iterator spinfo_itor = m_screenpaneinfo.begin();
			spinfo_itor != m_screenpaneinfo.end(); spinfo_itor++)
		{
			ScreenPaneInfo* pSPinfo = NULL;
			pSPinfo = spinfo_itor->second;
			if (pSPinfo != NULL)
			{
				delete pSPinfo;
				pSPinfo = NULL;
			}
		}

		m_screenpaneinfo.clear();

		//m_isNetMpErrorTransfer = false;
		//m_uiTransferFromNetMPid = 0u;

		m_bWaitNetmpReopen = false;
		m_steplength = 1;//������ϯģʽ��ת��������ϯ�Ĳ���
		m_confpolllist.clear();
	}

	//SR_uint32 m_screenstate; //��Ļ״̬


	SR_uint32	m_textpos;//�ն������ı�λ�ã�1-���ϡ�2-���ϡ�3-���ϡ�4-���¡�5-���¡�6-����
	std::string	m_textcolor;//�ı���ɫ:RGB��ɫֵ,6λʮ������ֵ,��:��ɫ #000000����ɫ #FFFFFF����ɫ #FF0000����ɫ #00FF00����ɫ #0000FF����ɫ #FFFF00
	SR_uint32	m_textFontsize;//�ı������С��1-��2-�С�3-С
	SR_bool		m_enableTextBg;//ʹ���ı�����
	std::string	m_textBgColor;//�ı�������ɫ:RGB��ɫֵ,6λʮ������ֵ,��:��ɫ #000000����ɫ #FFFFFF����ɫ #FF0000����ɫ #00FF00����ɫ #0000FF����ɫ #FFFF00

	SR_bool m_enableTername; //ʹ�ܻ᳡����

	SR_uint64 m_confid;
	SR_uint32 m_mcid;//���������mc_deviceid
	SR_uint32 m_mpid;//������Ϣ��mp_deviceid
	SR_uint32 m_creatsponsortype; //������������ 
	SR_uint32 m_reqterid;//���������terid, 0-web��mc����0-�ն�terid
	SR_uint32 m_terreqseqnum;//�ն˻���web������������кţ�����mc����������0
	SR_uint32 m_mcreqseqnum;//������������кţ��ɷ��������MCά��
	SR_uint32 m_channelid;//������ĳ����ý�崦��ʵ��ͨ��channelid�ϴ�����Ļ
	SR_uint32 m_screenid;//���󴴽�����Ļ��screenid
	SR_uint32 m_screentype;//���󴴽�����Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ��4-¼��ֱ����Ļ
	SR_uint32 m_relatednetmpid;//���󴴽�����Ļ������netmpid
	SR_bool	m_voiceActiveInd;//���󴴽���Ļ�����ԣ����Լ���ָʾ
	std::string m_voiceActiveColor;//���󴴽���Ļ�����ԣ����Լ�����ɫ:RGB��ɫֵ,6λʮ������ֵ,��:��ɫ #000000����ɫ #FFFFFF����ɫ #FF0000����ɫ #00FF00����ɫ #0000FF����ɫ #FFFF00
	std::string m_chairFeccColor;//���󴴽���Ļ�����ԣ�Զ�̿���ָʾ��ɫ:RGB��ɫֵ,6λʮ������ֵ,��:��ɫ #000000����ɫ #FFFFFF����ɫ #FF0000����ɫ #00FF00����ɫ #0000FF����ɫ #FFFF00

	SR_bool m_isok;
	std::string m_failreason;//��ʧ�ܣ������ʧ��˵��

	//�������� �ն�ѡ����Ƶ,�����ն��˻����
	std::map<SR_uint32, SR_uint32> m_select_video;//terid <-> videosize �������һ��ѡ�������������ն�����
	std::map<SR_uint32, SR_uint32> m_be_selected;//terid <-> videosize
	std::map<SR_uint32, SR_uint32> m_last_send_level_count;//videosize <-> count
	SR_uint32 m_last_recvfps; //��һ�ν���֡�� 0:0 1:8 2:16 3:24 4:30 5:60.����ն�/��Ļѡ��û���ѡ�ǿ��ʹ��Ĭ��3:24

	SR_bool m_isErrorTransferScreen;
	SR_uint32 m_uiTransferFromMPid;//���ĸ�MP��Ǩ�ƹ�����mp_deviceid
	SR_uint32 m_uiTransferFromNetMPid;//���ĸ�NetMP��Ǩ�ƹ�����netmp_deviceid

	SR_uint32 m_layouttype;//�����������ͣ�0-ת�����֡�1-��������
	SR_uint32 m_layoutmode;//��Ļ����ģʽ��0-�Զ���1-������2-2������3-3������4-4����������11-13������12-16������13-¼������14-20������15-25������16-�ݽ���ģʽ4+1���֡�17-���л�,Сͼ�������²���,PIP_RightDown
	std::map<SR_uint32, ScreenPaneInfo*> m_screenpaneinfo; // key:��0��ʼ�����24
	SR_uint32 m_curpanenum;// ��ǰ����ģʽʵ�ʷ�������
	SR_uint32  m_steplength;//������ϯģʽ��ת��������ϯ�Ĳ���
	std::list<SR_uint32> m_confpolllist;// ��ϯģʽ��ת�����֣���ѯ�б�
	std::list<SR_uint32> m_confterlist;//��ϯģʽ��ת������,�ų�ָ���ն˺����е���ѯ�ն�
	SR_uint32  m_chairfollow;
	//SR_bool m_isNetMpErrorTransfer;
	//SR_uint32 m_uiTransferFromNetMPid;//���ĸ�NETMP��Ǩ�ƹ�����netmp_deviceid

	SR_bool m_bWaitNetmpReopen; // �Ի���Ϊ��λ��ͬһ�����飩��mpi���䵽һ��netmp��

private:

};

class ConfMPIInfo
{
public:
	ConfMPIInfo()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
		m_relatednetmpid = 0u;

		m_failreason.clear();
		m_select_video.clear();
		m_be_selected.clear();

		m_bWaitNetmpReopen = false;
		m_uiTransferFromNetMpid = 0u;

		m_bNeedErrorTransfer = false;
		m_isErrorTransferMPI = false;
		m_uiTransferFromMPid = 0u;
	}
	~ConfMPIInfo()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
		m_relatednetmpid = 0u;

		m_failreason.clear();
		m_select_video.clear();
		m_be_selected.clear();

		m_bWaitNetmpReopen = false;
		m_uiTransferFromNetMpid = 0u;

		m_bNeedErrorTransfer = false;
		m_isErrorTransferMPI = false;
		m_uiTransferFromMPid = 0u;
	}

	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_mpid;
	SR_uint32		 m_channelid; //MCΪ����ý�崦��ʵ�������ͨ��id
	SR_uint32		 m_relatednetmpid;
	std::string 	 m_failreason;

	//�������� �ն�ѡ����Ƶ,�����ն��˻����
	std::map<SR_uint32, SR_uint32> m_select_video;//terid <-> videosize �������һ��ѡ�������������ն����Σ����汻mpiѡ�����նˣ���mpi����ƵԴ
	std::map<SR_uint32, SR_uint32> m_be_selected;//terid <-> videosize ����ѡ����mpi���նˣ���mpi��Ŀ�Ķ�

	SR_bool         m_bWaitNetmpReopen; // �Ի���Ϊ��λ��ͬһ�����飩��mpi���䵽һ��netmp��
	SR_uint32		m_uiTransferFromNetMpid;//���ĸ�NetMp��Ǩ�ƹ�����netmp_deviceid

	SR_bool			m_bNeedErrorTransfer;
	SR_bool			m_isErrorTransferMPI;
	SR_uint32		m_uiTransferFromMPid;//���ĸ�MP��Ǩ�ƹ�����mp_deviceid

private:

};
class SubStatusInfo
{
public:
	SubStatusInfo()
	{
		m_substatustype = 0u;
		m_substattus = 0u;
		m_assistvideotimes = 0u;
	}
	~SubStatusInfo()
	{
		m_substatustype = 0u;
		m_substattus = 0u;
		m_assistvideotimes = 0u;
	}
	SR_uint32 m_substatustype;//�����й��ܵ��ӹ�������,0-�����Ƿ���ͣ(����������ǰ����)��1-������ע(����������ǰ����)��2-ֻ�������˿��Է�����
	SR_uint32 m_substattus;//�����й��ܵ��ӹ��ܶ�Ӧ״̬��0-�رա�1-������substatustype==2ʱ��0-���������˶����Է�������1-ֻ�������˿��Է�������
	SR_uint32 m_assistvideotimes;//�����й���ʱ�䣬��λ���룻Ŀǰֻ֧��substatustype==2ʱʹ��
};
class ConfStatusInfo
{
public:
	ConfStatusInfo()
	{
		m_statustype = 0u;
		m_stattus = 0u;
		m_substatusinfos.clear();
	}
	~ConfStatusInfo()
	{
		m_statustype = 0u;
		m_stattus = 0u;
		for (std::map<SR_uint32, SubStatusInfo*>::iterator substatus_itor = m_substatusinfos.begin();
			substatus_itor != m_substatusinfos.end(); substatus_itor++)
		{
			SubStatusInfo* substatusinfos = NULL;
			substatusinfos = substatus_itor->second;
			if (substatusinfos != NULL)
			{
				delete substatusinfos;
				substatusinfos = NULL;
			}
		}
		m_substatusinfos.clear();
	}
	SR_uint32 m_statustype;//�����й��ܵ�����,0-δ֪��1-˫����������
	SR_uint32 m_stattus;//�����й��ܵ�״̬��0-�رա�1-����
	std::map<SR_uint32, SubStatusInfo*> m_substatusinfos;//�����й��ܵ��ӹ���
};
class RecordFileInfo
{
public:
	RecordFileInfo()
	{
		m_crschannelid = 0;
		//m_recordid = 0;
		//m_recordfileid = 0;
		m_filestorsvrip.clear();
		m_filestorpath.clear();
		m_sdefilepath.clear();
		m_relativepath.clear();
	}
	~RecordFileInfo()
	{
		m_crschannelid = 0;
		//m_recordid = 0;
		//m_recordfileid = 0;
		m_filestorsvrip.clear();
		m_filestorpath.clear();
		m_sdefilepath.clear();
		m_relativepath.clear();
	}
	SR_uint32		 m_crschannelid;// ��¼���ļ������ĸ�crsͨ��
	//SR_uint32		 m_recordid;// mc�Լ�ά����	
	//SR_uint32		 m_recordfileid;// mc�Լ�ά����
	std::string      m_filestorsvrip;//CRS¼���ļ���ŷ����ip
	std::string 	 m_filestorpath;//CRS¼���ļ�������·��
	std::string 	 m_sdefilepath;//CRS¼�Ƶļ����ļ�������·��
	std::string 	 m_relativepath;//CRS¼���ļ���Ÿ�·��

private:

};

class RecChannelInfo
{
public:
	RecChannelInfo()
	{
		m_recchannelid = 0;
		m_recscreenid = 0;
		m_recscreentype = 0;
	}
	~RecChannelInfo()
	{
		m_recchannelid = 0;
		m_recscreenid = 0;
		m_recscreentype = 0;
	}
	SR_uint32		 m_recchannelid;
	SR_uint32		 m_recscreenid;
	SR_uint32		 m_recscreentype;
	//SR_uint32		 m_relatedcrsid;
	//SR_uint32		 m_relatedcrschannelid;

private:

};


class ConfCRSInfo
{
public:
	ConfCRSInfo()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_relatednetmpid = 0u;
		m_relatedrecchannelid = 0u;
		m_relatedrecscreenid = 0u;
		m_relatedrecscreentype = 0u;

		m_relatedlivechannelid = 0u;
		m_relatedlivescreenid = 0u;
		m_relatedlivescreentype = 0u;

		m_failreason.clear();
		m_select_video.clear();
		m_be_selected.clear();

		//m_recvaudiofmt = G711U; //�ն˽�����Ƶ��ʽ
		//m_recvaudiofrequency = kNbInHz; //�ն˽�����Ƶ����Ƶ��
		//m_recvaudiochannelnum = MonoChannel; //�ն˽�����Ƶͨ����
		//m_recvaudioframelen = 160; //�ն˽�����Ƶ֡��
		//m_recvaudiopt = 0; //�ն˽�����Ƶ��������payload type
		//m_recvvideofmt = Video_H264_SVC; //�ն˽�������Ƶ��ʽ
		//m_recvvideopt = 0; //�ն˽�������Ƶ��������payload type
		//m_recvduovideofmt = Video_H264_SVC; //�ն˽��ո�����Ƶ��ʽ
		//m_recvduovideopt = 0; //�ն˽��ո�����������payload type

		m_recvaudiofmt = OPUS_48; //CRS������Ƶ��ʽ
		m_recvaudiofrequency = kFbInHz; //CRS������Ƶ����Ƶ��
		m_recvaudiochannelnum = MonoChannel; //CRS������Ƶͨ����
		m_recvaudioframelen = 160; //CRS������Ƶ֡��
		m_recvaudiopt = 0; //CRS������Ƶ��������payload type
		m_recvvideofmt = Video_H264_SVC; //�ն˽�������Ƶ��ʽ
		m_recvvideopt = 0; //CRS��������Ƶ��������payload type
		m_recvduovideofmt = Video_H264_SVC; //�ն˽��ո�����Ƶ��ʽ
		m_recvduovideopt = 0; //CRS���ո�����������payload type
		m_recvvideobandwidth = 0; //CRS������������

		m_bWaitNetmpReopen = false;
		m_bWaitMPReopen = false;
		//m_uiTransferFromNetMpid = 0u;

		//m_bNeedErrorTransfer = false;
		m_isErrorTransferCrsChan = false;
		m_isStartRec = false;
		m_isStartLive = false;
		//m_uiTransferFromMPid = 0u;

		m_reqnumandreason.clear();
	}
	~ConfCRSInfo()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_relatednetmpid = 0u;

		m_relatedrecchannelid = 0u;
		m_relatedrecscreenid = 0u;
		m_relatedrecscreentype = 0u;
		
		m_relatedlivechannelid = 0u;
		m_relatedlivescreenid = 0u;
		m_relatedlivescreentype = 0u;

		m_failreason.clear();
		m_select_video.clear();
		m_be_selected.clear();

		//m_recvaudiofmt = G711U; //CRS������Ƶ��ʽ
		//m_recvaudiofrequency = kNbInHz; //CRS������Ƶ����Ƶ��
		//m_recvaudiochannelnum = MonoChannel; //CRS������Ƶͨ����
		//m_recvaudioframelen = 160; //CRS������Ƶ֡��
		//m_recvaudiopt = 0; //CRS������Ƶ��������payload type
		//m_recvvideofmt = Video_H264_SVC; //CRS��������Ƶ��ʽ
		//m_recvvideopt = 0; //CRS��������Ƶ��������payload type
		//m_recvduovideofmt = Video_H264_SVC; //CRS���ո�����Ƶ��ʽ
		//m_recvduovideopt = 0; //CRS���ո�����������payload type

		m_recvaudiofmt = OPUS_48; //CRS������Ƶ��ʽ
		m_recvaudiofrequency = kFbInHz; //CRS������Ƶ����Ƶ��
		m_recvaudiochannelnum = MonoChannel; //CRS������Ƶͨ����
		m_recvaudioframelen = 160; //CRS������Ƶ֡��
		m_recvaudiopt = 0; //CRS������Ƶ��������payload type
		m_recvvideofmt = Video_H264_SVC; //�ն˽�������Ƶ��ʽ
		m_recvvideopt = 0; //CRS��������Ƶ��������payload type
		m_recvduovideofmt = Video_H264_SVC; //�ն˽��ո�����Ƶ��ʽ
		m_recvduovideopt = 0; //CRS���ո�����������payload type
		m_recvvideobandwidth = 0; //CRS������������

		m_bWaitNetmpReopen = false;
		m_bWaitMPReopen = false;
		//m_uiTransferFromNetMpid = 0u;

		//m_bNeedErrorTransfer = false;
		m_isErrorTransferCrsChan = false;
		m_isStartRec = false;
		m_isStartLive = false;
		//m_uiTransferFromMPid = 0u;

		m_reqnumandreason.clear();

		//for (std::map<SR_uint32, std::map<SR_uint32, RecordFileInfo*>>::iterator recinfo_itor = m_recordinfo.begin();
		//	recinfo_itor != m_recordinfo.end(); recinfo_itor++)
		//{
		//	for (std::map<SR_uint32, RecordFileInfo*>::iterator recfile_itor = recinfo_itor->second.begin();
		//		recfile_itor != recinfo_itor->second->end(); recfile_itor++)
		//	{
		//		RecordFileInfo* pRecordFileInfo = NULL;
		//		pRecordFileInfo = recfile_itor->second;
		//		if (pRecordFileInfo != NULL)
		//		{
		//			delete pRecordFileInfo;
		//			pRecordFileInfo = NULL;
		//		}
		//	}
		//	recinfo_itor->second->clear();
		//}
	}

	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_crsid;
	SR_uint32		 m_crschannelid; //MCΪCRS�����ͨ��id
	SR_uint32		 m_relatednetmpid;
	std::map<SR_uint32, RecChannelInfo*> m_recinfo;//��ͨ��¼�ƶ�· key:recchannelid <--> value:RecChannelInfo*
	SR_uint32		 m_relatedrecchannelid;// ¼�Ƶ�MPI��Ӧͨ��id�����ն˶�Ӧ��ͨ��id
	SR_uint32		 m_relatedrecscreenid;// ¼�Ƶ���Ļid,���m_relatedchannelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_relatedrecscreentype;// ¼�Ƶ���Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ��4-¼��ֱ����Ļ,���m_relatedchannelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_relatedlivechannelid;// ֱ����MPI��Ӧͨ��id�����ն˶�Ӧ��ͨ��id
	SR_uint32		 m_relatedlivescreenid;// ֱ������Ļid,���m_relatedlivechannelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_relatedlivescreentype;// ֱ������Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ��4-¼��ֱ����Ļ,���m_relatedlivechannelid���ն�ͨ��id,��ѡ����Ժ���
	std::string 	 m_failreason;

	SR_uint32 m_recvaudiofmt; //CRS������Ƶ��ʽ
	SR_uint32 m_recvaudiofrequency; //CRS������Ƶ����Ƶ��
	SR_uint32 m_recvaudiochannelnum; //CRS������Ƶͨ����
	SR_uint32 m_recvaudioframelen; //CRS������Ƶ֡��
	SR_uint32 m_recvaudiopt; //CRS������Ƶ��������payload type
	SR_uint32 m_recvvideofmt; //CRS��������Ƶ��ʽ
	SR_uint32 m_recvvideopt; //CRS��������Ƶ��������payload type
	SR_uint32 m_recvduovideofmt; //CRS���ո�����Ƶ��ʽ
	SR_uint32 m_recvduovideopt; //CRS���ո�����������payload type
	SR_uint32 m_recvvideobandwidth; //CRS������������

	//�������� �ն�ѡ����Ƶ,�����ն��˻����
	std::map<SR_uint32, SR_uint32> m_select_video;//terid <-> videosize �������һ��ѡ�������������ն����Σ����汻mpiѡ�����նˣ���mpi����ƵԴ
	std::map<SR_uint32, SR_uint32> m_be_selected;//terid <-> videosize ����ѡ����mpi���նˣ���mpi��Ŀ�Ķ�

	SR_bool         m_bWaitNetmpReopen; // �Ի���Ϊ��λ��ͬһ�����飩��crs���䵽һ��netmp��
	SR_bool         m_bWaitMPReopen;
	//SR_uint32		m_uiTransferFromNetMpid;//���ĸ�NetMp��Ǩ�ƹ�����netmp_deviceid

	//SR_bool			m_bNeedErrorTransfer;
	SR_bool			m_isErrorTransferCrsChan;
	SR_bool         m_isStartRec;
	SR_bool         m_isStartLive;
	//SR_uint32		m_uiTransferFromMPid;//���ĸ�MP��Ǩ�ƹ�����mp_deviceid

	std::map<SR_uint32, SR_uint32> m_reqnumandreason; // reason�� 0-ҳ����ֹͣ¼��,1-������������ֹͣ¼��,2-����Ǩ������ֹͣ¼��,3-����Ϊ������ֹͣ¼��

	//std::map<SR_uint32, std::map<SR_uint32, RecordFileInfo*>> m_recordinfo; // ��¼��ͨ�������쳣¼�Ƽ�¼ recordid <-->(recordfileid<-->RecordFileInfo)

private:

};

class ConfRecInfo
{
public:
	ConfRecInfo()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_relatedcrsid = 0u;
		m_relatedcrschannelid = 0u;

		m_recchannelid = 0u;
		m_recscreenid = 0u;
		m_recscreentype = 0u;
		m_failreason.clear();

		m_isErrorTransferRec = false;

		m_reqnumandreason.clear();
		m_confrecord_id = 0ull;
		m_recordfile_id = 0ull;
	}
	~ConfRecInfo()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_relatedcrsid = 0u;
		m_relatedcrschannelid = 0u;

		m_recchannelid = 0u;
		m_recscreenid = 0u;
		m_recscreentype = 0u;
		m_failreason.clear();

		m_isErrorTransferRec = false;

		m_reqnumandreason.clear();
		m_confrecord_id = 0ull;
		m_recordfile_id = 0ull;
	}

	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_relatedcrsid;
	SR_uint32		 m_relatedcrschannelid; //MCΪCRS�����ͨ��id

	SR_uint32		 m_recchannelid;// ¼�Ƶ�ͨ��(MPI��Ӧͨ��id�����ն˶�Ӧ��ͨ��id)
	SR_uint32		 m_recscreenid;// ¼�Ƶ���Ļid,���m_recchannelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_recscreentype;// ¼�Ƶ���Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ��4-¼��ֱ����Ļ,���m_recchannelid���ն�ͨ��id,��ѡ����Ժ���
	std::string 	 m_failreason;
	
	SR_bool			m_isErrorTransferRec;

	std::map<SR_uint32, SR_uint32> m_reqnumandreason; // reason�� 0-ҳ����ֹͣ¼��,1-������������ֹͣ¼��,2-����Ǩ������ֹͣ¼��,3-����Ϊ������ֹͣ¼��

	SR_uint64        m_confrecord_id;// ��·¼��ͨ���Ʒ�id
	SR_uint64        m_recordfile_id;// ��·¼��ͨ��¼���ļ��Ʒ�id

private:

};

class LiveSetting
{
public:
	LiveSetting()
	{
		m_chairman.clear();
		m_subject.clear();
		m_abstract.clear();
		m_ispublic = 0u;
		m_livepwd.clear();
		m_isuserec = 1;
	}
	~LiveSetting()
	{
		m_chairman.clear();
		m_subject.clear();
		m_abstract.clear();
		m_ispublic = 0u;
		m_livepwd.clear();
		m_isuserec = 1;
	}

	std::string m_chairman;//ֱ��������
	std::string m_subject;//ֱ������
	std::string m_abstract;//ֱ��ժҪ/���
	SR_uint32   m_ispublic;//�Ƿ񹫿�,0-������,1-����
	std::string m_livepwd;//ֱ������
	SR_uint32   m_isuserec;//�Ƿ�����¼��,0-������¼��,1-����¼�ƣ�Ĭ�ϣ�
};

class ConfLiveInfo
{
public:
	ConfLiveInfo()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_relatedsrsid = 0u;
		m_livechannelid = 0u;
		m_livescreenid = 0u;
		m_livescreentype = 0u;
		m_livepushurl.clear();
		m_livepullurl.clear();
		m_liveplayurl.clear();
		m_failreason.clear();
		m_livesvrtype = 0u;
		m_liveaddrs.clear();

		m_isErrorTransferLive = false;
		m_liveinfo_id = 0ull;
	}
	~ConfLiveInfo()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_relatedsrsid = 0u;
		m_livechannelid = 0u;
		m_livescreenid = 0u;
		m_livescreentype = 0u;
		m_livepushurl.clear();
		m_livepullurl.clear();
		m_liveplayurl.clear();
		m_failreason.clear();
		m_livesvrtype = 0u;
		m_liveaddrs.clear();

		m_isErrorTransferLive = false;
		m_liveinfo_id = 0ull;
	}

	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_crsid;
	SR_uint32		 m_crschannelid; //MCΪCRS�����ͨ��id
	SR_uint32		 m_relatedsrsid;
	SR_uint32		 m_livechannelid;// ֱ����MPI��Ӧͨ��id�����ն˶�Ӧ��ͨ��id
	SR_uint32		 m_livescreenid;// ֱ������Ļid,���m_relatedchannelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_livescreentype;// ֱ������Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ��4-¼��ֱ����Ļ,���m_relatedchannelid���ն�ͨ��id,��ѡ����Ժ���
	std::string		 m_livepushurl;
	std::string		 m_livepullurl;
	std::string		 m_liveplayurl;
	std::string 	 m_failreason;
	SR_uint32		 m_livesvrtype;//ֱ����������,0-SRSֱ����1-������ֱ����2-��Ѷ��ֱ��
	std::map<SR_uint32, std::string> m_liveaddrs;//livesvrtype����0ʱ��ֱ����ַ��Ϣ
	
	SR_bool			m_isErrorTransferLive;
	SR_uint64       m_liveinfo_id;// ��·ֱ��ͨ���Ʒ�id

private:

};


//
//class DevSockInfo
//{
//public:
//	DevSockInfo(){
//		m_ip.clear();
//		m_port = (SR_uint16)0;
//		m_devsockstat = e_DevMgr_sock_init;
//		m_sockptr = (SR_void*)0;
//	}
//	SR_void* getSockPtr(){return m_sockptr;}//TODO:mutex lock
//	std::string   m_ip;//"192.168.1.1"��->��
//	int     m_port;//������
//	E_DevSockStat m_devsockstat;
//	SR_void*	  m_sockptr;
//};

#define MAX_DEVMGR_NUMS (4)
#define DEVMGR_HEART_FLAGS (0xfeed)
class NetMPInfoManager
{
public:
	NetMPInfoManager()
	{
		m_netmpinfo.clear();
		m_idle_netmps.clear();
		m_highload_netmps.clear();
		m_netmpinconf.clear();
		m_notuse_netmps.clear();
		m_connected_nemtps.clear();
		m_devmgrInfos.clear();
		m_current_devmgr_sockptr = (void*)0;
		m_current_dev_stat = DEVMGR_HEART_FLAGS;
		m_current_dev_connectcnt = 0u;
		m_connect_dev_loopcnt = 0u;
	}
	SR_void* getnetmpsocketptr(SR_uint32 netmpid)
	{
		//TODO:������
		std::map<SR_uint32,SR_void*>::const_iterator citor = m_netmpsockptr.find(netmpid);
		if(m_netmpsockptr.end() != citor)
			return citor->second;
		else
			return (SR_void*)0;
	}
	std::map<SR_uint32,NetMPInfo*> m_netmpinfo;//ͨ��DevMgr��ȡ��������netmp��Ϣ
	//DevSockInfo[MAX_DEVMGR_NUMS] m_devmgrInfos;//std::array ��c++11�汾
	std::set<DevSockInfo*> m_devmgrInfos;
	void*   m_current_devmgr_sockptr;
	SR_int32  m_current_dev_stat;
	SR_int32  m_current_dev_connectcnt; // ����devmgr�Ĵ���
	SR_int32  m_connect_dev_loopcnt; // ����devmgr��ѭ������
	std::list<BufferedProtoMsgPair*> m_buffered_dev;
	//���±���Ķ����Ѿ��������ӵ�netmp
	std::set<SR_uint32> m_connected_nemtps;
	std::set<SR_uint32> m_idle_netmps;//loadֵ��С��netmp,���� m_netmpinfo�� NetMPInfo
	std::set<SR_uint32> m_highload_netmps;//loadֵ�ϴ��netmp
	std::set<SR_uint32> m_notuse_netmps;//���������ֵ��netmp
	std::set<SR_uint32> m_netmpinconf;//�Ѿ��ٿ������netmp
	std::map<SR_uint32,SR_void*> m_netmpsockptr;//0606 ��֮ǰ��ȫ��sockfd�Ƶ��˴�
};


class MPInfoManager
{
public:
	MPInfoManager()
	{
		m_mpinfo.clear();
		m_idle_mps.clear();
		m_highload_mps.clear();
		m_mpinconf.clear();
		m_notuse_mps.clear();
		m_connected_mps.clear();
		//m_devmgrInfos.clear();
		//m_current_devmgr_sockptr = (void*)0;
		//m_current_dev_stat = DEVMGR_HEART_FLAGS;
	}
	SR_void* getmpsocketptr(SR_uint32 mpid)
	{
		//TODO:������
		std::map<SR_uint32, SR_void*>::const_iterator citor = m_mpsockptr.find(mpid);
		if (m_mpsockptr.end() != citor)
			return citor->second;
		else
			return (SR_void*)0;
	}
	std::map<SR_uint32, MPInfo*> m_mpinfo;//ͨ��DevMgr��ȡ��������netmp��Ϣ
	////DevSockInfo[MAX_DEVMGR_NUMS] m_devmgrInfos;//std::array ��c++11�汾
	//std::set<DevSockInfo*> m_devmgrInfos;
	//void*   m_current_devmgr_sockptr;
	//SR_int32  m_current_dev_stat;
	//std::list<BufferedProtoMsgPair*> m_buffered_dev;
	//���±���Ķ����Ѿ��������ӵ�mp
	std::set<SR_uint32> m_connected_mps;
	std::set<SR_uint32> m_idle_mps;//loadֵ��С��netmp,���� m_mpinfo�� MPInfo
	std::set<SR_uint32> m_highload_mps;//loadֵ�ϴ��mp
	std::set<SR_uint32> m_notuse_mps;//���������ֵ��mp
	std::set<SR_uint32> m_mpinconf;//�Ѿ��ٿ������mp
	std::map<SR_uint32, SR_void*> m_mpsockptr;//0606 ��֮ǰ��ȫ��sockfd�Ƶ��˴�
};

class GWInfoManager
{
public:
	GWInfoManager()
	{
		m_gwinfo.clear();
		m_idle_gws.clear();
		m_highload_gws.clear();
		m_gwinconf.clear();
		m_notuse_gws.clear();
		m_connected_gws.clear();
	}
	SR_void* getgwsocketptr(SR_uint32 gwid)
	{
		//TODO:������
		std::map<SR_uint32, SR_void*>::const_iterator citor = m_gwsockptr.find(gwid);
		if (m_gwsockptr.end() != citor)
			return citor->second;
		else
			return (SR_void*)0;
	}
	std::map<SR_uint32, GWInfo*> m_gwinfo;//ͨ��DevMgr��ȡ��������gw��Ϣ
	//���±���Ķ����Ѿ��������ӵ�gw
	std::set<SR_uint32> m_connected_gws;
	std::set<SR_uint32> m_idle_gws;//loadֵ��С��gw,���� m_gwinfo�� GWInfo
	std::set<SR_uint32> m_highload_gws;//loadֵ�ϴ��gw
	std::set<SR_uint32> m_notuse_gws;//���������ֵ��gw
	std::set<SR_uint32> m_gwinconf;//�Ѿ��ٿ������mp
	std::map<SR_uint32, SR_void*> m_gwsockptr;//0606 ��֮ǰ��ȫ��sockfd�Ƶ��˴�
};

class CRSInfoManager
{
public:
	CRSInfoManager()
	{
		m_crsinfo.clear();
		m_idle_crss.clear();
		//m_highload_crss.clear();
		//m_crsinconf.clear();
		//m_notuse_crss.clear();
		m_connected_crss.clear();
	}
	SR_void* getcrssocketptr(SR_uint32 crsid)
	{
		//TODO:������
		std::map<SR_uint32, SR_void*>::const_iterator citor = m_crssockptr.find(crsid);
		if (m_crssockptr.end() != citor)
			return citor->second;
		else
			return (SR_void*)0;
	}
	std::map<SR_uint32, CRSInfo*> m_crsinfo;//ͨ��DevMgr��ȡ��������crs��Ϣ
	//���±���Ķ����Ѿ��������ӵ�crs
	std::set<SR_uint32> m_connected_crss;
	std::set<SR_uint32> m_idle_crss;//loadֵ��С��crs,���� m_crsinfo�� CRSInfo
	//std::set<SR_uint32> m_highload_crss;//loadֵ�ϴ��crs
	//std::set<SR_uint32> m_notuse_crss;//���������ֵ��crs
	//std::set<SR_uint32> m_crsinconf;//�Ѿ��ٿ������crs
	std::map<SR_uint32, SR_void*> m_crssockptr;//0606 ��֮ǰ��ȫ��sockfd�Ƶ��˴�
};

class SRSInfoManager
{
public:
	SRSInfoManager()
	{
		m_srsinfo.clear();
	}
	std::map<SR_uint32, SRSInfo*> m_srsinfo;//ͨ��DevMgr��ȡ��������srs��Ϣ
};

class RelayServerInfoManager
{
public:
	RelayServerInfoManager()
	{
		m_relaysvrinfo.clear();
		m_idle_relaysvrs.clear();
		m_highload_relaysvrs.clear();
		m_relaysvrinconf.clear();
		m_notuse_relaysvrs.clear();
		m_connected_relaysvrs.clear();
	}
	SR_void* getrelayserversocketptr(SR_uint32 relayserverid)
	{
		//TODO:������
		std::map<SR_uint32, SR_void*>::const_iterator citor = m_relaysvrsockptr.find(relayserverid);
		if (m_relaysvrsockptr.end() != citor)
			return citor->second;
		else
			return (SR_void*)0;
	}
	std::map<SR_uint32, RelayServerInfo*> m_relaysvrinfo;//ͨ��DevMgr��ȡ��������relayserver��Ϣ
	//���±���Ķ����Ѿ��������ӵ�relayserver
	std::set<SR_uint32> m_connected_relaysvrs;
	std::set<SR_uint32> m_idle_relaysvrs;//loadֵ��С��relayserver,���� m_relaysvrinfo�� RelayServerInfo
	std::set<SR_uint32> m_highload_relaysvrs;//loadֵ�ϴ��relayserver
	std::set<SR_uint32> m_notuse_relaysvrs;//���������ֵ��relayserver
	std::set<SR_uint32> m_relaysvrinconf;//�Ѿ��ٿ������mp
	std::map<SR_uint32, SR_void*> m_relaysvrsockptr;//0606 ��֮ǰ��ȫ��sockfd�Ƶ��˴�
};

class RelayMcInfoManager
{
public:
	RelayMcInfoManager()
	{
		m_relaymcinfo.clear();
	}
	~RelayMcInfoManager()
	{
		m_relaymcinfo.clear();
	}
	std::map<SR_uint32, RelayMcInfo*> m_relaymcinfo;//ͨ��DevMgr��ȡ��������relaymc��Ϣ
};
class DevMgrNetMPProcessThread  : public ThreadMessgeHandler
{
public:
	enum tag_timerflag{
		e_waitConfInfo_timer = 0xa,
		e_waitNetCreatRsp_timer = 0xb,
		e_hearttodevmgr_timer = 0xc,
		e_hearttonetmp_timer = 0xd,
		e_check_netmp_timer = 0xe,
		e_check_devmgr_timer = 0xf,
		e_update_netmp_timer = 0x1a,
		e_hearttomp_timer = 0x1b,
		e_update_mp_timer = 0x1c,
		e_hearttogw_timer = 0x1d,
		e_update_gw_timer = 0x1e,
		e_update_crs_timer = 0x1f,
		e_hearttocrs_timer = 0x20,
		e_update_srs_timer = 0x21,
		e_update_sys_load_timer = 0x22,
		e_update_relayserver_timer = 0x23,
		e_hearttorelaysvr_timer = 0x24,
		e_update_relaymc_timer = 0x25,
		e_checkrelaymc_timeout = 0x26,
		e_connect_devmgr_timer = 0x615,
		e_re_register_timer = 0x616
	};
	DevMgrNetMPProcessThread();
	SR_void startUp();
	virtual SR_void OnMessage(Message* msg);
	virtual CAsyncThread* GetSelfThread() { return m_selfThread; }
	void* createTimer(SR_uint64 millisecond,SR_int32 timertype,SR_uint32 timerflag, SR_uint64 timerdata);
	SR_bool deleteTimer(SR_void* timerid);
	TimerManager* m_timermanger;
	CAsyncThread* m_terthreadhandle;
	CAsyncThread* m_selfThread;
	TerMsgProcessThread* m_terprocess;
	DeviceManager* m_deviceMan;
	void PostToTerThread(unsigned int id, SRMC::MessageData *pdata = NULL)
	{
		if (m_terthreadhandle && m_terprocess)
			m_terthreadhandle->Post(m_terprocess, id, pdata);
	}
	static NetMPInfoManager* m_netmpinfomanager;
	static MPInfoManager* m_mpinfomanager;
	static GWInfoManager* m_gwinfomanager;
	static CRSInfoManager* m_crsinfomanager;
	static SRSInfoManager* m_srsinfomanager;
	static RelayServerInfoManager* m_relaysvrinfomgr;
	static RelayMcInfoManager* m_relaymcinfomgr;
private:
	SR_void updateNetmpInfoPeric();
	SR_void updateMPInfoPeric();
	SR_void updateGWInfoPeric();
	SR_void updateCRSInfoPeric();
	SR_void updateSRSInfoPeric();
	SR_void updateRelayServerInfoPeric();
	SR_void updateRelayMCInfoPeric();
	SR_void updateSystemCurLoadPeric();

	//�����е� socket ��д�쳣
	SR_void onSockError(const SockErrorData* psed);//e_sock_error
	SR_void onTimerArrive(const TimerData* ptd);//e_timerid
	SR_void onConnectSuccess(const SockRelateData* psrd);//e_connector_connect_ok
	SR_void onTcpDataRecv(const SockTcpData* pstd);//e_sock_data

	//netmp->MC devMgr->MC protobuf msg
	SR_void processRspMCConfCreate(const SRMsgs::RspMCConfCreate* s);
	SR_void processIndMCHeartBeatOfNetMP(const SRMsgs::IndMCHeartBeatOfNetMP* s);
	SR_void processRspRegister(const SRMsgs::RspRegister* s);
	
	//SRTcpConnector* m_pconnector;//���ӹ���,����֮������Ӧ��sock�¼�
	void* m_periodtimer_updatenetmp;
	void* m_periodtimer_hearttodevmgr;
	void* m_periodtimer_hearttonetmp;
	void* m_check_devmgr_heart_timer;

	void* m_periodtimer_updatemp;
	void* m_periodtimer_hearttomp;

	void* m_periodtimer_updategw;
	void* m_periodtimer_hearttogw;

	void* m_periodtimer_updatecrs;
	void* m_periodtimer_hearttocrs;

	void* m_periodtimer_updatesrs;
	void* m_periodtimer_updatesyscurload;

	void* m_periodtimer_updaterelayserver;
	void* m_periodtimer_hearttorelayserver;
	void* m_periodtimer_updaterelaymc;
	void* m_periodtimer_checkrelaymctimeout;
	
	std::list<SR_void*> m_re_register_timer_list;// ����ע�ᶨʱ���б�
	std::list<SR_void*> m_connect_devmgr_timer_list;// �����豸��������ʱ���б�
	//SR_bool m_isRegisterOK;
};

typedef struct
{
	SR_uint64 m_msgid;
	void*    m_ptimer;
}Msgid_Timerid_Pair;

typedef struct
{
	SR_uint64 m_confid;
	SR_uint32 m_tersuid;

	SR_void*  m_ptimer;
}Confid_Tersuid_Pair;

typedef struct
{
	SR_uint64 m_confid;
	SR_uint32 m_relaysvrid;
	SR_uint32 m_tersuid;

	SR_void*  m_ptimer;
}RelaySvrid_Confid_Pair;

typedef struct
{
	SR_uint64 m_confid;
	SR_uint32 m_netmpid;
	SR_uint32 m_netmpno;
	SR_uint32 m_netmpgroupid;

	SR_void*  m_ptimer;
}NetMPid_Confid_Pair;

typedef struct
{
	SR_uint64 m_confid;

	SR_uint32 m_mpid;
	SR_uint32 m_channelid;
	SR_uint32 m_relatednetmpid;
	SR_bool m_isErrorTransfer;

	SR_void*  m_ptimer;
}Confid_ReqCrtMPI_Pair;

typedef struct
{
	SR_uint64 m_confid;
	SR_uint32 m_terid;
	void*    m_deskshare_timer;
}Confid_Terid_Pair;

typedef struct
{
	SR_uint64 m_confid;

	SR_uint32 m_mpid;
	SR_uint32 m_channelid;
	SR_uint32 m_screenid;
	SR_uint32 m_screentype;
	SR_uint32 m_reqseqnum;
	SR_bool m_isErrorTransferScr;

	SR_void*  m_ptimer;
}Confid_ReqCrtScr_Pair;

typedef struct
{
	SR_uint64 m_confid;

	SR_uint32 m_crsid;
	SR_uint32 m_crschannelid;
	SR_uint32 m_relatednetmpid;
	//SR_uint32 m_reqseqnum;
	//SR_bool m_isErrorTransfer;

	SR_void*  m_ptimer;
}Confid_ReqCRSCrtConf_Pair;

typedef struct
{
	SR_uint64 m_confid;
	SR_uint32 m_crsid;
	SR_uint32 m_crschannelid;
	SR_uint32 m_channelid;
	SR_uint32 m_screenid;
	SR_uint32 m_screentype;
	SR_uint32 m_reqseqnum;

	SR_void*  m_ptimer;
}Confid_ReqCRSStartRec_Pair;
typedef struct
{
	SR_uint64 m_confid;
	SR_uint32 m_reqlicnum;
	SR_void*  m_ptimer;
}Confid_ReqLicenseFromDevMgr_Pair;
typedef struct
{
	SR_uint64 m_confid;
	SR_uint32 m_crsid;
	SR_uint32 m_crschannelid;
	SR_uint32 m_channelid;
	SR_uint32 m_screenid;
	SR_uint32 m_screentype;
	SR_uint32 m_reqseqnum;
	SR_uint32 m_reqreason; // 0-ҳ����ֹͣ¼��,1-������������ֹͣ¼��,2-����Ǩ������ֹͣ¼��

	SR_void*  m_ptimer;
}Confid_ReqCRSStopRec_Pair;

typedef struct
{
	SR_uint64 m_confid;
	SR_uint32 m_crsid;
	SR_uint32 m_crschannelid;
	SR_uint32 m_livechannelid;
	SR_uint32 m_livescreenid;
	SR_uint32 m_livescreentype;
	SR_uint32 m_relatedsrsid;
	SR_uint32 m_reqseqnum;

	SR_void*  m_ptimer;
}Confid_ReqCRSStartLive_Pair;

typedef struct
{
	SR_uint64 m_confid;
	SR_void*  m_ptimer;
}Confid_LayoutPoll_Pair;
//typedef struct
//{
//	SR_uint64 m_confid;
//	SR_uint32 m_crsid;
//	SR_uint32 m_crschannelid;
//	SR_uint32 m_livechannelid;
//	SR_uint32 m_livescreenid;
//	SR_uint32 m_livescreentype;
//	SR_uint32 m_reqseqnum;
//	SR_uint32 m_reqreason; // 0-ҳ����ֹͣ¼��,1-������������ֹͣ¼��,2-����Ǩ������ֹͣ¼��
//}Confid_ReqCRSStopLive_Pair;

class BufferedProtoMsgPair{
public:
	explicit BufferedProtoMsgPair(SR_uint32 msguid,
					google::protobuf::Message* pmsg):m_msguid(msguid),m_pmsg(pmsg){}
	~BufferedProtoMsgPair(){
		if(m_pmsg)
			delete m_pmsg;
		m_pmsg = (google::protobuf::Message*)0;
	}
	SR_uint32 m_msguid;
	google::protobuf::Message* m_pmsg;
private:
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(BufferedProtoMsgPair);
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(BufferedProtoMsgPair);
};

class GlobalVars
{
public:
	static SR_int32 getAllTerNums(){
	#ifdef USE_LOCK_RW_ALL_TERSNUM	
		CThreadMutexGuard crit(m_lock);//�˴���������
	#endif
		return ters_in_mc;
	}
	static SR_void addOrSubTerNums(SR_int32 addOrSub){//���̵߳���addOrSubTerNumsʱ
	#ifdef USE_LOCK_RW_ALL_TERSNUM
		GlobalVars::m_lock.Lock();
	#endif	
		ters_in_mc += addOrSub;
	#ifdef USE_LOCK_RW_ALL_TERSNUM
		GlobalVars::m_lock.Unlock();
	#endif
	}
private:
	static volatile SR_int32 ters_in_mc;
	#ifdef USE_LOCK_RW_ALL_TERSNUM
	static CThreadMutex	m_lock;
	#endif
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(GlobalVars);
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(GlobalVars);
};

#define GET_CURRENT_DEVMGR() (DevMgrNetMPProcessThread::m_netmpinfomanager->m_current_devmgr_sockptr)
#define IS_DEVMGR_UNREGISTER() ((NULL == DevMgrNetMPProcessThread::m_netmpinfomanager->m_current_devmgr_sockptr) || (0u == MCCfgInfo::instance()->get_mcdeviceid()))
// ���뷢�͸� netmp �� devmgr
SR_void SerialAndSend(SR_void* sockptr,SR_uint32 proto_msguid,const google::protobuf::Message* msg);
SR_void SerialAndSendDevmgr_nobuf(SR_uint32 proto_msguid,const google::protobuf::Message* msg);
//���߳�,ֻ���ն��̵߳���
SR_void SerialAndSendDevmgr_buffered(SR_uint32 proto_msguid,const google::protobuf::Message* msg);

//SR_void SerialAndSendToCCS_nobuf(SR_uint32 proto_msguid, const google::protobuf::Message* msg);
//SR_void SerialAndSendToCCS_buffered(SR_uint32 proto_msguid, const google::protobuf::Message* msg);

}// namespace SRMC
#endif //#ifndef SRMCPROCESS_H