
#ifndef LIST_USERCONFDETAIL_H
#define LIST_USERCONFDETAIL_H

#include <iostream>
#include "DBconnector.h"
//#include "sr_timer.h"

using std::string;

static unsigned long long m_pub_userrpt_detail_id;

class CUserConfDetail
{
public:
	CUserConfDetail(unsigned int uiDevmgrNo);
	virtual ~CUserConfDetail();
	//static CUserConfDetail* GetInstance();

	//
	unsigned long long GetConfID()
	{ 
		//CCriticalAutoLock MyLock(m_cs);
		return m_conf_id;
	}
	unsigned long long GetConfReportID()
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_conf_report_id;
	}
	unsigned long long GetUserRptDetailID()
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_userrpt_detail_id; 
	}
	int GetUserID() 
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_user_id;
	}
	std::string GetUserAlias() 
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_useralias; 
	}
	std::string GetRealuseStarttime() 
	{ 
		//CCriticalAutoLock MyLock(m_cs);
		return m_realjointime; 
	}
	std::string GetRealuseStoptime() 
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_realleavetime; 
	}
	int GetFromtype()
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_fromtype;
	}
	int GetTermtype()
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_termtype;
	}

	void SetConfID(unsigned long long conf_id) 
	{ 
		//CCriticalAutoLock MyLock(m_cs);
		m_conf_id = conf_id;
	}
	void SetConfReportID(unsigned long long conf_report_id)
	{ 
		//CCriticalAutoLock MyLock(m_cs);
		m_conf_report_id = conf_report_id; 
	}
	void SetUserRptDetailID(unsigned long long userrpt_detail_id)
	{ 
		//CCriticalAutoLock MyLock(m_cs);
		m_userrpt_detail_id = userrpt_detail_id;
	}
	void SetUserID(int user_id) 
	{
		//CCriticalAutoLock MyLock(m_cs);
		m_user_id = user_id; 
	}
	void SetUserAlias(std::string useralias) 
	{
		//CCriticalAutoLock MyLock(m_cs);
		m_useralias = useralias; 
	}
	void SetRealjointime(std::string realjointime)
	{
		//CCriticalAutoLock MyLock(m_cs);
		m_realjointime = realjointime;
	}
	void SetRealleavetime(std::string realleavetime) 
	{
		//CCriticalAutoLock MyLock(m_cs);
		m_realleavetime = realleavetime;
	}
	void SetFromtype(int fromtype)
	{
		//CCriticalAutoLock MyLock(m_cs);
		m_fromtype = fromtype;
	}
	void SetTermtype(int termtype)
	{
		//CCriticalAutoLock MyLock(m_cs);
		m_termtype = termtype;
	}
	
	//
	bool InsertDB();
	bool DeleteDB();
	bool UpdateDB1();
	bool UpdateDB2();
	bool UpdateDB4();
	bool SelectDB();

	bool InsertUserConfDetailToDB();
protected:
	//CUserConfDetail();
	bool UpdateDB3(int status);

private:
	unsigned long long m_conf_id;
	unsigned long long m_conf_report_id;
	unsigned long long m_userrpt_detail_id;
	int m_user_id;
	std::string m_useralias;
	std::string m_realjointime;
	std::string m_realleavetime;
	int m_fromtype;
	int m_termtype;

	CCriticalSection m_csUCD;
};

#endif //LIST_USERCONFDETAIL_H