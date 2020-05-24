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

	SR_uint32 	m_psuid;//�ն�Ψһ��ʶ(���ն˲���)
	SR_uint32 	m_usertype;//�û�����,0-��ʱ�û�,1-��ʽ�û�,2-Ubox,3-��׼�ն�(H.323/sip),4-���ǰ��
	std::string m_nickname;//�ն��ǳơ�����
	std::string m_ip;//�ն�ip
	SR_uint32 	m_protocoltype;//��׼�ն˵ĺ���Э������,0-�Զ���1-sip��2-H.323��3-GB28181,SR�ն�ǿ������0
	SR_uint32 	m_bandwidth;//�ն˺��д���
	SR_uint32   m_devicetype; //�豸����,0-�նˡ�1-MCU��2-���
	std::string m_devicecode;//����豸����,��m_usertype==4-���ʱ������ǰ���豸����
	std::string m_shortname;//��׼�ն˵Ķ̺�,��m_usertype==3ע��ʱд�����ݿ��ֵ
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