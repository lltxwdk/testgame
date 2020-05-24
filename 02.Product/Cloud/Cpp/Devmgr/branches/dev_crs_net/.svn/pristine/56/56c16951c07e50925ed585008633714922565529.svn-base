
#ifndef LIST_CONFPOLLINFO_H
#define LIST_CONFPOLLINFO_H

#include <iostream>
#include <list>
#include "DBconnector.h"
#include "list_polllist.h"
//#include "sr_timer.h"

using std::string;

//static unsigned long long m_pub_liveinfo_id;
class PollInfo
{
public:
	PollInfo()
	{
		m_pollinfo_id = 0u;
		m_pollinfo_name.clear();
		m_polllist.clear();
		m_totalsendpartnum = 0;
		m_alreadysendpartnum = 0;
	}
	~PollInfo()
	{
		m_pollinfo_id = 0u;
		m_pollinfo_name.clear();

		for (std::list<PollList*>::iterator pl_itor = m_polllist.begin();
			pl_itor != m_polllist.end();/* pl_itor++*/)
		{
			PollList* pPolllist = *pl_itor;
			if (pPolllist != NULL)
			{
				delete pPolllist;
				pPolllist = NULL;
			}
			pl_itor = m_polllist.erase(pl_itor);
		}
		m_polllist.clear();
		m_totalsendpartnum = 0;
		m_alreadysendpartnum = 0;
	}

	unsigned long long m_pollinfo_id;
	std::string m_pollinfo_name; // 名单名称
	std::list<PollList*> m_polllist;
	unsigned int m_totalsendpartnum;
	unsigned int m_alreadysendpartnum;
protected:
private:
};

class CConfPollInfo
{
public:
	CConfPollInfo();
	virtual ~CConfPollInfo();


	unsigned long long GetConfID()
	{
		return m_conf_id;
	}
	unsigned long long GetPollInfoID()
	{
		return m_pollinfo_id;
	}
	std::string GetPollInfoName()
	{
		return m_pollinfo_name;
	}


	void SetConfID(unsigned long long conf_id)
	{
		m_conf_id = conf_id;
	}
	void SetPollInfoID(unsigned long long pollinfo_id)
	{
		m_pollinfo_id = pollinfo_id;
	}
	void SetPollInfoName(std::string pollinfo_name)
	{
		m_pollinfo_name = pollinfo_name;
	}

	////
	//bool InsertDB();
	//bool DeleteDB();
	//bool UpdateDB1();
	//bool UpdateDB2();
	bool SelectConfPollInfo(std::list<PollInfo*> &pollInfos);

	//bool InsertConfPollInfoToDB();
	//bool DeleteConfPollInfoForDB();
	//bool UpdateConfCRSRealEndtimeForDB();
	//bool SelectConfRecordidFromDB();

protected:

private:
	unsigned long long m_pollinfo_id;
	unsigned long long m_conf_id;
	std::string m_pollinfo_name; // 轮巡列表名称

	CCriticalSection m_csConfpollinfo;
};

#endif //LIST_CONFPOLLINFO_H