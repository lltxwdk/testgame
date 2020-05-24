
#ifndef LIST_CONFREPORT_H
#define LIST_CONFREPORT_H

#include <iostream>
#include "DBconnector.h"
//#include "sr_timer.h"

using std::string;

static unsigned long long m_pub_confreport_id;

class CConfReport
{
public:
	CConfReport(unsigned int uiDevmgrNo);
	virtual ~CConfReport();
	//static CConfReport* GetInstance();

	//
	unsigned long long GetConfID()
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_conf_id;
	}
	unsigned long long GetConfReportID()
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_confreport_id; 
	}
	std::string GetStartTime() 
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_starttime;
	}
	std::string GetStopTime() 
	{ 
		//CCriticalAutoLock MyLock(m_cs);
		return m_stoptime; 
	}
	unsigned int GetHasRecord()
	{
		//CCriticalAutoLock MyLock(m_cs);
		return m_hasrecord;
	}

	void SetConfID(unsigned long long conf_id) 
	{
		//CCriticalAutoLock MyLock(m_cs);
		m_conf_id = conf_id; 
	}
	void SetConfReportID(unsigned long long confreport_id)
	{ 
		//CCriticalAutoLock MyLock(m_cs);
		m_confreport_id = confreport_id; 
	}
	void SetStartTime(std::string starttime) 
	{ 
		//CCriticalAutoLock MyLock(m_cs);
		m_starttime = starttime;
	}
	void SetStopTime(std::string stoptime) 
	{ 
		//CCriticalAutoLock MyLock(m_cs);
		m_stoptime = stoptime; 
	}
	void SetHasRecord(unsigned int uihasrecord)
	{
		//CCriticalAutoLock MyLock(m_cs);
		m_hasrecord = uihasrecord;
	}

	//
	bool InsertDB();
	bool DeleteDB();
	bool UpdateDB1();
	bool UpdateDB2();
	bool SelectDB();

	bool InsertConfReportRecordToDB();
	bool DeleteConfReportRecordForDB();
	bool UpdateConfRealEndtimeForDB();
	bool UpdateConfHasRecordForDB();

protected:
	//CConfReport();

private:
	unsigned long long m_conf_id;
	unsigned long long m_confreport_id;
	std::string m_starttime;
	std::string m_stoptime;

	unsigned int m_hasrecord;

	CCriticalSection m_csConfRp;
};

#endif //LIST_MEDIAGROUP_H