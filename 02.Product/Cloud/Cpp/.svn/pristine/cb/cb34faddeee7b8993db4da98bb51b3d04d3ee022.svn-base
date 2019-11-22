#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_datadictionary.h"
#include "sr_type.h"
//#include "list_deviceconfig.h"
#include <cstdlib>
#include <stdio.h>

CDataDictionary::CDataDictionary()
{
	//m_datadic_id = 0;
}

CDataDictionary::~CDataDictionary()
{

}

bool CDataDictionary::SelectAudioProtocol(std::list<std::string> &audioProtocols)
{
	CCriticalAutoLock MyLock(m_csDatadic);

	char cSelect[1024];
	sprintf(cSelect, "select td_value from t_datadic where td_dic_type='%s' and td_status=1", "audioProtocol");

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CDataDictionary::SelectAudioProtocol GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CDataDictionary::SelectAudioProtocol() FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return false;
	}
	else
	{
		// 处理结果

		audioProtocols.clear();
		// 测试
		for (std::map<unsigned long long, MYSQL_ROW>::iterator row_itor = rows_set.begin();
			row_itor != rows_set.end(); row_itor++)
		{
			MYSQL_ROW ret_row = row_itor->second;
			if (ret_row)
			{
				std::string str_td_value;
				str_td_value.clear();

				if (ret_row[0] != NULL)
				{
					str_td_value = ret_row[0];
				} 
				audioProtocols.push_back(str_td_value);
			}

		}
	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CDataDictionary::SelectAudioProtocol() FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF

	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CDataDictionary::SelectAudioProtocol GetResults error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		audioProtocols.clear();
		if (prs != NULL)
		{
			unsigned long long ullRowNum = 0;
			// 注意：列的编号是从1开始,如果结果为空也会进入while循环,此时获取的值是0
			while (prs->next())
			{
				ullRowNum++;
				
				std::string str_td_value;
				str_td_value.clear();

				str_td_value = prs->getString(1);

				audioProtocols.push_back(str_td_value);
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


bool CDataDictionary::SelectVideoProtocol(std::list<std::string> &videoProtocols)
{
	CCriticalAutoLock MyLock(m_csDatadic);

	char cSelect[1024];
	sprintf(cSelect, "select td_value from t_datadic where td_dic_type='%s' and td_status=1", "videoProtocol");

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CDataDictionary::SelectVideoProtocol GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CDataDictionary::SelectVideoProtocol() FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return false;
	}
	else
	{
		// 处理结果

		videoProtocols.clear();
		// 测试
		for (std::map<unsigned long long, MYSQL_ROW>::iterator row_itor = rows_set.begin();
			row_itor != rows_set.end(); row_itor++)
		{
			MYSQL_ROW ret_row = row_itor->second;
			if (ret_row)
			{
				std::string str_td_value;
				str_td_value.clear();

				if (ret_row[0] != NULL)
				{
					str_td_value = ret_row[0];
				}
				videoProtocols.push_back(str_td_value);
			}

		}
	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CDataDictionary::SelectVideoProtocol() FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF

	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CDataDictionary::SelectVideoProtocol GetResults error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		videoProtocols.clear();
		if (prs != NULL)
		{
			unsigned long long ullRowNum = 0;
			// 注意：列的编号是从1开始,如果结果为空也会进入while循环,此时获取的值是0
			while (prs->next())
			{
				ullRowNum++;

				std::string str_td_value;
				str_td_value.clear();

				str_td_value = prs->getString(1);

				videoProtocols.push_back(str_td_value);
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