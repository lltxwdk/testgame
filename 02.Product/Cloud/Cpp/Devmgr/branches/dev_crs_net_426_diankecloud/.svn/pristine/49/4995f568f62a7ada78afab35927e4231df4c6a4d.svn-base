#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_confrollcallinfo.h"
#include <cstdlib>
#include <stdio.h>

CConfRollCallInfo::CConfRollCallInfo()
{
	m_callinfo_id = 0;
	m_conf_id = 0;
	m_callinfo_name = "";
}

CConfRollCallInfo::~CConfRollCallInfo()
{
	m_callinfo_id = 0;
	m_conf_id = 0;
	m_callinfo_name = "";
}

bool CConfRollCallInfo::SelectConfCallInfo(std::list<RollCallInfo*> &rollcallInfos)
{
	CCriticalAutoLock MyLock(m_csConfrollcallinfo);

	char cSelect[1024];

//#ifdef WIN32
//	sprintf(cSelect, "select ci_id,ci_name,conf_id from call_info where conf_id='%I64d'", m_conf_id);
//#elif defined LINUX
//	sprintf(cSelect, "select ci_id,ci_name,conf_id from call_info where conf_id='%lld'", m_conf_id);
//#endif

#ifdef WIN32
	sprintf(cSelect, "select ci_id,ci_name from call_info where conf_id='%I64d'", m_conf_id);
#elif defined LINUX
	sprintf(cSelect, "select ci_id,ci_name from call_info where conf_id='%lld'", m_conf_id);
#endif

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CConfRollCallInfo::SelectDB GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CConfRollCallInfo::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

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
				unsigned long long ullcallinfoid = 0;
#ifdef WIN32
				ullcallinfoid = _atoi64(ret_row[0]);//
				//printf("SelectConfCallInfo::SelectDB() OK ci_id:%s->%I64d\r\n", ret_row[0], m_conf_id);
#elif defined LINUX
				ullcallinfoid = strtoll(ret_row[0], NULL, 10);//
				//printf("SelectConfCallInfo::SelectDB() OK ci_id:%s->%lld\r\n", ret_row[0], m_conf_id);
#endif	

				std::string str_callinfo_name;
				str_callinfo_name.clear();
				if (ret_row[1] != NULL)
				{
					str_callinfo_name = ret_row[1];
				}

#ifdef WIN32
				printf("SelectConfCallInfo::RowNum:%I64d={%I64d, %s}\n", (row_itor->first) + 1, _atoi64(ret_row[0]), str_callinfo_name.c_str());
#elif defined LINUX
				printf("SelectConfCallInfo::RowNum:%lld={%lld, %s}\n", (row_itor->first) + 1, strtoll(ret_row[0], NULL, 10), str_callinfo_name.c_str());
#endif


				if (ullcallinfoid != 0)
				{
					RollCallInfo* pRollCallInfo = new RollCallInfo();
					pRollCallInfo->m_callinfo_id = ullcallinfoid;
					pRollCallInfo->m_callinfo_name = str_callinfo_name;
					rollcallInfos.push_back(pRollCallInfo);
				}
			}
		}
	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CConfRollCallInfo::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CConfRollCallInfo::SelectDB GetResults error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		rollcallInfos.clear();
		if (prs != NULL)
		{
			unsigned long long ullRowNum = 0;
			// 注意：列的编号是从1开始,如果结果为空也会进入while循环,此时获取的值是0
			while (prs->next())
			{
				ullRowNum++;

				unsigned long long ullcallinfoid = 0;
				ullcallinfoid = (unsigned long)prs->getNumber(1);

				std::string str_callinfo_name;
				str_callinfo_name.clear();
				str_callinfo_name = prs->getString(2);

#ifdef WIN32
				printf("SelectConfCallInfo::RowNum:%I64d={%I64d, %s}\n", ullRowNum, ullcallinfoid, str_callinfo_name.c_str());
#elif defined LINUX
				printf("SelectConfCallInfo::RowNum:%lld={%lld, %s}\n", ullRowNum, ullcallinfoid, str_callinfo_name.c_str());
#endif

				if (ullcallinfoid != 0)
				{
					RollCallInfo* pRollCallInfo = new RollCallInfo();
					pRollCallInfo->m_callinfo_id = ullcallinfoid;
					pRollCallInfo->m_callinfo_name = str_callinfo_name;
					rollcallInfos.push_back(pRollCallInfo);
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

bool CConfRollCallInfo::InsertConfCallInfoToDB()
{
	CCriticalAutoLock MyLock(m_csConfrollcallinfo);

	char cInsert[1024];
#ifdef WIN32
	sprintf(cInsert, "insert into call_info(ci_id, ci_name, conf_id) values('%I64d','%s','%I64d')",
		m_callinfo_id, m_callinfo_name.c_str(), m_conf_id);
#elif defined LINUX
	sprintf(cInsert, "insert into call_info(ci_id, ci_name, conf_id) values('%lld','%s','%lld')",
		m_callinfo_id, m_callinfo_name.c_str(), m_conf_id);
#endif

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cInsert);

	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_DEBUG, "insert call_info callinfo_id:%I64d,callinfo_name:%s,conf_id:%I64d error\r\n", m_callinfo_id, m_callinfo_name.c_str(), m_conf_id, CDBConnector::GetInstance()->OutErrors());
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_DEBUG, "insert call_info callinfo_id:%lld,callinfo_name:%s,conf_id:%lld error\r\n", m_callinfo_id, m_callinfo_name.c_str(), m_conf_id, CDBConnector::GetInstance()->OutErrors());
#endif
		return false;
	}

	return true;
}

bool CConfRollCallInfo::DeleteConfCallInfoForDB()
{
	CCriticalAutoLock MyLock(m_csConfrollcallinfo);
	char cDelete[1024];
#ifdef WIN32
	sprintf(cDelete, "delete from call_info where ci_id='%I64d' and conf_id='%I64d'", m_callinfo_id, m_conf_id);
#elif defined LINUX
	sprintf(cDelete, "delete from call_info where ci_id='%lld' and conf_id='%lld'", m_callinfo_id, m_conf_id);
#endif

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cDelete);

	if (!CDBConnector::GetInstance()->ExectCommand(cDelete))
	{
		sr_printf(SR_LOGLEVEL_DEBUG, "%s error\r\n", cDelete, CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}