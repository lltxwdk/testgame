
#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_mediagroup.h"
#include <cstdlib>
#include <stdio.h>

CMediaGroup::CMediaGroup()
{
	//m_mediagroup_id = 0;
	//m_mediagroup_domain.clear();
	//m_mediagroup_desc.clear();
}

CMediaGroup::~CMediaGroup()
{
	//m_mediagroup_id = 0;
	//m_mediagroup_domain.clear();
	//m_mediagroup_desc.clear();
}

CMediaGroup* CMediaGroup::GetInstance()
{
	static CMediaGroup g_cMediaGroup;
	return &g_cMediaGroup;
}

bool CMediaGroup::InsertDB()
{
	CCriticalAutoLock MyLock(m_csMediaGroup);
	return true;
	char cInsert[1024];
	sprintf(cInsert, "insert into mediagroup(mediagroup_id, mediagroup_domain, mediagroup_desc) values('%d','%s','%s')", m_mediagroup_id, m_mediagroup_domain.c_str(), m_mediagroup_desc.c_str());
	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
		printf("insert mediagroup error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CMediaGroup::DeleteDB()
{
	CCriticalAutoLock MyLock(m_csMediaGroup);
	return true;
	char cDelete[1024];
	sprintf(cDelete, "delete from mediagroup where mediagroup_id = '%d'", m_mediagroup_id);
	if (!CDBConnector::GetInstance()->ExectCommand(cDelete))
	{
		printf("delete mediagroup error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CMediaGroup::UpdateDB()
{
	CCriticalAutoLock MyLock(m_csMediaGroup);
	return true;
	char cUpdate[1024];
	sprintf(cUpdate, "update mediagroup set mediagroup_domain = '%s', mediagroup_desc = '%s', where mediagroup_id='%d'", m_mediagroup_domain.c_str(), m_mediagroup_desc.c_str(), m_mediagroup_id);

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update mediagroup error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CMediaGroup::SelectDB()
{
	CCriticalAutoLock MyLock(m_csMediaGroup);
	//获取后，更新本地变量
	char cSelect[1024];
	sprintf(cSelect, "select mediagroup_id,mediagroup_domain,mediagroup_desc,mediagroup_createtm from mediagroup where mediagroup_id='%d'", m_mediagroup_id);

#ifdef MYSQL_DB_DEF
	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		//printf("CMediaGroup::SelectDB GetResult error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^111111111111^^^^^^^^^^^^^ CMediaGroup::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

	//#ifdef WIN32
	//	printf(" CMediaGroup::SelectDB GetResult llRowNum = %I64d\n", llRowNum);
	//#elif defined LINUX
	//	printf(" CMediaGroup::SelectDB GetResult llRowNum = %lld\n", llRowNum);
	//#endif


	//if (nRow == 1)
	if (llRowNum == 1)
	{
		//MYSQL_ROW row = CDBConnector::GetInstance()->GetRecord();
		if (row != NULL && row[0] != 0)
			m_mediagroup_id = atoi(row[0]);
		if (row != NULL && row[1] != 0)
		{
			m_mediagroup_domain = row[1];
		}
		else
		{
			m_mediagroup_domain.clear();
		}
		if (row != NULL && row[2] != 0)
		{
			m_mediagroup_desc = row[2];
		}
		else
		{
			m_mediagroup_desc.clear();
		}
	}

	printf("^^^^^^^^2222222222222^^^^^^^^^^^^^ CMediaGroup::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF
	unsigned long long ullRowNum = 0;
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;
	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CMediaGroup::SelectDB error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		if (prs != NULL)
		{
			// 注意：列的编号是从1开始,如果结果为空也会进入while循环,此时获取的值是0
			while (prs->next())
			{
				ullRowNum++;
				if (ullRowNum == 1)
				{
					int iMediagroupid = 0;
					iMediagroupid = prs->getInt(1);
					m_mediagroup_id = iMediagroupid;

					m_mediagroup_domain.clear();
					m_mediagroup_domain = prs->getString(2);

					m_mediagroup_desc.clear();
					m_mediagroup_desc = prs->getString(3);

					bgetok = true;
					break;
				}
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
