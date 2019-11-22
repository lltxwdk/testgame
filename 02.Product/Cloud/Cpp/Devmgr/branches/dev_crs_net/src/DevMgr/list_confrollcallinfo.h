
#ifndef LIST_CONFROLLCALLINFO_H
#define LIST_CONFROLLCALLINFO_H

#include <iostream>
#include <list>
#include "DBconnector.h"
//#include "sr_timer.h"

using std::string;

//static unsigned long long m_pub_liveinfo_id;
class RollCallInfo
{
public:
	RollCallInfo()
	{
		m_callinfo_id = 0u;
		m_callinfo_name.clear();
	}
	~RollCallInfo()
	{
		m_callinfo_id = 0u;
		m_callinfo_name.clear();
	}

	unsigned long long m_callinfo_id;
	std::string m_callinfo_name; // 名单名称
protected:
private:
};

class CConfRollCallInfo
{
public:
	CConfRollCallInfo();
	virtual ~CConfRollCallInfo();


	unsigned long long GetConfID()
	{
		return m_conf_id;
	}
	unsigned long long GetCallInfoID()
	{
		return m_callinfo_id;
	}
	std::string GetCallInfoName()
	{
		return m_callinfo_name;
	}


	void SetConfID(unsigned long long conf_id)
	{
		m_conf_id = conf_id;
	}
	void SetCallInfoID(unsigned long long callinfo_id)
	{
		m_callinfo_id = callinfo_id;
	}
	void SetCallInfoName(std::string callinfo_name)
	{
		m_callinfo_name = callinfo_name;
	}

	////
	//bool InsertDB();
	//bool DeleteDB();
	//bool UpdateDB1();
	//bool UpdateDB2();
	bool SelectConfCallInfo(std::list<RollCallInfo*> &rollcallInfos);

	bool InsertConfCallInfoToDB();
	bool DeleteConfCallInfoForDB();
	//bool UpdateConfCRSRealEndtimeForDB();
	//bool SelectConfRecordidFromDB();

protected:

private:
	unsigned long long m_callinfo_id;
	unsigned long long m_conf_id;
	std::string m_callinfo_name; // 名单名称

	CCriticalSection m_csConfrollcallinfo;
};

#endif //LIST_CONFROLLCALLINFO_H