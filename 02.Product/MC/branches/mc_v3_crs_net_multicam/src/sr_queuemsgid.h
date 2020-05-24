#ifndef SR_QUEUEMSGID_H
#define SR_QUEUEMSGID_H
#include <set>
#include <string>
#include <string.h>
#include <map>
#include <list>
#include "sr_message.h"
#include "sr_tcpheader.h"

namespace SRMC{


typedef enum //��ʱ������
{	
	e_periodicity_timer = 0xcad,//Ĭ�ϵ������Զ�ʱ��	
	e_dispoable_timer = 0x19880824,//һ���Զ�ʱ��
}E_TimerType;//����ͽ��Ͷ�������߶���
typedef enum{
	e_DeviceType_TER = 0x0, // TER
	e_DeviceType_MCU = 0x1, // MCU
	e_DeviceType_MONITOR = 0x2, // ���
	e_DeviceType_PSTN = 0x3, // PSTN
	e_DeviceType_STREAM = 0x4,//��ý��
	E_DeviceType_TRUNK = 0x5//TRUNK
}E_DeviceType;// �豸����,0-�նˡ�1-MCU��2-��ء�3-PSTN�����绰(�����̻����ֻ�)��4-��ý��
//����������Ҳ�ɵ����߸���
class TimerData : public MessageData
{
public:	
	explicit TimerData(SR_uint32 timerflag,SR_uint64 timerdata)	
	{
		m_timerflag = timerflag;
		m_timerdata = timerdata;
	}	
	~TimerData(){}// do nothing	
	SR_uint32 m_timerflag;//����ͬһ���͵Ĳ�ͬ���õĶ�ʱ��	
	SR_uint64 m_timerdata;
};
class TimeInfo{
public:
	TimeInfo()
	{
		m_starttime.clear();
		m_endtime.clear();
	}

	~TimeInfo()
	{
		m_starttime.clear();
		m_endtime.clear();
	}
	std::string 	 m_starttime;
	std::string 	 m_endtime;
};
//�ն���Ϣ
class Participant
{
public:
	Participant()
	{
		m_name.clear();
		m_suid = 0u;
		m_ip.clear();
		m_protocoltype = 0;
		m_bandwidth = 0;
		m_usertype = 0u;
		m_devicetype = e_DeviceType_TER;
		m_calloption.clear();
		m_devicecode.clear();
		m_shortname.clear();
		m_orderno = 0u;
		//m_isonline = false;

		m_querystate = 0u;
		//m_playbackparm.clear();
		m_totaltimespan.m_starttime.clear();
		m_totaltimespan.m_endtime.clear();
		m_subtimespans.clear();
	}
	~Participant()
	{
		m_name.clear();
		m_suid = 0u;
		m_ip.clear();
		m_protocoltype = 0u;
		m_bandwidth = 0u;
		m_usertype = 0u;
		m_devicetype = e_DeviceType_TER;
		m_calloption.clear();
		m_devicecode.clear();
		m_shortname.clear();
		m_orderno = 0u;
		//m_isonline = false;

		m_querystate = 0u;
		//m_playbackparm.clear();
		m_totaltimespan.m_starttime.clear();
		m_totaltimespan.m_endtime.clear();
		for (std::list<TimeInfo*>::iterator subts_itor = m_subtimespans.begin();
			subts_itor != m_subtimespans.end(); subts_itor++)
		{
			TimeInfo* pti = NULL;
			pti = (*subts_itor);
			if (pti != NULL)
			{
				delete pti;
				pti = NULL;
			}
		}
		m_subtimespans.clear();
	}

	std::string 	m_name;//���ݿ����ն����ƣ���ʼ���ƣ�
	SR_uint32	 	m_suid;//���ݿ����ն�Ψһ��ʶ��(suid)
	std::string 	m_ip;//�ն�ip,devicetype==4ʱ��ʾ��ý��url
	SR_uint32	 	m_protocoltype;//��׼�ն˵ĺ���Э������,0-�Զ���1-sip��2-H.323��3-GB28181��4-PSTN��5-RTSP
	SR_uint32	 	m_bandwidth;// ���д���
	SR_uint32 		m_usertype;// �ն�����,0-��ʱ�û�,1-��ʽ�û�,2-Ubox,3-��׼�ն�(H.323/sip),4-���ǰ��,5-�Ǳ����û�,6-3288�ն�,7-���ǰ��¼��,8-΢��webrtc��ҳ�ͻ���
	SR_uint32		m_devicetype; //�豸����,0-�ն�,1-MCU,2-���,3-PSTN�����绰(�����̻����ֻ�),4-��ý��
	std::string 	m_calloption; // ����ѡ��,һ����֪ͨ��gw��devicetype==1���������
	std::string 	m_devicecode;//����豸����,��devicetype==2-���ʱ������ǰ���豸����
	std::string 	m_shortname;//�̺�,һ����usertype==3��׼�ն�ע��ʱ�̺ź�����
	SR_uint32	 	m_orderno;//�ն�����id
	//SR_bool         m_isonline; // �Ƿ��Ѿ��λ�

	SR_uint32		 m_querystate;// usertype==7,���¼��Ĳ�ѯ���
	//std::string 	 m_playbackparm;// usertype==7,���¼��Ĳ��Ž��Ȳ���
	TimeInfo         m_totaltimespan;// usertype==7,���¼�����ʱ�����Ϣ
	std::list<TimeInfo*> m_subtimespans;// usertype==7,���¼�����ʱ�����Ϣ
private:

};
//��ѯ�б���Ϣ
class Autopollinfo
{
public:
	Autopollinfo()
	{
		m_name.clear();
		m_apid = 0u;
		m_polllist.clear();
	}
	~Autopollinfo()
	{
		m_name.clear();
		m_apid = 0u;
		for (std::list<Participant*>::iterator polllist_itor = m_polllist.begin();
			polllist_itor != m_polllist.end(); polllist_itor++)
		{
			Participant* ppart = NULL;
			ppart = (*polllist_itor);
			if (ppart != NULL)
			{
				delete ppart;
				ppart = NULL;
			}
		}
		m_polllist.clear();
	}

