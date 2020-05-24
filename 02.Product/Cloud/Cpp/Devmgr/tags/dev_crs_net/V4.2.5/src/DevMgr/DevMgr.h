
#pragma once

#include "sredisdll.h"
//#include "sr_timer.h"
#include "device_ser.pb.h"
#include "deviceConnMgr.h"
#include "HttpConnMgr.h"
#include "sr_type.h"
#include <vector>
#include <map>

#include "sr_msgprocess.h"
#include "sr_queuemsgid.h"
#include "timer_manager.h"

using namespace ISREDIS;
using std::vector;
using std::map;
using std::string;

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#include <WS2tcpip.h>
#elif defined(LINUX) || defined(POSIX)
#define __stdcall
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Queue.h"
#include "MsgQueue.h"
#endif

static const SR_uint32 kdevmgr_sock_type = 0xbeafu;

#define MAX_SUB_NODE_NUM 400

// 定时检测心跳时间5000毫秒
#define CHECK_CLIENT_HEARTBEAT_TIME 5000
// 定时发送心跳时间3000毫秒
#define SEND_HEARTBEAT_TO_DEVMGR_TIME 3000
// 定时检测服务器（devmgr）心跳时间5000毫秒
#define CHECK_SVR_HEARTBEAT_TIME 5000
// 写自身心跳时间5000毫秒
#define UP_SELF_HEARTBEAT_TIME 5000
// redis数据回写时间5000毫秒
#define WRITE_DB_TIME 5000
// 连接上级设备管理器的间隔时间3000毫秒
#define CONNECT_DEVMGR_TIME 3000


#define SRMSG_CMDTYPE_RELAY_TER_MC (200)
#define SRMSG_CMDTYPE_NETMP_MC     (201)
#define SRMSG_CMDTYPE_DevMgr_MC    (202)

struct sDeviceInfo
{
	unsigned int devicetype;
	time_t time;
	std::string token;
	DeviceConnect* pClient;
};

struct sConfReport
{
	bool bOperate;
	int op; // 1-插入，2-更新
	unsigned long long conf_id;
	unsigned long long conf_report_id;
	time_t lltime;
	std::string time;
};

struct sDeviceConfDetail
{
	int op; // 1-插入，2-更新
	int device_id;
	unsigned long long conf_id;
	unsigned long long conf_report_id;
	unsigned long long device_detail_id;
	time_t lltime;
	std::string time;
};

struct sUserConfInfo
{
	int op; // 1-插入，2-更新
	int user_id;
	unsigned long long conf_id;
	unsigned long long conf_report_id;
	unsigned long long userrpt_detail_id;
	time_t lltime;
	std::string time;
	std::string user_alias;
};

class CAsyncThread;
class CUserConfDetail;
class CDevice;
class CDeviceConfig;
class CConference;
class CConfReport;
class CConfRecord;
class CRecordFile;
class CConfLiveInfo;
class CDeviceConfDetail;
class CConfParticipant;
class CConfLiveSetting;
class CConfRollCallInfo;
class CRollCallList;
class CConfPollInfo;
class CPollList;


struct sMysqlTablePtr
{
	CDevice* pDevice;
	CDeviceConfig* pDeviceConfig;
	CConference* pConference;
	CConfReport* pConfReport;
	CUserConfDetail* pUserConfDetail;
	CDeviceConfDetail* pDeviceConfDetail;
};

typedef enum{
	e_RC_TT_MainThread = 0x0,
	e_RC_TT_UpMsqThread = 0x1,
	e_RC_TT_WriteDBThread = 0x2,
	e_RC_TT_HttpThread = 0x3
}E_RedisConnThreadType;

enum tag_timerflag{
	e_update_selfheartbeat2db_timer = 0xaaaa,
	e_check_client_heartbeat_timer = 0xaaab,
	e_write_redis2mysql_timer = 0xaaac,
	e_connect_devmgr_timer = 0xaaad,
	e_re_register_timer = 0xaaae,
	e_hearttodevmgr_timer = 0xaab1,
	e_check_acitiveconnect_timer = 0xaab2,
	e_check_svr_heartbeat_timer = 0xaab3,
	e_req_up_relaymcinfo_timer = 0xaab4,
	e_req_up_relayserverinfo_timer = 0xaab5,
	e_req_up_stunserverinfo_timer = 0xaab6,
	e_register_self_timer = 0xaab7,
};

