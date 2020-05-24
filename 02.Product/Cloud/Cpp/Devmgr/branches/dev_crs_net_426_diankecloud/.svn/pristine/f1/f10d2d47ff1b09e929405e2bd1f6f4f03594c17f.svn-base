#ifndef LIST_CONF_PARTICIPANT_H
#define LIST_CONF_PARTICIPANT_H

#include <iostream>
#include "DBconnector.h"

class ParticipantInfo
{
public:
	ParticipantInfo()
	{
		m_psuid = 0u;
		m_usertype = 0u;
		m_nickname.clear();
		m_ip.clear();
		m_protocoltype = 0u;
		m_bandwidth = 0u;
		m_devicetype = 0u;
		m_devicecode.clear();
		m_shortname.clear();
		m_orderno = 0u;

		m_calloption.clear();
		m_relatedgwid = 0u;
	}
	~ParticipantInfo()
	{
		m_psuid = 0u;
		m_usertype = 0u;
		m_nickname.clear();
		m_ip.clear();
		m_protocoltype = 0u;
		m_bandwidth = 0u;
		m_devicetype = 0u;
		m_devicecode.clear();
		m_shortname.clear();
		m_orderno = 0u;

		m_calloption.clear();
		m_relatedgwid = 0u;
	}

	SR_uint32 	m_psuid;//终端唯一标识(由终端产生)
	SR_uint32 	m_usertype;//用户类型,0-临时用户,1-正式用户,2-Ubox,3-标准终端(H.323/sip),4-监控前端
	std::string m_nickname;//终端昵称、别名
	std::string m_ip;//终端ip
	SR_uint32 	m_protocoltype;//标准终端的呼叫协议类型,0-自动、1-sip、2-H.323、3-GB28181,SR终端强制填入0
	SR_uint32 	m_bandwidth;//终端呼叫带宽
	SR_uint32   m_devicetype; //设备类型,0-终端、1-MCU、2-监控
	std::string m_devicecode;//监控设备编码,即m_usertype==4-监控时填入监控前端设备编码
	std::string m_shortname;//标准终端的短号,即m_usertype==3注册时写入数据库的值
	SR_uint32 	m_orderno;

	std::string m_calloption;
	SR_uint32 	m_relatedgwid;
protected:
private:
};

class CConfParticipant
{
public:
	CConfParticipant();
	virtual ~CConfParticipant();

	unsigned long long GetParticipantID() { return m_participant_id; }
	unsigned long long GetConfID() { return m_conf_id; }
	unsigned int GetUserID() { return m_user_id; }
	unsigned int GetOrderNo() { return m_order_no; }

	void SetParticipantID(unsigned long long participant_id) { m_participant_id = participant_id; }
	void SetConfID(unsigned long long conf_id) { m_conf_id = conf_id; }
	void SetUserID(unsigned int user_id) { m_user_id = user_id; }
	void SetOrderNo(unsigned int order_no) { m_order_no = order_no; }

	bool InsertDB();
	bool DeleteDB();
	bool UpdateDB();
	bool SelectDB();
	bool SelectParticipantUserInfo(std::map<unsigned int, ParticipantInfo*> &participantInfos);

protected:
private:
	unsigned long long m_participant_id;
	unsigned long long m_conf_id;
	unsigned int m_user_id;
	unsigned int m_order_no;

	CCriticalSection m_csConfParticipant;
};

#endif //LIST_CONF_PARTICIPANT_H