	std::string 	m_name;//���ݿ�����ѯ�б�����������
	SR_uint64	 	m_apid;//���ݿ�����ѯ�б�����Ψһ��ʶ��(rcid)
	std::list<Participant*> m_polllist;// ��ѯ�б�,ֻ��д
private:

};

class IPPortInfo
{
public:
	IPPortInfo()
	{
		m_nettype = 0u;
		m_ip.clear();
		m_port = 0u;
	}
	~IPPortInfo()
	{
		m_nettype = 0u;
		m_ip.clear();
		m_port = 0u;
	}
	SR_uint32     m_nettype;// ��������,0-������1-����
	std::string   m_ip;//
	SR_uint32     m_port;// 
protected:
private:
};

class AddrInfo
{
public:
	AddrInfo()
	{
		m_svrtype = 0u;
		m_level = 0u;
		m_ipports.clear();
	}
	~AddrInfo()
	{
		m_svrtype = 0u;
		m_level = 0u;
		for (std::map<int, IPPortInfo*>::iterator ip_itor = m_ipports.begin();
			ip_itor != m_ipports.end(); ip_itor++)
		{
			IPPortInfo* pIPPortInfo = NULL;
			pIPPortInfo = ip_itor->second;
			if (pIPPortInfo)
			{
				delete pIPPortInfo;
				pIPPortInfo = NULL;
			}
		}
		m_ipports.clear();
	}
	SR_uint32     m_svrtype;// ��������, MC=1��NetMP=2��Relaymc=3��relayserver=4��MP=5��GW=6��Ddevmgr=7��CRS=9��stunserver=11�� 
	SR_uint32     m_level;// relaymc=3�㼶��ϵ
	std::map<int, IPPortInfo*> m_ipports; // key:<-->value:IPPortInfo*
protected:
private:
};

class DomainInfo
{
public:
	DomainInfo()
	{
		m_dlevel = 0u;
		m_dname.clear();
	}
	~DomainInfo()
	{
		m_dlevel = 0u;
		m_dname.clear();
	}
	std::string   m_dname;// ��������
	SR_uint32     m_dlevel;// �����㼶��ϵ
protected:
private:
};
class LicenceInfoData : public MessageData
{
public:
	explicit LicenceInfoData(SR_void* pdata, uint32_t maxsrcount, uint32_t maxstdcount, char* cstdexpiringdate, uint32_t maxreccount, char* crecexpiringdate, uint32_t maxlivecount, char* cliveexpiringdate, uint32_t maxternumperconf, uint32_t maxconfnum, uint32_t maxmonitorcount, char* cmonitorexpiringdate, uint32_t maxvoicecount, char* cvoiceexpiringdate, char* myversion)
	{
		m_data_len = 0;
		m_buf = (SR_uint8*)0;
		m_data_len = strlen((char*)pdata);
		if (m_data_len != 0)
		{
			m_buf = new SR_uint8[m_data_len];
			memcpy(m_buf, pdata, m_data_len);
		}
		m_maxsrcount = maxsrcount;
		m_maxstdcount = maxstdcount;
		m_maxreccount = maxreccount;
		m_maxlivecount = maxlivecount;
		m_maxternumperconf = maxternumperconf;
		m_maxconfnum = maxconfnum;
		m_maxmonitorcount = maxmonitorcount;
		m_maxvoicecount = maxvoicecount;
		m_ver_len = 0;
		m_ver_buf = (SR_uint8*)0;
		m_ver_len = strlen((char*)myversion);
		if (m_ver_len != 0)
		{
			m_ver_buf = new SR_uint8[m_ver_len];
			memcpy(m_ver_buf, myversion, m_ver_len);
		}
		m_std_len = 0;
		m_cstdexpiringdate = (SR_uint8*)0;
		m_std_len = strlen((char*)cstdexpiringdate);
		if (m_std_len != 0)
		{
			m_cstdexpiringdate = new SR_uint8[m_std_len];
			memcpy(m_cstdexpiringdate,cstdexpiringdate,m_std_len);
		}
		m_rec_len = 0;
		m_crecexpiringdate = (SR_uint8*)0;
		m_rec_len = strlen((char*)crecexpiringdate);
		if (m_rec_len != 0)
		{
			m_crecexpiringdate = new SR_uint8[m_rec_len];
			memcpy(m_crecexpiringdate,crecexpiringdate,m_rec_len);
		}
		m_live_len = 0;
		m_cliveexpiringdate = (SR_uint8*)0;
		m_live_len = strlen((char*)cliveexpiringdate);
		if (m_live_len != 0)
		{
			m_cliveexpiringdate = new SR_uint8[m_live_len];
			memcpy(m_cliveexpiringdate,cliveexpiringdate,m_live_len);
		}
		m_monitor_len = 0;
		m_cmonitorexpiringdate = (SR_uint8*)0;
		m_monitor_len = strlen((char*)cmonitorexpiringdate);
		if (m_monitor_len != 0)
		{
			m_cmonitorexpiringdate = new SR_uint8[m_monitor_len];
			memcpy(m_cmonitorexpiringdate,cmonitorexpiringdate,m_monitor_len);
		}
		m_voice_len = 0;
		m_cvoiceexpiringdate = (SR_uint8*)0;
		m_voice_len = strlen((char*)cvoiceexpiringdate);
		if (m_voice_len != 0)
		{
			m_cvoiceexpiringdate = new SR_uint8[m_voice_len];
			memcpy(m_cvoiceexpiringdate,cvoiceexpiringdate,m_voice_len);
		}
	}
	~LicenceInfoData()
	{
		if (m_buf != 0)
		{
			delete[] m_buf;
			m_buf = (SR_uint8*)0;
		}
		m_data_len = 0;

		if (m_ver_buf != 0)
		{
			delete[] m_ver_buf;
			m_ver_buf = (SR_uint8*)0;
		}
		m_ver_len = 0;
		if (m_cstdexpiringdate != 0)
		{
			delete[] m_cstdexpiringdate;
			m_cstdexpiringdate = (SR_uint8*)0;
		}
		m_std_len = 0;
		if (m_crecexpiringdate != 0)
		{
			delete[] m_crecexpiringdate;
			m_crecexpiringdate = (SR_uint8*)0;
		}
		m_rec_len = 0;
		if (m_cliveexpiringdate != 0)
		{
			delete[] m_cliveexpiringdate;
			m_cliveexpiringdate = (SR_uint8*)0;
		}
		m_live_len = 0;
		if (m_cmonitorexpiringdate != 0)
		{
			delete[] m_cmonitorexpiringdate;
			m_cmonitorexpiringdate = (SR_uint8*)0;
		}
		m_monitor_len = 0;
		if (m_cvoiceexpiringdate != 0)
		{
			delete[] m_cvoiceexpiringdate;
			m_cvoiceexpiringdate = (SR_uint8*)0;
		}
		m_voice_len = 0;
	}
	SR_uint8* m_buf;
	uint32_t m_data_len;
	SR_uint32 m_maxsrcount;
	SR_uint32 m_maxstdcount;
	SR_uint32 m_maxreccount;
	SR_uint32 m_maxlivecount;
	SR_uint32 m_maxternumperconf;
	SR_uint32 m_maxconfnum;
	SR_uint8* m_ver_buf;
	uint32_t m_ver_len;
	SR_uint32 m_maxmonitorcount;
	SR_uint32 m_maxvoicecount;
	SR_uint8* m_cstdexpiringdate;
	uint32_t m_std_len;
	SR_uint8* m_crecexpiringdate;
	uint32_t m_rec_len;
	SR_uint8* m_cliveexpiringdate;
	uint32_t m_live_len;
	SR_uint8* m_cmonitorexpiringdate;
	uint32_t m_monitor_len;
	SR_uint8* m_cvoiceexpiringdate;
	uint32_t m_voice_len;
};

//e_sock_recv_data
class SockTcpData : public MessageData
{
public:	
	//explicit SockTcpData(const S_SRMsgHeader* psrmh, SR_socket fd, SR_void* pSocket, char* pbuf) // del by csh
	explicit SockTcpData(const S_SRMsgHeader* psrmh, SR_void* pSocket, char* pbuf)
	{		
		m_buf = new SR_uint8[psrmh->m_data_len];		
		memcpy(m_buf,pbuf,psrmh->m_data_len);		
		memcpy(&m_srmh,psrmh,sizeof(S_SRMsgHeader));		
		//m_sock = fd; // del by csh
		m_pSocket = pSocket;
	}	
	~SockTcpData(){delete[] m_buf;m_buf = (SR_uint8*)0;}	
	S_SRMsgHeader m_srmh;	
	SR_uint8* m_buf;//������ S_SRMsgHeader	
	//SR_socket m_sock;
	SR_void* m_pSocket;
};

class SockRelateData : public MessageData
{
public:	
	enum tagMsgId{
		e_connect_error = e_sock_connect_error,		
		e_connect_ing = e_sock_connect_ing,		
		e_connect_ok = e_sock_connect_ok	
	};	
	explicit SockRelateData(SR_uint32 socktype = 0U,SR_uint32 relatedata = 0U)	
	{		
		m_socktype = socktype;		
		m_relatedata= relatedata;	
	}	
	const SockRelateData& operator=(const SockRelateData& sd)	
	{
		if(this == &sd)
			return *this;
		this->m_socktype = sd.m_socktype;		
		this->m_relatedata = sd.m_relatedata;		
		//this->m_sock = sd.m_sock;	 // del by csh
		this->m_pSocket = sd.m_pSocket;
		return *this;	
	}	
	SockRelateData(const SockRelateData& sd)	
	{		
		this->operator=(sd);	
	}		
	//SR_socket m_sock;//sock��� --> dispatcher������Ϣʱ���	// del by csh
	SR_void* m_pSocket;
	SR_uint32 m_socktype;//socket ����	
	SR_uint32 m_relatedata;//socket �����������
};

//e_sock_rw_error
class SockErrorData : public MessageData
{
public:	
	//// del by csh
	//explicit SockErrorData(SR_socket fd, SR_void* pSocket) :m_errorfd(fd),m_pErrorSocket(pSocket){}
	//SR_socket m_errorfd;
	explicit SockErrorData(SR_void* pSocket) :m_pErrorSocket(pSocket){}
	SR_void* m_pErrorSocket;
private:	
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(SockErrorData);
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(SockErrorData);
};


//e_notify_connected_netmp
class ConnectedNetMPData : public MessageData
{
public:
	//explicit ConnectedNetMPData(SR_uint32 netmpid,SR_socket netmpsockfd,SR_void* pNetmpSocket) // del by csh
	explicit ConnectedNetMPData(SR_uint32 netmpid, SR_uint32 groupid, SR_void* pNetmpSocket, std::list<std::string>& mapinternetiplist)
	{
		m_netmpid = netmpid;
		m_groupid = groupid;
		//m_nemtpsockfd = netmpsockfd; // del by csh
		m_pNetmpSocket = pNetmpSocket;
		m_mapinternetips.clear();
		for (std::list<std::string>::iterator mapip_itor = mapinternetiplist.begin();
			mapip_itor != mapinternetiplist.end(); mapip_itor++)
		{
			if ((*mapip_itor).length() > 0)
			{
				m_mapinternetips.push_back((*mapip_itor));
			}
		}
	}
	~ConnectedNetMPData(){ m_mapinternetips.clear(); }
	SR_uint32 m_netmpid;
	SR_uint32 m_groupid;
	//SR_socket m_nemtpsockfd; // del by csh
	SR_void* m_pNetmpSocket;
	std::list<std::string> m_mapinternetips;
private:
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(ConnectedNetMPData);
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(ConnectedNetMPData);
};

class ConnectedMPData : public MessageData
{
public:
	explicit ConnectedMPData(SR_uint32 mpid, SR_void* pMpSocket)
	{
		m_mpid = mpid;
		m_pMpSocket = pMpSocket;
	}
	~ConnectedMPData(){}
	SR_uint32 m_mpid;
	SR_void* m_pMpSocket;
private:
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(ConnectedMPData);
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(ConnectedMPData);
};

class ConnectedGWData : public MessageData
{
public:
	explicit ConnectedGWData(SR_uint32 gwid, SR_void* pgwSocket)
	{
		m_gwid = gwid;
		m_pGWSocket = pgwSocket;
	}
	~ConnectedGWData(){}
	SR_uint32 m_gwid;
	SR_void* m_pGWSocket;
private:
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(ConnectedGWData);
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(ConnectedGWData);
};

class ConnectedRelayServerData : public MessageData
{
public:
	explicit ConnectedRelayServerData(SR_uint32 rsid, SR_void* prsSocket)
	{
		m_rsid = rsid;
		m_pRSSocket = prsSocket;
	}
	~ConnectedRelayServerData(){}
	SR_uint32 m_rsid;
	SR_void* m_pRSSocket;
private:
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(ConnectedRelayServerData);
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(ConnectedRelayServerData);
};

class ConnectedCRSData : public MessageData
{
public:
	explicit ConnectedCRSData(SR_uint32 crsid, SR_void* pcrsSocket)
	{
		m_crsid = crsid;
		m_pCRSSocket = pcrsSocket;
	}
	~ConnectedCRSData(){}
	SR_uint32 m_crsid;
	SR_void* m_pCRSSocket;
private:
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(ConnectedCRSData);
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(ConnectedCRSData);
};

//e_update_netmpinfo_inconf
class NetMPInfoData : public MessageData
{
public:
	NetMPInfoData(){}
	~NetMPInfoData(){}
	SR_uint32 m_netmpid;
	SR_uint32 m_load;// netmp�Ĵ���
	SR_uint32 m_load2;// netmp�ϵ��ն˸���
	std::list<std::string> m_mapinternetips;
	SR_uint32 m_nettype;// �������ָ�netmp�ǲ������ַ�ʽע���
	SR_uint32 m_max_bandwidth;//netmp�ɸ��ص�������
	SR_uint32 m_max_terms;//netmp�ɸ��ص�����ն���
	std::string   m_ip;// netmp��������ַ
	std::string   m_ipandport;// netmp������port 
};

class MPInfoData : public MessageData
{
public:
	MPInfoData(){}
	~MPInfoData(){}
	SR_uint32 m_mpid;
	SR_uint32 m_load;// mp:���д�����netmp->mp��
	SR_uint32 m_load2;// mp:cpu����
};

class GWInfoData : public MessageData
{
public:
	GWInfoData(){}
	~GWInfoData(){}
	SR_uint32 m_gwid;
	SR_uint32 m_load;// gw:���д�����netmp->mp��
	SR_uint32 m_load2;// gw:�ն˸���
};

class CRSInfoData : public MessageData
{
public:
	CRSInfoData(){}
	~CRSInfoData(){}
	SR_uint32 m_crsid;
	SR_uint32 m_load;// crs:���̿ռ�ʹ����,����16λ��������,��16λ��ʣ������,��λ��GB��
	SR_uint32 m_load2;// crs:cpuʹ����
};

class SRSInfoData : public MessageData
{
public:
	SRSInfoData()
	{
		m_srsid = 0;
		m_strip.clear();
		m_port = 0;
		m_isroot = 0;
		m_addordel = 0;
		m_txbw = 0;
	}
	~SRSInfoData()
	{
		m_srsid = 0;
		m_strip.clear();
		m_port = 0;
		m_isroot = 0;
		m_addordel = 0;
		m_txbw = 0;
	}
	SR_uint32 m_srsid;
	std::string m_strip;
	SR_uint32   m_port;
	SR_uint32 m_isroot;// �Ƿ��Ǹ��ڵ�,0-��,1-��
	SR_uint32 m_addordel;// ���ӻ���ɾ��,1-����,2-ɾ��,3-����ip+port,4-���¸���,5-���ӽڵ��ɸ��ڵ�,6-�ɸ��ڵ����ӽڵ�
	SR_uint32 m_txbw;// SRS�ϴ����Ĵ��� 
};

class RelayServerInfoData : public MessageData
{
public:
	RelayServerInfoData()
	{
		m_relayserverid = 0;
		//m_strip.clear();
		//m_port = 0;
		m_ternum = 0;
		m_tx = 0;
	}
	~RelayServerInfoData()
	{
		m_relayserverid = 0;
		//m_strip.clear();
		//m_port = 0;
		m_ternum = 0;
		m_tx = 0;
	}
	SR_uint32 m_relayserverid;
	//std::string m_strip;
	//SR_uint32   m_port;
	SR_uint32   m_ternum;// relayserver:�����ն���
	SR_uint32   m_tx;// relayserver:����(����)����
};

class RelayMcInfoData : public MessageData
{
public:
	RelayMcInfoData()
	{
		m_relaymcid = 0;
		m_ip = 0;
		m_port = 0;
		m_ternum = 0;
		m_groupid = 0;
		m_mapinternetips.clear();
		m_addordel = 0;
	}
	~RelayMcInfoData()
	{
		m_relaymcid = 0;
		m_ip = 0;
		m_port = 0;
		m_ternum = 0;
		m_groupid = 0;
		m_mapinternetips.clear();
		m_addordel = 0;
	}
	SR_uint32 m_relaymcid;
	SR_uint32 m_ip;
	SR_uint32 m_port;
	SR_uint32 m_ternum;// relaymc:�����ն���
	SR_uint32 m_groupid;// relaymc���������id
	std::list<std::string> m_mapinternetips;
	SR_uint32 m_addordel;// ���ӻ���ɾ��,1-����,2-ɾ��,3-����ip+port,4-���¸���,5-����ӳ���ַ
};
//e_update_sys_cur_load
class SystemCurLoadData : public MessageData
{
public:
	SystemCurLoadData(){}
	~SystemCurLoadData(){}
	SR_uint32 m_curconfcnt;// ϵͳ��ǰ��������
	SR_uint32 m_curreccnt;// ϵͳ��ǰ¼����·��
	SR_uint32 m_curlivecnt;// ϵͳ��ǰֱ����·��
};
//e_notify_netmpsock_error
class NetMPSockErrorData : public MessageData
{
public:
	NetMPSockErrorData(){}
	~NetMPSockErrorData(){}
	SR_uint32 m_netmpid;
	SR_void* m_pSocket;
};

class MPSockErrorData : public MessageData
{
public:
	MPSockErrorData(){}
	~MPSockErrorData(){}
	SR_uint32 m_mpid;
	SR_void* m_pSocket;
};

class GWSockErrorData : public MessageData
{
public:
	GWSockErrorData(){}
	~GWSockErrorData(){}
	SR_uint32 m_gwid;
	SR_void* m_pSocket;
};

class CRSSockErrorData : public MessageData
{
public:
	CRSSockErrorData(){}
	~CRSSockErrorData(){}
	SR_uint32 m_crsid;
	SR_void* m_pSocket;
};

class RelayServerSockErrorData : public MessageData
{
public:
	RelayServerSockErrorData(){}
	~RelayServerSockErrorData(){}
	SR_uint32 m_relayserverid;
	SR_void* m_pSocket;
};

//update from mc_netmp.proto
class NetMPCreateConfRsp{
public:
	NetMPCreateConfRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_netmpid = 0u;
		m_ip.clear();
		m_audiodatarecvport = 0u;
		m_audioctrlrecvport = 0u;
		m_videodatarecvport = 0u;
		m_videoctrlrecvport = 0u;
		m_video2datarecvport = 0u;
		m_video2ctrlrecvport = 0u;
		m_video3datarecvport = 0u;
		m_video3ctrlrecvport = 0u;
		m_desktopdatarecvport = 0u;
		m_desktopctrlrecvport = 0u;
		