class UpdateDeviceInfoData
{
public:
	UpdateDeviceInfoData()
	{
		m_mapDeviceidandConfidset.clear();
		m_mapRelatedDeviceidandConfidset.clear();
	}
	~UpdateDeviceInfoData()
	{
		for (std::map< int, std::vector<unsigned long long> >::iterator itor = m_mapDeviceidandConfidset.begin();
			itor != m_mapDeviceidandConfidset.end(); itor++)
		{
			itor->second.clear();
		}
		m_mapDeviceidandConfidset.clear();

		for (std::map< int, std::vector<unsigned long long> >::iterator itor_r = m_mapRelatedDeviceidandConfidset.begin();
			itor_r != m_mapRelatedDeviceidandConfidset.end(); itor_r++)
		{
			itor_r->second.clear();
		}
		m_mapRelatedDeviceidandConfidset.clear();
	}

	std::map< int, std::vector<unsigned long long> > m_mapDeviceidandConfidset;
	std::map< int, std::vector<unsigned long long> > m_mapRelatedDeviceidandConfidset;
};

class CDevMgr : public ICsredisSink, public SRMC::ThreadMessgeHandler//, public TcpServerSink
{
public:
	
	CDevMgr();
	~CDevMgr();

	bool Init();
	void Fini();
	void Splitpathex(const char *path, char *dirname, char *filename);

	//void DeleteData(int deviceid);

public:
	virtual int OnServerReciveData(void* pclient, char* data, uint32_t data_len);//must return how many data Has been copied(recive)
	virtual int OnClientReciveData(void* psvr, char* data, uint32_t data_len);

public:
	virtual void __stdcall OnConnect(const int isok);
	virtual void __stdcall OnError(const char* error);

	virtual void __stdcall OnClosed(void *pTcpConnet);

	virtual void __stdcall ActiveConnOnConnected(void *pActiveTcp);
	virtual void __stdcall ActiveConnOnClosed(void *pActiveTcp);

	virtual void __stdcall RegisterToDevmgr();

	//SR_void onActiveConnSockError(void *pActiveTcp);
	//SR_void onActiveConnectSuccess(void *pActiveTcp);

	ICsredis** m_pRedisConnList;				//Redis连接表
	SR_uint32 m_uiNeedRedisCoonNum;

	CAsyncThread*	m_pMainThread;			// 用户机主线程
	CDevice* m_pMainDevice;
	CDevice* m_pUpmsgDevice;
	std::map<CAsyncThread*, CDevice*> m_mapAsyncThreadDevice;

	std::map<std::string, std::string> m_mapSystemDomainInfo;
	std::string m_softwareversion;

	CDeviceConfig* m_pMainDeviceConfig;
	CDeviceConfig* m_pUpmsgDeviceConfig;
	std::map<CAsyncThread*, CDeviceConfig*> m_mapAsyncThreadDeviceConfig;

	CConference* m_pMainConference;
	CConference* m_pUpmsgConference;
	CConference* m_pWriteDBConference;
	std::map<CAsyncThread*, CConference*> m_mapAsyncThreadConference;

	CConfReport* m_pMainConfReport;
	CConfReport* m_pUpmsgConfReport;
	CConfReport* m_pWriteDBConfReport;
	std::map<CAsyncThread*, CConfReport*> m_mapAsyncThreadConfReport;

	CConfRecord* m_pMainConfRecord;
	CConfRecord* m_pUpmsgConfRecord;
	CConfRecord* m_pWriteDBConfRecord;
	std::map<CAsyncThread*, CConfRecord*> m_mapAsyncThreadConfRecord;

	CUserConfDetail* m_pMainUserConfDetail;
	CUserConfDetail* m_pUpmsgUserConfDetail;
	CUserConfDetail* m_pWriteDBUserConfDetail;
	std::map<CAsyncThread*, CUserConfDetail*> m_mapAsyncThreadUserConfDetial;

