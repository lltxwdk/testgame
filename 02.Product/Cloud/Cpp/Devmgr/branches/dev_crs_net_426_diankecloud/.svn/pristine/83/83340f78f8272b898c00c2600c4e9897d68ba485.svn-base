
#ifndef LIST_LIVESETTING_H
#define LIST_LIVESETTING_H

#include <iostream>
#include "DBconnector.h"
//#include "sr_timer.h"

using std::string;

//static unsigned long long m_pub_confrecord_id;

class CConfLiveSetting
{
public:
	CConfLiveSetting();
	virtual ~CConfLiveSetting();

	
	unsigned long long GetConfID()
	{
		return m_conf_id;
	}
	unsigned long long GetLivesetID()
	{
		return m_liveset_id;
	}
	std::string GetLivesetSubject()
	{
		return m_liveset_subject;
	}
	std::string GetLivesetChairman()
	{
		return m_liveset_chairman;
	}
	std::string GetLivesetAbstract()
	{
		return m_liveset_abstract;
	}
	int GetIsPublic()
	{
		return m_is_public;
	}
	std::string GetLivePWD()
	{
		return m_live_pwd;
	}
	int GetIsUseRec()
	{
		return m_is_userec;
	}

	void SetConfID(unsigned long long conf_id)
	{
		m_conf_id = conf_id;
	}
	void SetLivesetID(unsigned long long liveset_id)
	{
		m_liveset_id = liveset_id;
	}
	void SetLivesetSubject(std::string liveset_subject)
	{
		m_liveset_subject = liveset_subject;
	}
	void SetLivesetChairman(std::string liveset_chairman)
	{
		m_liveset_chairman = liveset_chairman;
	}
	void SetLivesetAbstract(std::string liveset_abstract)
	{
		m_liveset_abstract = liveset_abstract;
	}
	void SetIsPublic(int is_public)
	{
		m_is_public = is_public;
	}
	void SetLivePWD(std::string live_pwd)
	{
		m_live_pwd = live_pwd;
	}
	void SetIsUseRec(int is_userec)
	{
		m_is_userec = is_userec;
	}
	
	bool SelectConfLiveSettingInfo();

protected:

private:
	unsigned long long m_liveset_id;
	unsigned long long m_conf_id;
	std::string m_liveset_subject; // 直播主题
	std::string m_liveset_chairman;
	std::string m_liveset_abstract; // 直播摘要/简介
	int m_is_public;
	std::string m_live_pwd;
	int m_is_userec;

	CCriticalSection m_csConfLivesetting;
};

#endif //LIST_LIVESETTING_H