		m_failreason.clear();
		m_localip.clear();
		m_netmpno = 0u;

		m_haserrorcode = false;
		m_errorcode = 0u;
		m_netmp_conf_detail_id = 0ull;
		m_nettype = 0u;
		m_edgeipports.clear();
	}
	~NetMPCreateConfRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_netmpid = 0u;
		m_ip.clear();
		m_audiodatarecvport = 0u;
		m_audioctrlrecvport = 0u;
		m_videodatarecvport = 0u;
		m_videoctrlrecvport = 0u;
		m_video2datarecvport = 0u;
		m_video2ctrlrecvport = 0u;
		m_video3datarecvport = 0u;
		m_video3ctrlrecvport = 0u;
		m_desktopdatarecvport = 0u;
		m_desktopctrlrecvport = 0u;

		m_failreason.clear();
		m_localip.clear();
		m_netmpno = 0u;

		m_haserrorcode = false;
		m_errorcode = 0u;
		m_netmp_conf_detail_id = 0ull;
		m_nettype = 0u;
		for (std::list<IPPortInfo*>::iterator edge_itor = m_edgeipports.begin();
			edge_itor != m_edgeipports.end(); /*edge_itor++*/)
		{
			IPPortInfo* pEdgeIPPort = NULL;
			pEdgeIPPort = (*edge_itor);
			if (pEdgeIPPort != NULL)
			{
				delete pEdgeIPPort;
				pEdgeIPPort = NULL;
			}
			edge_itor = m_edgeipports.erase(edge_itor);
		}
		m_edgeipports.clear();
	}
	SR_bool          m_isok;
	SR_uint64        m_confid;//���ඨ��,�Խ�������
	SR_uint32		 m_netmpid;//���ඨ��,�Խ�������
	std::string      m_ip;//����ip,netmp��������RspMCConfCreateЯ��������ip
	SR_int32         m_audiodatarecvport;
	SR_int32         m_audioctrlrecvport;
	SR_int32         m_videodatarecvport;
	SR_int32         m_videoctrlrecvport;
	SR_int32         m_video2datarecvport;
	SR_int32         m_video2ctrlrecvport;
	SR_int32		 m_video3datarecvport;
	SR_int32 		 m_video3ctrlrecvport;
	SR_int32		 m_desktopdatarecvport;
	SR_int32		 m_desktopctrlrecvport;	
	std::string 	 m_failreason;
	std::string      m_localip;//����ip,netmp��devmgrע��ʱЯ����ip
	SR_uint32        m_netmpno; //mc������netmpԤ����ı��

	SR_bool          m_haserrorcode;// �Ƿ��д�����
	SR_uint32        m_errorcode;// �д�����ʱ��Ŵ�����
	SR_uint64        m_netmp_conf_detail_id;// ��netmp�λ�Ʒ�id
	SR_uint32        m_nettype;// �������ָ�netmp�ǲ������ַ�ʽע���
	std::list<IPPortInfo*> m_edgeipports;// ���netmp�ظ��ı�Ե��ַ��Ϣ
};

class MPCreateMPIRsp{
public:
	MPCreateMPIRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
		m_relatednetmpid = 0u;

		m_select_video.clear();
		m_be_selected.clear();

		m_small_video_tersnum = 0u;
		m_middle_video_tersnum = 0u;
		m_big_video_tersnum = 0u;
		m_bWaitNetmpReopen = false;

		m_haserrorcode = false;
		m_errorcode = 0u;
	}

	~MPCreateMPIRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
		m_relatednetmpid = 0u;

		m_select_video.clear();
		m_be_selected.clear();

		m_small_video_tersnum = 0u;
		m_middle_video_tersnum = 0u;
		m_big_video_tersnum = 0u;
		m_bWaitNetmpReopen = false;

		m_haserrorcode = false;
		m_errorcode = 0u;
	}
	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_mpid;
	SR_uint32		 m_channelid; //MCΪ����ý�崦��ʵ�������ͨ��id
	SR_uint32		 m_relatednetmpid;
	std::string 	 m_failreason;

	//�������� �ն�ѡ����Ƶ,�����ն��˻����
	std::map<SR_uint32, SR_uint32> m_select_video;//terid <-> videosize �������һ��ѡ�������������ն�����
	std::map<SR_uint32, SR_uint32> m_be_selected;//terid <-> videosize
	//���ն˵Ĵ���С����ѡ���Ĵ���
	SR_uint32		m_small_video_tersnum;
	SR_uint32		m_middle_video_tersnum;
	SR_uint32		m_big_video_tersnum;
	SR_bool         m_bWaitNetmpReopen; // �Ի���Ϊ��λ��ͬһ�����飩��mpi���䵽һ��netmp��

	SR_bool          m_haserrorcode;// �Ƿ��д�����
	SR_uint32        m_errorcode;// �д�����ʱ��Ŵ�����
};