	CDeviceConfDetail* m_pMainDeviceConfDetail;
	CDeviceConfDetail* m_pUpmsgDeviceConfDetail;
	CDeviceConfDetail* m_pWriteDBDeviceConfDetail;
	std::map<CAsyncThread*, CDeviceConfDetail*> m_mapAsyncThreadDeviceConfDetail;

	CRecordFile* m_pMainRecordFile;
	CRecordFile* m_pUpmsgRecordFile;
	CRecordFile* m_pWriteDBRecordFile;
	std::map<CAsyncThread*, CRecordFile*> m_mapAsyncThreadRecordFile;

	CConfLiveInfo* m_pMainConfLiveInfo;
	CConfLiveInfo* m_pUpmsgConfLiveInfo;
	CConfLiveInfo* m_pWriteDBConfLiveInfo;
	std::map<CAsyncThread*, CConfLiveInfo*> m_mapAsyncThreadConfLiveInfo;
	
	CConfParticipant* m_pMainConfParticipant;
	std::map<CAsyncThread*, CConfParticipant*> m_mapAsyncThreadConfParticipant;

	CConfLiveSetting* m_pMainConfLiveSetting;
	std::map<CAsyncThread*, CConfLiveSetting*> m_mapAsyncThreadConfLiveSetting;

	CConfRollCallInfo* m_pMainConfRollCallInfo;
	CConfRollCallInfo* m_pWriteDBConfRollCallInfo;
	std::map<CAsyncThread*, CConfRollCallInfo*> m_mapAsyncThreadConfRollCallInfo;
	CRollCallList* m_pMainRollCallList;
	CRollCallList* m_pWriteDBRollCallList;
	std::map<CAsyncThread*, CRollCallList*> m_mapAsyncThreadRollCallList;

	CConfPollInfo* m_pMainConfPollInfo;
	std::map<CAsyncThread*, CConfPollInfo*> m_mapAsyncThreadConfPollInfo;
	CPollList* m_pMainPollList;
	std::map<CAsyncThread*, CPollList*> m_mapAsyncThreadPollList;

	//CUserConfDetail* m_pMainUserConfDetail;
	//CUserConfDetail* m_pUpmsgUserConfDetail;
	//std::map<CAsyncThread*, CUserConfDetail*> m_mapAsyncThreadUserConfDetial;
	
	void Handle_CmdHJGW_OPMonitor_msg(void *pArg);
	void Handle_CmdHJGW_OPInvitePart_msg(void *pArg);
	void Handle_CheckDeviceHeartbeat(void *pArg);
	void Handle_UpdateHttpSvrLisentState(void *pArg);
	int GetSelfDeviceid(){ return m_iSelfDeviceid; }

//#ifdef WIN32
//	UINT m_idCheckHeartbeatTimer;
//#elif defined LINUX
//	timer_t   m_idCheckHeartbeatTimer;
//#endif

	// 创建毫秒级定时器
	void* createTimer(SR_uint64 millisecond, SR_int32 timertype, SR_uint32 timerflag, SR_uint64 timerdata);

protected:

	void GetSoftwareVersion();
	void LoadSystemDomainInfo();
	int UpDomainIPToHostsFile(char* newdomainip, char* domainname);
	int ReloadNginx(char* domainname, char* regip, char* connip, unsigned int port);
	void Handle_ReloadNginx(void* pArg);
	//int CheckDomainInfoHavechange(char* newdomainip, char* domainname, int checktype);

	// realtimetype=0表示真实开始时间,realtimetype=1表示真实结束时间;permanentenable=0表示一次性会议、permanentenable=1表示永久性会议、permanentenable=-1表示超时回写数据时未知会议是一次性会议还是永久会议
	void UpdateConfRealtime(unsigned long long confid, int realtimetype, char* time, time_t lltime, int permanentenable=-1);
	void Handle_UpdateConfRealtime(void* pArg);
	void InsertConfReport(unsigned long long confid, unsigned long long confreportid, char* time, time_t lltime);
	void UpdateConfReport(unsigned long long confid, unsigned long long confreportid, char* time, time_t lltime);
	void Handle_InsertConfReport(void* pArg);
	void Handle_UpdateConfReport(void* pArg);
	
