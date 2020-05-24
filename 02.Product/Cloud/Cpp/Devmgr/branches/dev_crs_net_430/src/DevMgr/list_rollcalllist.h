
#ifndef LIST_ROLLCALLLIST_H
#define LIST_ROLLCALLLIST_H

#include <iostream>
#include <list>
#include "DBconnector.h"
//#include "sr_timer.h"

using std::string;

//static unsigned long long m_pub_liveinfo_id;
class CallList
{
public:
	CallList()
	{
		m_user_id = 0;
		m_order_no = 0;
		m_nickname.clear();
	}
	~CallList()
	{
		m_user_id = 0;
		m_order_no = 0;
		m_nickname.clear();
	}

	int m_user_id;
	int m_order_no;
	std::string m_nickname;
protected:
private:
};

class CRollCallList
{
public:
	CRollCallList();
	virtual ~CRollCallList();


	unsigned long long GetCallListID()
	{
		return m_calllist_id;
	}
	unsigned long long GetCallInfoID()
	{
		return m_callinfo_id;
	}
	int GetUserID()
	{
		return m_user_id;
	}
	int GetOrderNO()
	{
		return m_order_no;
	}


	void SetCallListID(unsigned long long calllist_id)
	{
		m_calllist_id = calllist_id;
	}
	void SetCallInfoID(unsigned long long callinfo_id)
	{
		m_callinfo_id = callinfo_id;
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
	bool SelectCallList(std::list<CallList*> &calllist);

	bool InsertCallListToDB();
	bool DeleteCallListForDB();
	//bool UpdateConfCRSRealEndtimeForDB();
	//bool SelectConfRecordidFromDB();

protected:

private:
	unsigned long long m_calllist_id;
	unsigned long long m_callinfo_id;
	int m_user_id;
	int m_order_no;

	CCriticalSection m_csRollcalllist;
};

#endif //LIST_ROLLCALLLIST_H