class MPDestoryMPIRsp{
public:
	MPDestoryMPIRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
	}

	~MPDestoryMPIRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
	}

	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_mpid;
	SR_uint32		 m_channelid; //MCΪ����ý�崦��ʵ�������ͨ��id
	std::string 	 m_failreason;
	std::string      m_ip;//����ip
	std::string      m_localip;//����ip,devmgr��ȡ��ip
};

class MPCreateScreenRsp{
public:
	MPCreateScreenRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_seqnum = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_relatednetmpid = 0u;
		m_screentype = 0x1; // e_Screen_Type_Mixed = 0x1
		m_failreason.clear();

		m_select_video.clear();
		m_be_selected.clear();

		m_small_video_tersnum = 0u;
		m_middle_video_tersnum = 0u;
		m_big_video_tersnum = 0u;

		m_haserrorcode = false;
		m_errorcode = 0u;
	}

	~MPCreateScreenRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_seqnum = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_relatednetmpid = 0u;
		m_screentype = 0x1; // e_Screen_Type_Mixed = 0x1
		m_failreason.clear();

		m_select_video.clear();
		m_be_selected.clear();

		m_small_video_tersnum = 0u;
		m_middle_video_tersnum = 0u;
		m_big_video_tersnum = 0u;

		m_haserrorcode = false;
		m_errorcode = 0u;
	}
	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_mpid;
	SR_uint32        m_seqnum;
	SR_uint32		 m_channelid; //MCΪ����ý�崦��ʵ�������ͨ��id
	SR_uint32        m_screenid; //����m_channelid�´�������Ļid
	SR_uint32		 m_relatednetmpid;
	SR_uint32		 m_screentype;
	std::string 	 m_failreason;

	//�������� �ն�ѡ����Ƶ,�����ն��˻����
	std::map<SR_uint32, SR_uint32> m_select_video;//terid <-> videosize �������һ��ѡ�������������ն�����
	std::map<SR_uint32, SR_uint32> m_be_selected;//terid <-> videosize
	//���ն˵Ĵ���С����ѡ���Ĵ���
	SR_uint32		m_small_video_tersnum;
	SR_uint32		m_middle_video_tersnum;
	SR_uint32		m_big_video_tersnum;

	SR_bool          m_haserrorcode;// �Ƿ��д�����
	SR_uint32        m_errorcode;// �д�����ʱ��Ŵ�����
};

class MPDestoryScreenRsp{
public:
	MPDestoryScreenRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
	}
	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_mpid;
	SR_uint32		 m_channelid; //MCΪ����ý�崦��ʵ�������ͨ��id
	SR_uint32        m_screenid; //����m_channelid�´�������Ļid
	std::string 	 m_failreason;
	std::string      m_ip;//����ip
	std::string      m_localip;//����ip,devmgr��ȡ��ip
};

class SourceInfo
{
public:
	SourceInfo()
	{
		m_srcid = 0u;
		m_name.clear();
		m_isOn = false;
		m_priority = 0u;
		m_select_video.clear();
		m_be_selected.clear();
		m_be_scr_selected.clear();
		m_last_send_level_count.clear();
		m_last_recvfps = 0u;		
	}
	~SourceInfo()
	{
		m_srcid = 0u;
		m_name.clear();
		m_isOn = false;
		m_priority = 0u;
		m_select_video.clear();
		m_be_selected.clear();
		m_be_scr_selected.clear();
		m_last_send_level_count.clear();
		m_last_recvfps = 0u;			
	}
	SR_uint32     m_srcid;	//Դ���,0-0������ͷ��1- 1��Դ��2-2��Դ������31-31��Դ,Ŀǰ���֧��32��Դ
	std::string   m_name;   //Դ����
	SR_bool 	  m_isOn;	//Դ�Ƿ��
	SR_uint32     m_priority; //Դ�����ȼ�����(����srcid˭������Դ)
	std::map<SR_uint32,SR_uint32> m_select_video;//terid <-> videosize �������һ��ѡ�������������ն�����	
	std::map<SR_uint32,SR_uint32> m_be_selected;//terid <-> videosize 0:���� 1:С�� 2:���� 3:����  2019/5/5 ���� 
	std::map<SR_uint32, SR_uint32> m_be_scr_selected;//screenid <-> videosize	
	std::map<SR_uint32, SR_uint32> m_last_send_level_count;//videosize <-> count	
	SR_uint32 m_last_recvfps; //��һ�ν���֡�� 0:0 1:8 2:16 3:24 4:30 5:60.����ն�/��Ļѡ��û���ѡ�ǿ��ʹ��Ĭ��3:24
protected:
private:
};

class DeviceInfo
{
public:
	DeviceInfo()
	{
		m_type = 0u;
		m_srcinfos.clear();
	}
	~DeviceInfo()
	{
		m_type = 0u;
		for (std::map<SR_uint32, SourceInfo*>::iterator srcinfo_itor = m_srcinfos.begin();
			srcinfo_itor != m_srcinfos.end(); srcinfo_itor++)
		{
			SourceInfo* pSourceInfo = NULL;
			pSourceInfo = srcinfo_itor->second;
			if (pSourceInfo)
			{
				delete pSourceInfo;
				pSourceInfo = NULL;
			}
		}
		m_srcinfos.clear();
	}
	SR_uint32     m_type;// �豸����,0-��Ƶ�豸��1-��Ƶ�豸 
	std::map<SR_uint32, SourceInfo*> m_srcinfos; // key:m_srcid<-->value:SourceInfo*
	
protected:
private:
};

class PresetInfo
{
public:
	PresetInfo()
	{
		m_presetid = 0u;
		m_presetname.clear();
		m_relatedvsrcid = 0u;
	}
	~PresetInfo()
	{
		m_presetid = 0u;
		m_presetname.clear();
		m_relatedvsrcid = 0u;		
	}
	SR_uint32     m_presetid;// Ԥ��λ���
	std::string   m_presetname;// Ԥ��λ����
	SR_uint32     m_relatedvsrcid;
protected:
private:
};

class SeleteTermInfo
{
public:
	SeleteTermInfo()
	{
		m_terid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_videosize = 0u;
		m_recvfps = 3;		
		m_srcid = 0u;
	}
	~SeleteTermInfo()
	{
		m_terid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_videosize = 0u;
		m_recvfps = 3;
		m_srcid = 0u;
	}
	SR_uint32 m_terid;// ��ѡ�����ն�id����mpý�崦��ʵ����ͨ��id
	SR_uint32 m_channelid;// ��ѡ�����ն�ͨ��id����mpý�崦��ʵ����ͨ��id
	SR_uint32 m_screenid;// ��ѡ�������ն�������0,��ѡ������mpý�崦��ʵ��������������Ļid������0-����ѡ����Ӧ����Ļ��
	SR_uint32 m_videosize; // �������Ĵ�С��0�����������㣺�ֱ��ʴ�С(1:160x90��2:256x144��3:320x180��4:384x216��5:432x240��6:480x270��7:640x360��8:864x480��9:960x540��10:1280x720��11:1920x1080)
	SR_uint32 m_recvfps; // ������֡�ʴ�С��0:0 1:8 2:16 3:24 4:30 5:60.����ն�/��Ļѡ��û���ѡ�͸����netmpǿ��ʹ��Ĭ��3:24.
	SR_uint32 m_srcid;   // �ն���ƵԴ���,0-0��Դ��1- 1��Դ��2-2��Դ������31-31��Դ,���֧��32��Դ
protected:
private:
};

class MPSeleteVideoCmd{
public:
	MPSeleteVideoCmd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_mapSeleteTermInfo.clear();
	}

	~MPSeleteVideoCmd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_mapSeleteTermInfo.clear();
	}
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;//������Ϣ��mc_deviceid
	SR_uint32		 m_mpid;//����������������ѡ������mp deviceid
	SR_uint32		 m_channelid;//����������������ѡ�����ڻ���ý�崦��ʵ��ͨ��id
	SR_uint32        m_screenid;//����������������ѡ����������Ļid
	std::map<SR_uint32, SeleteTermInfo> m_mapSeleteTermInfo;

	//message CmdMPSeleteVideo
	//{
	//	message TermInfo
	//	{
	//		optional uint32 terid = 1;
	//		optional uint32 channelid = 2;// �ն�ͨ��id
	//		optional uint32 videosize = 3; // 0:ֹͣѡ�� 1:С�� 2:���� 3:���� 
	//	}
	//	optional uint64		confid = 1;
	//	optional uint32		mcid = 2;//������Ϣ��mc_deviceid
	//	optional uint32     mpid = 3;//�������������ն�����mp deviceid
	//	optional uint32 	channelid = 4;//���������ն�����ý�崦��ʵ��ͨ��id
	//	optional uint32 	screenid = 5;//���������ն�������Ļid 
	//	repeated TermInfo  	seleteterinfo = 6;
	//}
};

class PaneInfo
{
public:
	PaneInfo()
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

	~PaneInfo()
	{
		m_paneindex = 0u;
		m_contenttype = 0u;
		m_optype = 1u; // ���� 0:�Զ�;����Ĭ��ֵ1:ָ���ն�;
		m_polltime = 10u;
		m_terid = 0u;
		m_terchannelid = 0u;
		m_tername.clear();
		for (std::map<SR_uint32, Autopollinfo*>::iterator itor = m_autopollinfo.begin();
			itor != m_autopollinfo.end(); itor++)
		{
			Autopollinfo* autopllinfo = NULL;
			autopllinfo = itor->second;
			if (autopllinfo != NULL)
			{
				delete autopllinfo;
				autopllinfo = NULL;
			}
		}
		m_autopollinfo.clear();
		m_vsrcid = -1;
	}