	void InsertDeviceConfDetail(unsigned long long confid, int deviceid, unsigned long long confreportid, unsigned long long devicedetailid, char* time, time_t lltime, bool bfromnetmp = false);
	void UpdateDeviceConfDetail(unsigned long long confid, int deviceid, unsigned long long confreportid, unsigned long long devicedetailid, char* time, time_t lltime);
	void Handle_InsertDeviceConfDetail(void* pArg);
	void Handle_UpdateDeviceConfDetail(void* pArg);

	void InsertConfRecord(unsigned long long confid, unsigned long long confreportid, unsigned long long confrecordid, char* time, time_t lltime, char* confname);
	void UpdateConfRecord(unsigned long long confid, unsigned long long confreportid, unsigned long long confrecordid, char* time, time_t lltime, char* confname);
	void Handle_InsertConfRecord(void* pArg);
	void Handle_UpdateConfRecord(void* pArg);

	void InsertRecordFile(unsigned long long confid, unsigned long long confreportid, unsigned long long confrecordid, unsigned long long recordfileid, char* filestorsvrip, char* filerootpath, char* filepath, char* sdefilepath, time_t lltime, unsigned long long filesize);
	//void UpdateRecordFile(unsigned long long confid, unsigned long long confrecordid, char* time, time_t lltime);
	void Handle_InsertRecordFile(void* pArg);
	//void Handle_UpdateRecordFile(void* pArg);

	void InsertConfLiveInfo(unsigned long long liveinfoid, const SRMsgs::IndCRSStartLive* ind, char* time, time_t lltime);
	void Handle_InsertConfLiveInfo(void* pArg);
	void UpdateConfLiveInfo(const SRMsgs::IndCRSStopLive* ind, char* time, time_t lltime);
	void Handle_UpdateConfLiveInfo(void* pArg);

	void ModifyConfCallListInfo(const SRMsgs::IndModifyConfCallList* ind, char* time, time_t lltime);
	void Handle_ModifyConfCallListInfo(void* pArg);
	void UpdateTerStatisticsInfo(const SRMsgs::IndTerStatisticsInfo* ind, char* time, time_t lltime);
	void Handle_UpdateTerStatisticsInfo(void* pArg);

	void AddUserConfDetail(int suid, unsigned long long confid, unsigned long long confreportid, unsigned long long userrptdetailid, char* alias, char* time, time_t lltime, int fromtype, int termtype);
	void DelUserConfDetail(int suid, unsigned long long confid, unsigned long long confreportid, unsigned long long userrptdetailid, char* time, time_t lltime);
	
	void DeleteAllRedis();
	void LoadAllRedisDevice();
	int GetProcessPath();
	bool ServerTcpStart(const int listenport);
	void StartHttpServer(void* pArg);
	bool ParseClientData(DeviceConnect *pClient, CAsyncThread* pThread, const char* pData, const int nLen, unsigned long recv_data_time);

	void Handle_WriteUserConfInfoToDB(void* pArg);
	unsigned int WriteUserConfInfoToDB(CConference* pConference, CConfReport* pConfReport, CDeviceConfDetail* pDeviceConfDetail, CUserConfDetail* pUserConfDetail, CConfRecord* pConfRecord, CRecordFile* pRecordFile, CConfLiveInfo* pConfLiveInfo
		, CConfRollCallInfo* pConfRollCallInfo, CRollCallList* pRollCallList);

	void Handle_AddUserConfDetail(void* pArg);
	void Handle_DelUserConfDetail(void* pArg);
	void Handle_ServerReciveData(void* pArg);
	//void Handle_DeleteData(void* pArg);
	void Handle_IndServerHeart(void *pArg);

	void Handle_DeviceOnClose(void *pArg);
	void Handle_ActiveConnOnClose(void *pArg);
	void Handle_ActiveConnOnConnected(void *pArg);
	void UpdateDeviceInfoInDB(int device_id, int device_type, CAsyncThread *pThread, char* ptime, time_t lltime);
	void Handle_UpdateDeviceInfoInDB(void *pArg);
	void SendHeartbeatInd(DeviceConnect *pClient, char* token);
	void Handle_SendHeartbeatInd(void *pArg);
	
	void SendHeartBeatToDevmgr();
	void Handle_SendHeartBeatToDevmgr(void *pArg);

