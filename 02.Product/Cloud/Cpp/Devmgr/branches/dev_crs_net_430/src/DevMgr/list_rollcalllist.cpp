#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_rollcalllist.h"
#include <cstdlib>
#include <stdio.h>

CRollCallList::CRollCallList()
{
	m_calllist_id = 0;
	m_callinfo_id = 0;
	m_user_id = 0;
	m_order_no = 0;
}

CRollCallList::~CRollCallList()
{
	m_calllist_id = 0;
	m_callinfo_id = 0;
	m_user_id = 0;
	m_order_no = 0;
}

bool CRollCallList::SelectCallList(std::list<CallList*> &calllist)
{
	CCriticalAutoLock MyLock(m_csRollcalllist);

	char cSelect[1024];

	// 按照order_no升序获取
#ifdef WIN32
	sprintf(cSelect, "select user_id,order_no from call_list where ci_id='%I64d' order by order_no asc", m_callinfo_id);
#elif defined LINUX
	sprintf(cSelect, "select user_id,order_no from call_list where ci_id='%lld' order by order_no asc", m_callinfo_id);
#endif

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CRollCallList::SelectDB GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CRollCallList::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return false;
	}
	else
	{
		for (std::map<unsigned long long, MYSQL_ROW>::iterator row_itor = rows_set.begin();
			row_itor != rows_set.end(); row_itor++)
		{
			MYSQL_ROW ret_row = row_itor->second;
			if (ret_row)
			{
				int uiUserid = 0;
				uiUserid = atoi(ret_row[0]);

				int uiOrderNo = 0;
				uiOrderNo = atoi(ret_row[1]);

#ifdef WIN32
				printf("SelectCallList::RowNum:%I64d={%d, %d}\n", (row_itor->first) + 1, uiUserid, uiOrderNo);
#elif defined LINUX
				printf("SelectCallList::RowNum:%lld={%d, %d}\n", (row_itor->first) + 1, uiUserid, uiOrderNo);
#endif


				if (uiUserid != 0)
				{
					CallList* pCallList = new CallList();
					pCallList->m_user_id = uiUserid;
					pCallList->m_order_no = uiOrderNo;
					calllist.push_back(pCallList);
				}
			}
		}
	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CRollCallList::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;

#elif SHENTONG_DB_DEF
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CRollCallList::SelectDB GetResults error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		calllist.clear();
		if (prs != NULL)
		{
			unsigned long long ullRowNum = 0;
			// 注意：列的编号是从1开始,如果结果为空也会进入while循环,此时获取的值是0
			while (prs->next())
			{
				ullRowNum++;

				int uiUserid = 0;
				uiUserid = prs->getInt(1);

				int uiOrderNo = 0;
				uiOrderNo = prs->getInt(2);

#ifdef WIN32
				printf("SelectCallList::RowNum:%I64d={%d, %d}\n", ullRowNum, uiUserid, uiOrderNo);
#elif defined LINUX
				printf("SelectCallList::RowNum:%lld={%d, %d}\n", ullRowNum, uiUserid, uiOrderNo);
#endif


				if (uiUserid != 0)
				{
					CallList* pCallList = new CallList();
					pCallList->m_user_id = uiUserid;
					pCallList->m_order_no = uiOrderNo;
					calllist.push_back(pCallList);
				}
			}

			if (ullRowNum > 0)
			{
				bgetok = true;
			}
		}
		else
		{
			bgetok = false;
		}

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);
	}

	return bgetok;
#endif
}

bool CRollCallList::InsertCallListToDB()
{
	CCriticalAutoLock MyLock(m_csRollcalllist);

	char cInsert[1024];
#ifdef WIN32
	sprintf(cInsert, "insert into call_list(ci_id, user_id, order_no) values('%I64d','%d','%d')",
		m_callinfo_id, m_user_id, m_order_no);
#elif defined LINUX
	sprintf(cInsert, "insert into call_list(ci_id, user_id, order_no) values('%lld','%d','%d')",
		m_callinfo_id, m_user_id, m_order_no);
#endif

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cInsert);

	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_DEBUG, "insert call_list callinfo_id:%I64d,user_id:%d,order_no:%d error\r\n", m_callinfo_id, m_user_id, m_order_no, CDBConnector::GetInstance()->OutErrors());
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_DEBUG, "insert call_list callinfo_id:%lld,user_id:%d,order_no:%d error\r\n", m_callinfo_id, m_user_id, m_order_no, CDBConnector::GetInstance()->OutErrors());
#endif
		return false;
	}

	return true;
}

bool CRollCallList::DeleteCallListForDB()
{
	CCriticalAutoLock MyLock(m_csRollcalllist);

	char cDelete[1024];
#ifdef WIN32
	sprintf(cDelete, "delete from call_list where ci_id='%I64d'", m_callinfo_id);
#elif defined LINUX
	sprintf(cDelete, "delete from call_list where ci_id='%lld'", m_callinfo_id);
#endif

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cDelete);

	if (!CDBConnector::GetInstance()->ExectCommand(cDelete))
	{
		sr_printf(SR_LOGLEVEL_DEBUG, "%s error\r\n", cDelete, CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}