	SR_uint32 m_paneindex;//����id
	SR_uint32 m_contenttype;//��������ʾ�������ͣ�0:�ն�; 1:voipλͼ; 2:��; 3:˫��
	SR_uint32 m_optype;//�����в������ͣ�0:�Զ�; 1:ָ���ն�; 2:��������;3:�Զ���Ѳ.
	SR_uint32 m_polltime;//��������е���ʾ����Ϊ��3:�Զ���Ѳ������˴������Ϊ��ѯʱ��������λ����
	SR_uint32 m_terid;//�������������ն˶�Ӧ��id 
	SR_uint32 m_terchannelid;//�������������ն˶�Ӧ��ͨ��id
	std::string m_tername;//�������������ն˶�Ӧ������
	std::map<SR_uint32, Autopollinfo*> m_autopollinfo;//�Զ���ѯ�б���Ϣ apid<--->Autopollinfo
	SR_int32  m_vsrcid;

protected:
private:
};

class LayoutInfo
{
public:
	LayoutInfo()
	{
		m_screenid = 0u;
		//m_screentype = e_Screen_Type_Mixed;
		//m_layoutmode = e_Layout_Mode_Auto;
		m_screentype = 1;
		m_layoutmode = 0;
		m_chairfollow = 1;
		m_paneinfos.clear();
	}
	~LayoutInfo()
	{
		m_screenid = 0u;
		//m_screentype = e_Screen_Type_Mixed;
		//m_layoutmode = e_Layout_Mode_Auto;
		m_screentype = 1;
		m_layoutmode = 0;
		m_chairfollow = 1;
		for (std::map<SR_uint32, PaneInfo*>::iterator itor = m_paneinfos.begin();
			itor != m_paneinfos.end(); itor++)
		{
			PaneInfo* ppinfo = NULL;
			ppinfo = itor->second;
			if (ppinfo != NULL)
			{
				delete ppinfo;
			}
			ppinfo = NULL;
		}

		//for (std::set<PaneInfo*>::iterator itor = m_paneinfos.begin();
		//	itor != m_paneinfos.end(); itor++)
		//{
		//	PaneInfo* ppinfo = NULL;
		//	ppinfo = *itor;
		//	if (ppinfo != NULL)
		//	{
		//		delete ppinfo;
		//	}
		//	ppinfo = NULL;
		//}
		m_paneinfos.clear();
	}

	SR_uint32 m_screenid;// �û������ֶ�Ӧ��Ļid
	SR_uint32 m_screentype;// �û������ֶ�Ӧ��Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ
	SR_uint32 m_layoutmode; // ��������ģʽ��0-�Զ���1-������2-2������3-3������4-4����������11-13������12-16������13-¼������14-20������15-25����
	std::map<SR_uint32, PaneInfo*> m_paneinfos;// paneindex<-->PaneInfo
	SR_uint32 m_chairfollow;
	//std::set<PaneInfo*> m_paneinfos;

protected:
private:
};

class MPLayoutinfoInd{
public:
	MPLayoutinfoInd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
		m_layoutinfos.clear();
	}

	~MPLayoutinfoInd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_mpid = 0u;
		m_channelid = 0u;
		for (std::set<LayoutInfo*>::iterator itor = m_layoutinfos.begin();
			itor != m_layoutinfos.end(); itor++)
		{
			LayoutInfo* pliinfo = NULL;
			pliinfo = *itor;
			if (pliinfo != NULL)
			{
				delete pliinfo;
			}
			pliinfo = NULL;
		}
		m_layoutinfos.clear();
	}
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;//������Ϣ��mc_deviceid
	SR_uint32		 m_mpid;//����������������ѡ������mp deviceid
	SR_uint32		 m_channelid;//����������������ѡ�����ڻ���ý�崦��ʵ��ͨ��id
	//std::set<SR_uint32, LayoutInfo> m_layoutinfos;
	std::set<LayoutInfo*> m_layoutinfos;

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
};

class CRSCreateConfRsp{
public:
	CRSCreateConfRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_seqnum = 0u;
		m_crschannelid = 0u;
		m_relatednetmpid = 0u;
		m_failreason.clear();

		m_select_video.clear();
		m_be_selected.clear();

		m_haserrorcode = false;
		m_errorcode = 0u;
	}

	~CRSCreateConfRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_seqnum = 0u;
		m_crschannelid = 0u;
		m_relatednetmpid = 0u;
		m_failreason.clear();

		m_select_video.clear();
		m_be_selected.clear();

		m_haserrorcode = false;
		m_errorcode = 0u;
	}
	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_crsid;
	SR_uint32        m_seqnum;
	SR_uint32		 m_crschannelid; //MCΪCRS�����ͨ��id
	SR_uint32		 m_relatednetmpid;
	std::string 	 m_failreason;

	//�������� �ն�ѡ����Ƶ,�����ն��˻����
	std::map<SR_uint32, SR_uint32> m_select_video;//terid <-> videosize �������һ��ѡ�������������ն�����
	std::map<SR_uint32, SR_uint32> m_be_selected;//terid <-> videosize

	SR_bool          m_haserrorcode;// �Ƿ��д�����
	SR_uint32        m_errorcode;// �д�����ʱ��Ŵ�����
};
class CompLicenceInfo
{
public:
	CompLicenceInfo()
	{
		m_licencetype = 0u;
		m_licencenum = 0u;
		m_starttime.clear();
		m_exptime.clear();
	}
	~CompLicenceInfo()
	{
		m_licencetype = 0u;
		m_licencenum = 0u;
		m_starttime.clear();
		m_exptime.clear();
	}
	SR_uint32 m_licencetype;//��Ȩ���ͣ�0-δ֪��1-������Ȩ��2-SR�ն���Ȩ��3-��׼�ն���Ȩ������TER,MCU��΢�ţ�STREAM_LIVE��RTSP��,TRUNK��sip trunk������4-�����Ȩ���������ʵʱ��,���¼�񣩡�5-������Ȩ������PSTN����6-ֱ����Ȩ��7-¼����Ȩ��8-����ʶ����Ȩ��9-����ʶ����Ȩ
	SR_int32 m_licencenum;//��Ȩ��
	std::string m_starttime;//��Ȩ��ʼʱ��
	std::string m_exptime;//��Ȩ����ʱ��
};
class CompanyInfo
{
public:
	CompanyInfo()
	{
		m_compid = 0u;
		m_compname.clear();
		m_complicenceinfos.clear();
	}
	~CompanyInfo()
	{
		m_compid = 0u;
		m_compname.clear();
		for (std::map<SR_uint32, CompLicenceInfo*>::iterator itor_complic = m_complicenceinfos.begin();
			itor_complic != m_complicenceinfos.end(); ++itor_complic)
		{
			CompLicenceInfo* pcomplicenceinfo = itor_complic->second;
			if (pcomplicenceinfo)
				delete pcomplicenceinfo;
		}
		m_complicenceinfos.clear();
	}
	SR_uint32 m_compid;
	std::string m_compname;
	std::map<SR_uint32, CompLicenceInfo*> m_complicenceinfos;
};
class LicenseToMCRsp
{
public:
	LicenseToMCRsp()
	{
		m_isok = false;
		m_seqnumrsp = 0u;
		m_confid = 0u;
		m_rspcompinfos.clear();
	};
	~LicenseToMCRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_seqnumrsp = 0u;
		for (std::map<SR_uint32, CompanyInfo*>::iterator itor_comp = m_rspcompinfos.begin();
			itor_comp != m_rspcompinfos.end(); ++itor_comp)
		{
			CompanyInfo* pcompinfo = itor_comp->second;
			if (pcompinfo)
				delete pcompinfo;
		}
		m_rspcompinfos.clear();
	};
	SR_bool m_isok;
	SR_uint32 m_seqnumrsp;
	SR_uint64 m_confid;//�����
	std::map<SR_uint32, CompanyInfo*> m_rspcompinfos;
};
class CompLicenceInfoData : public MessageData
{
public:
	explicit CompLicenceInfoData(LicenseToMCRsp* cfdm)
	{
		m_complicenceinfo = cfdm;
	}
	~CompLicenceInfoData(){}//do not delete m_confinfo here!!!
	LicenseToMCRsp* m_complicenceinfo;//LicenseToMCRsp*
};
class TotalCompLicenceInfoData : public MessageData
{
public:
	explicit TotalCompLicenceInfoData(CompanyInfo* cfdm)
	{
		m_complicenceinfo = cfdm;
	}
	~TotalCompLicenceInfoData(){}//do not delete m_confinfo here!!!
	CompanyInfo* m_complicenceinfo;//CompanyInfo*
};
class CRSStartRecRsp{
public:
	CRSStartRecRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_filestorsvrip.clear();
		m_filestoragepath.clear();
		m_sdefilepath.clear();
		m_failreason.clear();
		m_filerootpath.clear();
		m_seqnum = 0u;

		m_haserrorcode = false;
		m_errorcode = 0u;
	}

	~CRSStartRecRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_filestorsvrip.clear();
		m_filestoragepath.clear();
		m_sdefilepath.clear();
		m_failreason.clear();
		m_filerootpath.clear();
		m_seqnum = 0u;

		m_haserrorcode = false;
		m_errorcode = 0u;
	}
	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_crsid;
	SR_uint32		 m_crschannelid; //��ʼ¼��crsͨ��id
	SR_uint32		 m_channelid;//CRS¼�Ƶ�MPI��Ӧͨ��id�����ն˶�Ӧ��ͨ��id
	SR_uint32		 m_screenid; //CRS¼�Ƶ���Ļid,���channelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_screentype;//CRS¼�Ƶ���Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ,���channelid���ն�ͨ��id,��ѡ����Ժ���
	std::string 	 m_filestorsvrip;//CRS¼���ļ���ŷ����ip
	std::string 	 m_filestoragepath; //CRS¼���ļ�������·��
	std::string 	 m_sdefilepath; //CRS¼�Ƶļ����ļ�������·��
	std::string 	 m_failreason;
	std::string 	 m_filerootpath; //CRS¼�Ƶļ����ļ���Ÿ�·��
	SR_uint32		 m_seqnum;
	
	SR_bool          m_haserrorcode;// �Ƿ��д�����
	SR_uint32        m_errorcode;// �д�����ʱ��Ŵ�����
};