	void ReqUPServerInfoPeric(unsigned int reqgetsvrtype);
	void Handle_ReqUPServerInfoToDevmgr(void *pArg);
	
	//void ReqUPRelayMCInfoPeric();
	//void Handle_ReqUPRelayMCInfoToDevmgr(void *pArg);
	//void ReqUPRelayServerInfoPeric();
	//void Handle_ReqUPRelayServerInfoToDevmgr(void *pArg);
	//void ReqUPStunServerInfoPeric();
	//void Handle_ReqUPStunServerInfoToDevmgr(void *pArg);

	void CheckConfTimeoutData();
	void Handle_CheckConfTimeoutData(void *pArg);
	void PushMsgToWriteDBQueue(char* pushvalue);
	void Handle_PushMsgToWriteDBQueue(void *pArg);

	void CheckWriteDB();
	void Handle_CheckWriteDB(void *pArg);

	void CheckConfig();
	void Handle_CheckConfig(void *pArg);
	
	void CheckDBConn();
	void Handle_CheckDBConn(void *pArg);
	void Handle_CheckRedisConn(void *pArg);

	void OnRegisterSelf();
	void Handle_RegisterSelf(void *pArg);
	bool RegisterSelf(const SRMsgs::ReqRegister* req, CDeviceConfig* pDeviceConfig, CDevice* pDevice);
	void UpdateSelfHeartbeatToDB(int selfdeviceid, time_t timeNow);
	void Handle_UpdateSelfHeartbeatToDB(void *pArg);

	bool ProcessReqRegister(const SRMsgs::ReqRegister* req, DeviceConnect *pClient, CDeviceConfig* pDeviceConfig, CDevice* pDevice, SRMsgs::RspRegister* rspsend);
	bool GetDeviceInfo(const SRMsgs::ReqGetDeviceInfo* req, SRMsgs::RspGetDeviceInfo* rsp);
	bool GetSysDeviceInfo(const SRMsgs::ReqGetSysDeviceInfo* req, SRMsgs::RspGetSysDeviceInfo* rsp);
	bool GetSystemCurLoad(const SRMsgs::ReqGetSystemCurLoad* req, SRMsgs::RspGetSystemCurLoad* rsp);
	void UpdatePeerHeartbeatToDB(const SRMsgs::IndsertodevHeart* ind, time_t timeNow);
	void GetConfInfoFromDB(const SRMsgs::ReqConfInfoFromDevMgr* req, CConference* pConference, CConfParticipant* pConfParticipant, CConfLiveSetting* pConfLiveSetting
		, CConfRollCallInfo* pConfRollCallInfo, CRollCallList* pRollCallList, CConfPollInfo* pConfPollInfo, CPollList* pPollList, SRMsgs::RspConfInfoToMC* rspsend);

	void SerialProtoMsgAndSend(DeviceConnect *pClient, int proto_msguid, const google::protobuf::Message* msg);

	void SyncConfInfo(const SRMsgs::ReqRegister* req, CDevice* pDevice, time_t timeNow);
	void GetSubGroupidInfo(SR_uint32 uiParentGroupid, std::set<SR_uint32> &groupids, SR_uint32 &uiNodeNum);

	void SyncSubSvrInfo(const SRMsgs::IndSubSvrInfoToDevmgr* ind, time_t timeNow);
	void UpdateSubSvrHeartbeatToDB(const SRMsgs::IndSubSvrHeartTodev* ind, time_t timeNow);
	
	// server<-->(me)client
	void Handle_ClientReciveData(void* pArg);
	void Handle_RegisterToDevmgr(void *pArg);
	bool ParseServerData(AcitiveConnect *pSvr, CAsyncThread* pThread, const char* pData, const int nLen, unsigned long recv_data_time);
	SR_void processRspRegister(const SRMsgs::RspRegister* s);
	void SyncUpSvrInfo(const SRMsgs::IndUpSvrInfoToDevmgr* ind, time_t timeNow);
	void UpdateUpSvrHeartbeatToDB(const SRMsgs::IndUpSvrHeartTodev* ind, time_t timeNow);
	void SyncSysSvrInfo(const SRMsgs::RspGetSysDeviceInfo* rsp, time_t timeNow);
	void SerialAndSendDevmgr(AcitiveConnect *pSvrConn, int proto_msguid, const google::protobuf::Message* msg);

