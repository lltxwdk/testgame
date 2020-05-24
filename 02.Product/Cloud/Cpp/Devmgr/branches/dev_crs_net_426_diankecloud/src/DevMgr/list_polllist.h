
#ifndef LIST_POLLLIST_H
#define LIST_POLLLIST_H

#include <iostream>
#include <list>
#include "DBconnector.h"
//#include "sr_timer.h"

using std::string;

//static unsigned long long m_pub_liveinfo_id;
class PollList
{
public:
	PollList()
	{
		m_user_id = 0;
		m_order_no = 0;
	}
	~PollList()
	{
		m_user_id = 0;
		m_order_no = 0;
	}

	int m_user_id;
	int m_order_no;
protected:
private:
};

class CPollList
{
public:
	CPollList();
	virtual ~CPollList();


	unsigned long long GetPollListID()
	{
		return m_polllist_id;
	}
	unsigned long long GetPollInfoID()
	{
		return m_pollinfo_id;
	}
	int GetUserID()
	{
		return m_user_id;
	}
	int GetOrderNO()
	{
		return m_order_no;
	}


	void SetPollListID(unsigned long long polllist_id)
	{
		m_polllist_id = polllist_id;
	}
	void SetPollInfoID(unsigned long long pollinfo_id)
	{
		m_pollinfo_id = pollinfo_id;
	}
	void SetUserID(int user_id)
	{
		m_user_id = user_id;
	}
	void SetOrderNO(int order_no)
	{
		m_order_no = order_no;
	}

	////
	//bool InsertDB();
	//bool DeleteDB();
	//bool UpdateDB1();
	//bool UpdateDB2();
	bool SelectPollList(std::list<PollList*> &polllist);

	//bool InsertPollListToDB();
	//bool DeletePollListForDB();
	////bool UpdateConfCRSRealEndtimeForDB();
	////bool SelectConfRecordidFromDB();

protected:

private:
	unsigned long long m_polllist_id;
	unsigned long long m_pollinfo_id;
	int m_user_id;
	int m_order_no;

	CCriticalSection m_csPolllist;
};

#endif //LIST_POLLLIST_H