class CRSStopRecRsp{
public:
	CRSStopRecRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_filestorsvrip.clear();
		m_filestoragepath.clear();
		m_sdefilepath.clear();
		m_failreason.clear();
		m_filerootpath.clear();
		m_seqnum = 0u;
		m_filesize = 0u;

		m_haserrorcode = false;
		m_errorcode = 0u;
	}

	~CRSStopRecRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_filestorsvrip.clear();
		m_filestoragepath.clear();
		m_sdefilepath.clear();
		m_failreason.clear();
		m_filerootpath.clear();
		m_seqnum = 0u;
		m_filesize = 0u;

		m_haserrorcode = false;
		m_errorcode = 0u;
	}
	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_crsid;
	SR_uint32		 m_crschannelid; //��ʼ¼��crsͨ��id
	SR_uint32		 m_channelid;//CRS¼�Ƶ�MPI��Ӧͨ��id�����ն˶�Ӧ��ͨ��id
	SR_uint32		 m_screenid; //CRS¼�Ƶ���Ļid,���channelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_screentype;//CRS¼�Ƶ���Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ,���channelid���ն�ͨ��id,��ѡ����Ժ���
	std::string 	 m_filestorsvrip;//CRS¼���ļ���ŷ����ip
	std::string 	 m_filestoragepath; //CRS¼���ļ�������·��
	std::string 	 m_sdefilepath; //CRS¼�Ƶļ����ļ�������·��
	std::string 	 m_failreason;
	std::string 	 m_filerootpath; //CRS¼�Ƶļ����ļ���Ÿ�·��
	SR_uint32		 m_seqnum;
	SR_uint64		 m_filesize;

	SR_bool          m_haserrorcode;// �Ƿ��д�����
	SR_uint32        m_errorcode;// �д�����ʱ��Ŵ�����
};

class CRSSeleteVideoCmd{
public:
	CRSSeleteVideoCmd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_mapSeleteTermInfo.clear();
	}

	~CRSSeleteVideoCmd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_mapSeleteTermInfo.clear();
	}
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;//������Ϣ��mc_deviceid
	SR_uint32		 m_crsid;//����ѡ��CRS��crs_deviceid
	SR_uint32		 m_crschannelid;//����ѡ��CRS��ͨ��id
	std::map<SR_uint32, SeleteTermInfo> m_mapSeleteTermInfo;
};

class CRSFileStorPathInd{
public:
	CRSFileStorPathInd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_filestorsvrip.clear();
		m_filestoragepath.clear();
		m_sdefilepath.clear();
		m_filerootpath.clear();

		m_recordstate = 0u;
		m_filesize = 0u;
	}

	~CRSFileStorPathInd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_filestorsvrip.clear();
		m_filestoragepath.clear();
		m_sdefilepath.clear();
		m_filerootpath.clear();

		m_recordstate = 0u;
		m_filesize = 0u;
	}
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_crsid;
	SR_uint32		 m_crschannelid; //��ʼ¼��crsͨ��id
	SR_uint32		 m_channelid;//CRS¼�Ƶ�MPI��Ӧͨ��id�����ն˶�Ӧ��ͨ��id
	SR_uint32		 m_screenid; //CRS¼�Ƶ���Ļid,���channelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_screentype;//CRS¼�Ƶ���Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ,���channelid���ն�ͨ��id,��ѡ����Ժ���
	std::string 	 m_filestorsvrip;//CRS¼���ļ���ŷ����ip
	std::string 	 m_filestoragepath; //CRS¼���ļ�������·��
	std::string 	 m_sdefilepath; //CRS¼�Ƶļ����ļ�������·��
	std::string 	 m_filerootpath; //CRS¼�Ƶļ����ļ���Ÿ�·��
	SR_uint32		 m_recordstate;
	SR_uint64		 m_filesize;
};

class CRSStartLiveRsp{
public:
	CRSStartLiveRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_relatedsrsid = 0u;
		m_failreason.clear();
		m_seqnum = 0u;
		m_liveurl.clear();
		m_livesvrtype = 0u;
		m_liveaddrs.clear();

		m_haserrorcode = false;
		m_errorcode = 0u;
	}

	~CRSStartLiveRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_relatedsrsid = 0u;
		m_failreason.clear();
		m_seqnum = 0u;
		m_liveurl.clear();
		m_livesvrtype = 0u;
		m_liveaddrs.clear();

		m_haserrorcode = false;
		m_errorcode = 0u;
	}
	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_crsid;
	SR_uint32		 m_crschannelid; //��ʼ¼��crsͨ��id
	SR_uint32		 m_channelid;//CRS¼�Ƶ�MPI��Ӧͨ��id�����ն˶�Ӧ��ͨ��id
	SR_uint32		 m_screenid; //CRS¼�Ƶ���Ļid,���channelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_screentype;//CRS¼�Ƶ���Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ,���channelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_relatedsrsid;
	std::string 	 m_failreason;
	SR_uint32		 m_seqnum;
	std::string 	 m_liveurl;
	SR_uint32		 m_livesvrtype;
	std::map<SR_uint32, std::string> m_liveaddrs;

	SR_bool          m_haserrorcode;// �Ƿ��д�����
	SR_uint32        m_errorcode;// �д�����ʱ��Ŵ�����
};

class CRSLiveStateInd{
public:
	CRSLiveStateInd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_livestate = 0u;
		m_statecont.clear();

		m_haserrorcode = false;
		m_errorcode = 0u;
	}

	~CRSLiveStateInd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_crsid = 0u;
		m_crschannelid = 0u;
		m_channelid = 0u;
		m_screenid = 0u;
		m_screentype = 0u;
		m_livestate = 0u;
		m_statecont.clear();

		m_haserrorcode = false;
		m_errorcode = 0u;
	}
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_crsid;
	SR_uint32		 m_crschannelid; //��ʼֱ��crsͨ��id
	SR_uint32		 m_channelid;//CRSֱ����MPI��Ӧͨ��id�����ն˶�Ӧ��ͨ��id
	SR_uint32		 m_screenid; //CRSֱ������Ļid,���channelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_screentype;//CRSֱ������Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ��4-¼��ֱ����Ļ,���channelid���ն�ͨ��id,��ѡ����Ժ���
	SR_uint32		 m_livestate;//ֱ��״̬,1-ֱ���ɹ���2-ֱ��ʧ��
	std::string 	 m_statecont; //��ֱ��ʧ�ܣ�������ʧ������

	SR_bool          m_haserrorcode;// �Ƿ��д�����
	SR_uint32        m_errorcode;// �д�����ʱ��Ŵ�����
};

//class CRSStopLiveRsp{
//public:
//	CRSStopLiveRsp()
//	{
//		m_isok = false;
//		m_confid = 0u;
//		m_mcid = 0u;
//		m_crsid = 0u;
//		m_crschannelid = 0u;
//		m_channelid = 0u;
//		m_screenid = 0u;
//		m_screentype = 0u;
//		m_failreason.clear();
//		m_seqnum = 0u;
//		m_liveurl.clear();
//	}
//
//	~CRSStopLiveRsp()
//	{
//		m_isok = false;
//		m_confid = 0u;
//		m_mcid = 0u;
//		m_crsid = 0u;
//		m_crschannelid = 0u;
//		m_channelid = 0u;
//		m_screenid = 0u;
//		m_screentype = 0u;
//		m_failreason.clear();
//		m_seqnum = 0u;
//		m_liveurl.clear();
//	}
//	SR_bool          m_isok;
//	SR_uint64        m_confid;
//	SR_uint32		 m_mcid;
//	SR_uint32		 m_crsid;
//	SR_uint32		 m_crschannelid; //��ʼ¼��crsͨ��id
//	SR_uint32		 m_channelid;//CRS¼�Ƶ�MPI��Ӧͨ��id�����ն˶�Ӧ��ͨ��id
//	SR_uint32		 m_screenid; //CRS¼�Ƶ���Ļid,���channelid���ն�ͨ��id,��ѡ����Ժ���
//	SR_uint32		 m_screentype;//CRS¼�Ƶ���Ļ���ͣ�1-����(��׼�ն�)��Ļ��2-�ն�ת����Ļ��3-˫����Ļ,���channelid���ն�ͨ��id,��ѡ����Ժ���
//	std::string 	 m_failreason;
//	SR_uint32		 m_seqnum;
//	std::string 	 m_liveurl;
//};

class MonitorInfo{
public:
	MonitorInfo()
	{
		m_suid = 0u;
		m_nickname.clear();
		m_ip.clear();
		m_devicecode.clear();
		m_querystate = 0u;
		m_playbackparm.clear();
		m_totaltimespan.m_starttime.clear();
		m_totaltimespan.m_endtime.clear();
		m_subtimespans.clear();
	}

	~MonitorInfo()
	{
		m_suid = 0u;
		m_nickname.clear();
		m_ip.clear();
		m_devicecode.clear();
		m_querystate = 0u;
		m_playbackparm.clear();
		m_totaltimespan.m_starttime.clear();
		m_totaltimespan.m_endtime.clear();
		for (std::list<TimeInfo*>::iterator subts_itor = m_subtimespans.begin();
			subts_itor != m_subtimespans.end(); subts_itor++)
		{
			TimeInfo* pti = NULL;
			pti = (*subts_itor);
			if (pti != NULL)
			{
				delete pti;
				pti = NULL;
			}
		}
		m_subtimespans.clear();
	}
	SR_uint32		 m_suid;
	std::string 	 m_nickname;
	std::string 	 m_ip;
	std::string 	 m_devicecode;
	SR_uint32		 m_querystate;// ��ѯ���
	std::string 	 m_playbackparm;// ���Ž��Ȳ���
	TimeInfo         m_totaltimespan;// ��ʱ�����Ϣ
	std::list<TimeInfo*> m_subtimespans;// ��ʱ�����Ϣ
};