	//// 创建毫秒级定时器
	//void* createTimer(SR_uint64 millisecond, SR_int32 timertype, SR_uint32 timerflag, SR_uint64 timerdata);
	virtual CAsyncThread* GetSelfThread(){ return m_selfThread; }// 主线程
	virtual SR_void OnMessage(SRMC::Message* msg);
	SR_void onTimerArrive(const SRMC::TimerData* ptd);//e_timerid
	void CheckDeviceHeartbeat();

private:
	//CAsyncThread*	m_pMainThread;			    // 用户机主线程
	CAsyncThread*	m_pWriteDBThread;			// 写数据库线程
	CAsyncThread*	m_pUpMsqThread;			    // 将消息写入内存线程
	CAsyncThread*	m_pUpConfigThread;			// 定时读取配置文件更新配置线程
	CAsyncThread*	m_pProcessHttpThread;       // 处理HTTP的线程

	//ICsredis* m_pRedis;
	CCriticalSection m_cs;
	//CRedisManager m_oRedisMgr;

	bool m_bConnRedis;
	bool m_bConnMysql;
	bool m_bRegSelfok;
	int m_iSelfDeviceid;
	bool m_HttpSvrListenISok;

	SR_void* m_checkclientheartbeat_periodtimer;
	SR_void* m_upselfhb2db_periodtimer;
	SR_void* m_writeredis2mysql_periodtimer;
	SR_void* m_heart2devmgr_periodtimer;
	SR_void* m_check_svr_heartbeat_timer;
	SR_void* m_req_up_relaymcinfo_periodtimer;
	SR_void* m_req_up_relayserverinfo_periodtimer;
	SR_void* m_req_up_stunserverinfo_periodtimer;
	CAsyncThread* m_selfThread; // 目前指向主线程
	TimerManager* m_timermanger;

	ITcpTransLib* m_pTcp_trans_module;
	//ITcpTransLib* m_pHttpTcp_transmodule;
	//SR_Timer*		m_pTimer;

    DeviceConnMgr* m_devConnMgr;
	HttpConnMgr* m_HttpConnMgr;

	FILE* m_fpLogFile;
	char m_msgLogfile[1024];
	char			m_fullpath[1024];
	//std::vector<CDeviceID*>		m_pVecDeviceID;
	long long		m_curTime;

	std::map<int, sDeviceInfo> m_MapDeviceHeartbeatTime;
	std::pair<int, sDeviceInfo>	pair_MapDeviceHeartbeatTime;
	typedef std::map<int, sDeviceInfo>::value_type valType;
	typedef std::map<int, sDeviceInfo>::iterator iter_mapDeviceHeartbeatTime;


	std::vector<sConfReport> m_sConfReport;
	typedef std::vector<sConfReport>::iterator ConfReport_iter;
	typedef std::vector<sConfReport>::reverse_iterator ConfReport_reiter;
	CCriticalSection m_csCR;

	std::vector<sDeviceConfDetail> m_sDeviceConfDetail;
	typedef std::vector<sDeviceConfDetail>::iterator DeviceConfDetail_iter;
	typedef std::vector<sDeviceConfDetail>::reverse_iterator DeviceConfDetail_reiter;
	CCriticalSection m_csDCD;

	std::vector<sUserConfInfo> m_sUserConfInfo;
	typedef std::vector<sUserConfInfo>::iterator UserConfInfo_iter;
	CCriticalSection m_csUCI;

	unsigned int m_op_db_num;
	unsigned int m_write_db_timespan;
	//unsigned long long m_ullnum_per_post;
	time_t m_last_post_msg_time; // 秒级
	unsigned int m_nGetDevHeartbeatTimeout;
	unsigned int m_nConfdtlinfoTimeout;// redis中会议计费信息超时时间,单位（秒）
	bool m_bCloseTimeoutSocket; // 是否删除心跳超时socket


	int m_iLoglevel;
	bool m_bisStdout;
	unsigned int m_uiDeviceNo;

	unsigned int m_uiMyDeviceid;
	std::string m_szMyToken;
	unsigned int m_uiMyGroupid;

};