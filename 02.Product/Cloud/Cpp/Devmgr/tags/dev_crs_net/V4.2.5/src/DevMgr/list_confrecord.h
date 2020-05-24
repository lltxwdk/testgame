
#ifndef LIST_CONFRECORD_H
#define LIST_CONFRECORD_H

#include <iostream>
#include "DBconnector.h"
//#include "sr_timer.h"

using std::string;

static unsigned long long m_pub_confrecord_id;

class CConfRecord
{
public:
	CConfRecord(unsigned int uiDevmgrNo);
	virtual ~CConfRecord();
	//static CConfReport* GetInstance();

	//
	//unsigned long long GetConfID()
	//{
	//	return m_conf_id;
	//}
	unsigned long long GetConfReportID()
	{
		return m_confreport_id;
	}
	unsigned long long GetConfRecordID()
	{
		return m_confrecord_id;
	}
	std::string GetStartTime()
	{
		return m_starttime;
	}
	std::string GetStopTime()
	{
		return m_stoptime;
	}
	std::string GetConfName()
	{
		return m_confname;
	}

	//void SetConfID(unsigned long long conf_id)
	//{
	//	m_conf_id = conf_id;
	//}
	void SetConfReportID(unsigned long long confreport_id)
	{
		m_confreport_id = confreport_id;
	}
	void SetConfRecordID(unsigned long long confrecord_id)
	{
		m_confrecord_id = confrecord_id;
	}
	void SetStartTime(std::string starttime)
	{
		m_starttime = starttime;
	}
	void SetStopTime(std::string stoptime)
	{
		m_stoptime = stoptime;
	}
	void SetConfName(std::string confname)
	{
		m_confname = confname;
	}

	////
	//bool InsertDB();
	//bool DeleteDB();
	//bool UpdateDB1();
	//bool UpdateDB2();
	//bool SelectDB();

	bool InsertConfCRSRecordToDB();
	//bool DeleteConfCRSRecordForDB();
	bool UpdateConfCRSRealEndtimeForDB();
	bool SelectConfRecordidFromDB();

protected:
	//CConfReport();

private:
	//unsigned long long m_conf_id;
	unsigned long long m_confreport_id;
	unsigned long long m_confrecord_id;
	std::string m_starttime;
	std::string m_stoptime;
	std::string m_confname;
	
	CCriticalSection m_csConfrec;
};

#endif //LIST_CONFRECORD_H