class GWQueryMonitorRecInd{
public:
	GWQueryMonitorRecInd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_gwid = 0u;
		m_queryinfos.clear();
	}

	~GWQueryMonitorRecInd()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_gwid = 0u;
		for (std::list<MonitorInfo*>::iterator monitor_itor = m_queryinfos.begin();
			monitor_itor != m_queryinfos.end(); monitor_itor++)
		{
			MonitorInfo* pmonitor = NULL;
			pmonitor = (*monitor_itor);
			if (pmonitor != NULL)
			{
				delete pmonitor;
				pmonitor = NULL;
			}
		}
		m_queryinfos.clear();
	}
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_gwid;
	std::list<MonitorInfo*> m_queryinfos;
};

class RelaySvrTJCRsp{
public:
	RelaySvrTJCRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_relaysvrid = 0u;
		m_suid = 0u;
		m_failreason.clear();
		//m_mapSvrAddrInfo.clear();
		
		m_haserrorcode = false;
		m_errorcode = 0u;
	}

	~RelaySvrTJCRsp()
	{
		m_isok = false;
		m_confid = 0u;
		m_mcid = 0u;
		m_relaysvrid = 0u;
		m_suid = 0u;
		m_failreason.clear();

		//for (std::map<SR_uint32, AddrInfo*>::iterator svr_addr_itor = m_mapSvrAddrInfo.begin();
		//	svr_addr_itor != m_mapSvrAddrInfo.end(); svr_addr_itor++)
		//{
		//	AddrInfo* pAddrinfo = NULL;
		//	pAddrinfo = svr_addr_itor->second;
		//	if (pAddrinfo)
		//	{
		//		delete pAddrinfo;
		//		pAddrinfo = NULL;
		//	}
		//}
		//m_mapSvrAddrInfo.clear();

		m_haserrorcode = false;
		m_errorcode = 0u;
	}
	SR_bool          m_isok;
	SR_uint64        m_confid;
	SR_uint32		 m_mcid;
	SR_uint32		 m_relaysvrid;
	SR_uint32		 m_suid;
	std::string 	 m_failreason;
	//std::map<SR_uint32, AddrInfo*> m_mapSvrAddrInfo;
	IPPortInfo m_relaysvraddr;// relayserver��������ַ
	
	SR_bool          m_haserrorcode;// �Ƿ��д�����
	SR_uint32        m_errorcode;// �д�����ʱ��Ŵ�����
};

class TSFrameInfo
{
public:
	TSFrameInfo()
	{
		m_fsw = 0u;
		m_fsh = 0u;
		m_fps = 0u;
	}
	~TSFrameInfo()
	{
		m_fsw = 0u;
		m_fsh = 0u;
		m_fps = 0u;
	}

	SR_uint32 m_fsw;// �ֱ��� �� frame size width
	SR_uint32 m_fsh;// �ֱ��� �� frame size height
	SR_uint32 m_fps;// ֡��

private:

};

class TSAdvanceInfo
{
public:
	TSAdvanceInfo()
	{
		m_delay = 0u;
		m_jitter = 0u;
		m_loss = 0u;
	}
	~TSAdvanceInfo()
	{
		m_delay = 0u;
		m_jitter = 0u;
		m_loss = 0u;
	}

	SR_uint32 m_delay; // ��ʱ,��λms
	SR_uint32 m_jitter;// ����,��λms
	SR_uint32 m_loss;// ������

private:

};

class TSBaseInfo
{
public:
	TSBaseInfo()
	{
		m_format = 0u;
		m_bitrate = 0u;
	}
	~TSBaseInfo()
	{
		m_format = 0u;
		m_bitrate = 0u;
	}

	SR_uint32 m_format;// ý���ʽ,�ο�SRMediaCommon.h������Ƶ��ʽ����
	SR_uint32 m_bitrate; // ����,��λkbps

private:

};

class TSAudioInfo
{
public:
	TSAudioInfo()
	{
		m_atype = 0u;
		m_aterid = 0u;
		m_baseinfo.m_format = 0u;
		m_baseinfo.m_bitrate = 0u;
		m_advinfo.m_delay = 0u;
		m_advinfo.m_jitter = 0u;
		m_advinfo.m_loss = 0u;
	}
	~TSAudioInfo()
	{
		m_atype = 0u;
		m_aterid = 0u;
		m_baseinfo.m_format = 0u;
		m_baseinfo.m_bitrate = 0u;
		m_advinfo.m_delay = 0u;
		m_advinfo.m_jitter = 0u;
		m_advinfo.m_loss = 0u;
	}

	SR_uint32 m_atype;//��Ƶ����,0-����Ƶ��1-������Ƶ(Ŀǰ����������Ƶͨ��,������ͨ��ʱ�ټ�)
	SR_uint32 m_aterid;//����ն�terid
	TSBaseInfo  m_baseinfo;//��Ƶ������Ϣ
	TSAdvanceInfo  m_advinfo;//��Ƶ�߼���Ϣ

private:

};
class TSVideoInfo
{
public:
	TSVideoInfo()
	{
		m_vtype = 0u;
		m_vterid = 0u;
		m_baseinfo.m_format = 0u;
		m_baseinfo.m_bitrate = 0u;
		m_advinfo.m_delay = 0u;
		m_advinfo.m_jitter = 0u;
		m_advinfo.m_loss = 0u;
		m_frameinfo.clear();
	}
	~TSVideoInfo()
	{
		m_vtype = 0u;
		m_vterid = 0u;
		m_baseinfo.m_format = 0u;
		m_baseinfo.m_bitrate = 0u;
		m_advinfo.m_delay = 0u;
		m_advinfo.m_jitter = 0u;
		m_advinfo.m_loss = 0u;

		for (std::list<TSFrameInfo*>::iterator fi_itor = m_frameinfo.begin();
			fi_itor != m_frameinfo.end(); fi_itor++)
		{
			TSFrameInfo* ptsfi = NULL;
			ptsfi = (*fi_itor);
			if (ptsfi != NULL)
			{
				delete ptsfi;
				ptsfi = NULL;
			}
		}
		m_frameinfo.clear();
	}

	SR_uint32 m_vtype;//��Ƶ����,0-������1-����(˫������)
	SR_uint32 m_vterid;//����ն�terid
	TSBaseInfo  m_baseinfo;//��Ƶ������Ϣ
	TSAdvanceInfo  m_advinfo;//��Ƶ�߼���Ϣ
	std::list<TSFrameInfo*> m_frameinfo;//���ֱ��ʺ�֡�ʵ���Ϣ(Ŀǰֻ�����ֱ����ǲ�)

private:

};
class TSXfeInfo
{
public:
	TSXfeInfo()
	{
		m_ainfo.clear();
		m_vinfo.clear();
	}
	~TSXfeInfo()
	{
		for (std::list<TSAudioInfo*>::iterator ai_itor = m_ainfo.begin();
			ai_itor != m_ainfo.end(); ai_itor++)
		{
			TSAudioInfo* ptsai = NULL;
			ptsai = (*ai_itor);
			if (ptsai != NULL)
			{
				delete ptsai;
				ptsai = NULL;
			}
		}
		m_ainfo.clear();
		for (std::list<TSVideoInfo*>::iterator vi_itor = m_vinfo.begin();
			vi_itor != m_vinfo.end(); vi_itor++)
		{
			TSVideoInfo* ptsai = NULL;
			ptsai = (*vi_itor);
			if (ptsai != NULL)
			{
				delete ptsai;
				ptsai = NULL;
			}
		}
		m_vinfo.clear();
	}

	std::list<TSAudioInfo*> m_ainfo;//��Ƶ��Ϣ
	std::list<TSVideoInfo*> m_vinfo;//��Ƶ��Ϣ

private:

};
class TerStatisticsInfo
{
public:
	TerStatisticsInfo()
	{
		m_terid = 0u;
		m_txbweresult = 0u;
		m_txinfo.clear();
		m_rxinfo.clear();
	}
	~TerStatisticsInfo()
	{
		m_terid = 0u;
		m_txbweresult = 0u;
		for (std::list<TSXfeInfo*>::iterator txi_itor = m_txinfo.begin();
			txi_itor != m_txinfo.end(); txi_itor++)
		{
			TSXfeInfo* ptsxfei = NULL;
			ptsxfei = (*txi_itor);
			if (ptsxfei != NULL)
			{
				delete ptsxfei;
				ptsxfei = NULL;
			}
		}
		m_txinfo.clear();
		for (std::list<TSXfeInfo*>::iterator rxi_itor = m_rxinfo.begin();
			rxi_itor != m_rxinfo.end(); rxi_itor++)
		{
			TSXfeInfo* ptsxfei = NULL;
			ptsxfei = (*rxi_itor);
			if (ptsxfei != NULL)
			{
				delete ptsxfei;
				ptsxfei = NULL;
			}
		}
		m_rxinfo.clear();
	}

	SR_uint32 m_terid;//��ͳ���ն�terid
	std::list<TSXfeInfo*> m_txinfo;//����˸���ͳ���ն˷��͵�ͳ����Ϣ(��֧�ֶ�·���)
	std::list<TSXfeInfo*> m_rxinfo;//����˽��ձ�ͳ���ն˵�ͳ����Ϣ
	SR_uint32 m_txbweresult;//����˸���ͳ���ն˷��͵Ĵ����������,0-unknown��1-�ϲ2-һ�㡢3-����

private:

};

class NetmpIndTerStatisticsInfo{
public:
	NetmpIndTerStatisticsInfo()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_netmpid = 0u;
		m_tsinfos.clear();
	}

	~NetmpIndTerStatisticsInfo()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_netmpid = 0u;
		for (std::list<TerStatisticsInfo*>::iterator tsi_itor = m_tsinfos.begin();
			tsi_itor != m_tsinfos.end(); tsi_itor++)
		{
			TerStatisticsInfo* ptsi = NULL;
			ptsi = (*tsi_itor);
			if (ptsi != NULL)
			{
				delete ptsi;
				ptsi = NULL;
			}
		}
		m_tsinfos.clear();
	}
	SR_uint64 m_confid;
	SR_uint32 m_mcid;
	SR_uint32 m_netmpid;
	std::list<TerStatisticsInfo*> m_tsinfos;
};

