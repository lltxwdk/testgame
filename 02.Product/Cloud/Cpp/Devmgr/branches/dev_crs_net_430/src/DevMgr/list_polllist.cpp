#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_polllist.h"
#include <cstdlib>
#include <stdio.h>

CPollList::CPollList()
{
	m_polllist_id = 0;
	m_pollinfo_id = 0;
	m_user_id = 0;
	m_order_no = 0;
}

CPollList::~CPollList()
{
	m_polllist_id = 0;
	m_pollinfo_id = 0;
	m_user_id = 0;
	m_order_no = 0;
}

bool CPollList::SelectPollList(std::list<PollList*> &polllist)
{
	CCriticalAutoLock MyLock(m_csPolllist);

	char cSelect[1024];

	// 按照order_no升序获取
#ifdef WIN32
	sprintf(cSelect, "select user_id,order_no from t_poll_list where pi_id='%I64d' order by order_no asc", m_pollinfo_id);
#elif defined LINUX
	sprintf(cSelect, "select user_id,order_no from t_poll_list where pi_id='%lld' order by order_no asc", m_pollinfo_id);
#endif

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CPollList::SelectDB GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CPollList::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return false;
	}
	else
	{
		polllist.clear();
		for (std::map<unsigned long long, MYSQL_ROW>::iterator row_itor = rows_set.begin();
			row_itor != rows_set.end(); row_itor++)
		{
			MYSQL_ROW ret_row = row_itor->second;
			if (ret_row)
			{
				int uiUserid = 0;
				uiUserid = atoi(ret_row[0]);

				int uiOrderNo = 0;
				if (ret_row[1] != NULL)
				{
					uiOrderNo = atoi(ret_row[1]);
				}

#ifdef WIN32
				printf("SelectPollList::RowNum:%I64d={%d, %d}\n", (row_itor->first) + 1, uiUserid, uiOrderNo);
#elif defined LINUX
				printf("SelectPollList::RowNum:%lld={%d, %d}\n", (row_itor->first) + 1, uiUserid, uiOrderNo);
#endif


				if (uiUserid != 0)
				{
					PollList* pPollList = new PollList();
					pPollList->m_user_id = uiUserid;
					pPollList->m_order_no = uiOrderNo;
					polllist.push_back(pPollList);
				}
			}
		}
	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CPollList::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
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
		polllist.clear();
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
				printf("SelectPollList::RowNum:%I64d={%d, %d}\n", ullRowNum, uiUserid, uiOrderNo);
#elif defined LINUX
				printf("SelectPollList::RowNum:%lld={%d, %d}\n", ullRowNum, uiUserid, uiOrderNo);
#endif


				if (uiUserid != 0)
				{
					PollList* pPollList = new PollList();
					pPollList->m_user_id = uiUserid;
					pPollList->m_order_no = uiOrderNo;
					polllist.push_back(pPollList);
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
