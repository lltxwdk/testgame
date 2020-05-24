#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_confpollinfo.h"
#include <cstdlib>
#include <stdio.h>

CConfPollInfo::CConfPollInfo()
{
	m_pollinfo_id = 0;
	m_conf_id = 0;
	m_pollinfo_name = "";
}

CConfPollInfo::~CConfPollInfo()
{
	m_pollinfo_id = 0;
	m_conf_id = 0;
	m_pollinfo_name = "";
}


bool CConfPollInfo::SelectConfPollInfo(std::list<PollInfo*> &pollInfos)
{
	CCriticalAutoLock MyLock(m_csConfpollinfo);

	char cSelect[1024];

	//#ifdef WIN32
	//	sprintf(cSelect, "select pi_id,pi_name,conf_id from t_poll_info where conf_id='%I64d'", m_conf_id);
	//#elif defined LINUX
	//	sprintf(cSelect, "select pi_id,pi_name,conf_id from t_poll_info where conf_id='%lld'", m_conf_id);
	//#endif

#ifdef WIN32
	sprintf(cSelect, "select pi_id,pi_name from t_poll_info where conf_id='%I64d'", m_conf_id);
#elif defined LINUX
	sprintf(cSelect, "select pi_id,pi_name from t_poll_info where conf_id='%lld'", m_conf_id);
#endif

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CConfPollInfo::SelectDB GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CConfPollInfo::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return false;
	}
	else
	{
		pollInfos.clear();
		for (std::map<unsigned long long, MYSQL_ROW>::iterator row_itor = rows_set.begin();
			row_itor != rows_set.end(); row_itor++)
		{
			MYSQL_ROW ret_row = row_itor->second;
			if (ret_row)
			{
				unsigned long long ullpollinfoid = 0;
#ifdef WIN32
				ullpollinfoid = _atoi64(ret_row[0]);//
				//printf("SelectConfPollInfo::SelectDB() OK ci_id:%s->%I64d\r\n", ret_row[0], m_conf_id);
#elif defined LINUX
				ullpollinfoid = strtoll(ret_row[0], NULL, 10);//
				//printf("SelectConfPollInfo::SelectDB() OK ci_id:%s->%lld\r\n", ret_row[0], m_conf_id);
#endif	

				std::string str_pollinfo_name;
				str_pollinfo_name.clear();
				if (ret_row[1] != NULL)
				{
					str_pollinfo_name = ret_row[1];
				}

#ifdef WIN32
				printf("SelectConfPollInfo::RowNum:%I64d={%I64d, %s}\n", (row_itor->first) + 1, _atoi64(ret_row[0]), str_pollinfo_name.c_str());
#elif defined LINUX
				printf("SelectConfPollInfo::RowNum:%lld={%lld, %s}\n", (row_itor->first) + 1, strtoll(ret_row[0], NULL, 10), str_pollinfo_name.c_str());
#endif


				if (ullpollinfoid != 0)
				{
					PollInfo* pPollInfo = new PollInfo();
					pPollInfo->m_pollinfo_id = ullpollinfoid;
					pPollInfo->m_pollinfo_name = str_pollinfo_name;
					pollInfos.push_back(pPollInfo);
				}
			}
		}
	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CConfPollInfo::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CConfPollInfo::SelectDB GetResults error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		pollInfos.clear();
		if (prs != NULL)
		{
			unsigned long long ullRowNum = 0;
			// 注意：列的编号是从1开始,如果结果为空也会进入while循环,此时获取的值是0
			while (prs->next())
			{
				ullRowNum++;

				unsigned long long ullpollinfoid = 0;
				ullpollinfoid = (unsigned long)prs->getNumber(1);

				std::string str_pollinfo_name;
				str_pollinfo_name.clear();
				str_pollinfo_name = prs->getString(2);

#ifdef WIN32
				printf("SelectConfPollInfo::RowNum:%I64d={%I64d, %s}\n", ullRowNum, ullpollinfoid, str_pollinfo_name.c_str());
#elif defined LINUX
				printf("SelectConfPollInfo::RowNum:%lld={%lld, %s}\n", ullRowNum, ullpollinfoid, str_pollinfo_name.c_str());
#endif

				if (ullpollinfoid != 0)
				{
					PollInfo* pPollInfo = new PollInfo();
					pPollInfo->m_pollinfo_id = ullpollinfoid;
					pPollInfo->m_pollinfo_name = str_pollinfo_name;
					pollInfos.push_back(pPollInfo);
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