class GWIndTerStatisticsInfo{
public:
	GWIndTerStatisticsInfo()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_gwid = 0u;
		m_tsinfos.clear();
	}

	~GWIndTerStatisticsInfo()
	{
		m_confid = 0u;
		m_mcid = 0u;
		m_gwid = 0u;
		for (std::list<TerStatisticsInfo*>::iterator tsi_itor = m_tsinfos.begin();
			tsi_itor != m_tsinfos.end(); tsi_itor++)
		{
			TerStatisticsInfo* ptsi = NULL;
			ptsi = (*tsi_itor);
			if (ptsi != NULL)
			{
				delete ptsi;
				ptsi = NULL;
			}
		}
		m_tsinfos.clear();
	}
	SR_uint64 m_confid;
	SR_uint32 m_mcid;
	SR_uint32 m_gwid;
	std::list<TerStatisticsInfo*> m_tsinfos;
};

//e_rspnetmpcreateconf
class NetMPCreateConfInfoData : public MessageData
{
public:
	explicit NetMPCreateConfInfoData(NetMPCreateConfRsp* pnccr)
	{
		m_netmpcreateconfrsp = pnccr;
	}
	~NetMPCreateConfInfoData(){}//do not delete m_netmpcreateconfrsp here!!!
	NetMPCreateConfRsp* m_netmpcreateconfrsp;
};

class MPCreateMPIInfoData : public MessageData
{
public:
	explicit MPCreateMPIInfoData(MPCreateMPIRsp* pmcmr)
	{
		m_mpcreatempirsp = pmcmr;
	}
	~MPCreateMPIInfoData(){}
	MPCreateMPIRsp* m_mpcreatempirsp;
};

class MPDestoryMPIInfoData : public MessageData
{
public:
	explicit MPDestoryMPIInfoData(MPDestoryMPIRsp* pmdmr)
	{
		m_mpdestroympirsp = pmdmr;
	}
	~MPDestoryMPIInfoData(){}
	MPDestoryMPIRsp* m_mpdestroympirsp;
};

class MPCreateScreenInfoData : public MessageData
{
public:
	explicit MPCreateScreenInfoData(MPCreateScreenRsp* pmcsr)
	{
		m_mpcreatescreenrsp = pmcsr;
	}
	~MPCreateScreenInfoData(){}
	MPCreateScreenRsp* m_mpcreatescreenrsp;
};


class MPDestoryScreenInfoData : public MessageData
{
public:
	explicit MPDestoryScreenInfoData(MPDestoryScreenRsp* pmdsr)
	{
		m_mpdestroyscrrsp = pmdsr;
	}
	~MPDestoryScreenInfoData(){}
	MPDestoryScreenRsp* m_mpdestroyscrrsp;
};

class MPSeleteVideoCmdData : public MessageData
{
public:
	explicit MPSeleteVideoCmdData(MPSeleteVideoCmd* pmpsvcmd)
	{
		m_mpseletevideocmd = pmpsvcmd;
	}
	~MPSeleteVideoCmdData(){}
	MPSeleteVideoCmd* m_mpseletevideocmd;
};

class MPLayoutinfoIndData : public MessageData
{
public:
	explicit MPLayoutinfoIndData(MPLayoutinfoInd* pmpliind)
	{
		m_mplayoutinfoind = pmpliind;
	}
	~MPLayoutinfoIndData(){}
	MPLayoutinfoInd* m_mplayoutinfoind;
};

class CRSCreateConfInfoData : public MessageData
{
public:
	explicit CRSCreateConfInfoData(CRSCreateConfRsp* pcsrrsp)
	{
		m_crscreateconfrsp = pcsrrsp;
	}
	~CRSCreateConfInfoData(){}
	CRSCreateConfRsp* m_crscreateconfrsp;
};

class CRSStartRecInfoData : public MessageData
{
public:
	explicit CRSStartRecInfoData(CRSStartRecRsp* pcsrrsp)
	{
		m_crsstartrecrsp = pcsrrsp;
	}
	~CRSStartRecInfoData(){}
	CRSStartRecRsp* m_crsstartrecrsp;
};

class CRSStopRecInfoData : public MessageData
{
public:
	explicit CRSStopRecInfoData(CRSStopRecRsp* pcsrrsp)
	{
		m_crsstoprecrsp = pcsrrsp;
	}
	~CRSStopRecInfoData(){}
	CRSStopRecRsp* m_crsstoprecrsp;
};

class CRSSeleteVideoCmdData : public MessageData
{
public:
	explicit CRSSeleteVideoCmdData(CRSSeleteVideoCmd* pcrssvcmd)
	{
		m_crsseletevideocmd = pcrssvcmd;
	}
	~CRSSeleteVideoCmdData(){}
	CRSSeleteVideoCmd* m_crsseletevideocmd;
};

class CRSFileStorPathInfoData : public MessageData
{
public:
	explicit CRSFileStorPathInfoData(CRSFileStorPathInd* pcsrind)
	{
		m_crsfilestorpathind = pcsrind;
	}
	~CRSFileStorPathInfoData(){}
	CRSFileStorPathInd* m_crsfilestorpathind;
};

class CRSStartLiveInfoData : public MessageData
{
public:
	explicit CRSStartLiveInfoData(CRSStartLiveRsp* pcsrrsp)
	{
		m_crsstartliversp = pcsrrsp;
	}
	~CRSStartLiveInfoData(){}
	CRSStartLiveRsp* m_crsstartliversp;
};

class CRSLiveStateInfoData : public MessageData
{
public:
	explicit CRSLiveStateInfoData(CRSLiveStateInd* pcsrind)
	{
		m_crslivestateind = pcsrind;
	}
	~CRSLiveStateInfoData(){}
	CRSLiveStateInd* m_crslivestateind;
};

//class CRSStopLiveInfoData : public MessageData
//{
//public:
//	explicit CRSStopLiveInfoData(CRSStopLiveRsp* pcsrrsp)
//	{
//		m_crsstopliversp = pcsrrsp;
//	}
//	~CRSStopLiveInfoData(){}
//	CRSStopLiveRsp* m_crsstopliversp;
//};

class GWQueryMonitorRecInfoData : public MessageData
{
public:
	explicit GWQueryMonitorRecInfoData(GWQueryMonitorRecInd* pgwind)
	{
		m_gwquerymrind = pgwind;
	}
	~GWQueryMonitorRecInfoData(){}
	GWQueryMonitorRecInd* m_gwquerymrind;
};

//e_netmpindterstatisticsinfo
class NetMPIndTerStatisticsInfoData : public MessageData
{
public:
	explicit NetMPIndTerStatisticsInfoData(NetmpIndTerStatisticsInfo* ptsi)
	{
		m_netmpindtsi = ptsi;
	}
	~NetMPIndTerStatisticsInfoData(){}//do not delete m_netmpcreateconfrsp here!!!
	NetmpIndTerStatisticsInfo* m_netmpindtsi;
};

//e_gwindterstatisticsinfo
class GWIndTerStatisticsInfoData : public MessageData
{
public:
	explicit GWIndTerStatisticsInfoData(GWIndTerStatisticsInfo* ptsi)
	{
		m_gwindtsi = ptsi;
	}
	~GWIndTerStatisticsInfoData(){}//do not delete m_netmpcreateconfrsp here!!!
	GWIndTerStatisticsInfo* m_gwindtsi;
};

class RelaySvrRspTJCInfoData : public MessageData
{
public:
	explicit RelaySvrRspTJCInfoData(RelaySvrTJCRsp* prsvrrsp)
	{
		m_relaysvrrsptjc = prsvrrsp;
	}
	~RelaySvrRspTJCInfoData(){}
	RelaySvrTJCRsp* m_relaysvrrsptjc;
};

//e_notify_terjoinconf
//e_notify_terexitconf
class NotifyTerJoinOrExitConfToDevMgrThreadData : public MessageData
{
public:
	explicit NotifyTerJoinOrExitConfToDevMgrThreadData(SR_uint64 confid,SR_uint32 terid,SR_uint32 netmpid)
	{
		m_confid = confid;
		m_terid = terid;
		m_netmpid = netmpid;
	}
	~NotifyTerJoinOrExitConfToDevMgrThreadData(){}
	SR_uint64 m_confid;
	SR_uint32 m_terid;
	SR_uint32 m_netmpid;
private:
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(NotifyTerJoinOrExitConfToDevMgrThreadData);
};

//e_notify_confexit
class NotifyConfExitToDevMgrThreadData : public MessageData
{
public:
	NotifyConfExitToDevMgrThreadData(){}//m_netmps.clear();}
	~NotifyConfExitToDevMgrThreadData(){}//m_netmps.clear();}
	SR_uint64 m_confid;
	SR_uint32 m_ternums;
};

class DevMgrSockErrorData : public MessageData
{
public:
	explicit DevMgrSockErrorData(/*SR_socket devmgrsockfd, */SR_void* pDevmgrSocket)
	{
		//m_devmgrsockfd = devmgrsockfd;
		m_pDevmgrSocket = pDevmgrSocket;
	}
	~DevMgrSockErrorData(){}
	//SR_socket m_devmgrsockfd;
	SR_void* m_pDevmgrSocket;
};

//e_rspconfinfotomc
class ConfInfoData : public MessageData
{
public:
	explicit ConfInfoData(SR_void* cfdm)
	{
		m_confinfo = cfdm;
	}
	~ConfInfoData(){}//do not delete m_confinfo here!!!
	SR_void* m_confinfo;//SRMsgs::RspConfInfoToMC*
};

//�Զ�ɾ�� MessageData������ָ��
class Simple_ScopedPtr
{
public:
	explicit Simple_ScopedPtr(MessageData* ptr){m_ptr = ptr;}
	~Simple_ScopedPtr(){delete m_ptr;}
private:
	MessageData* m_ptr;
	SUIRUI_DISALLOW_COPY_AND_ASSIGN(Simple_ScopedPtr);
	SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(Simple_ScopedPtr);
};


} //namespace SRMC

#endif //#ifndef SR_QUEUEMSGID_H