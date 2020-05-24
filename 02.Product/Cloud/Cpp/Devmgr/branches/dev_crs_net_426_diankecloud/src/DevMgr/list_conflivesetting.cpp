#ifdef WIN32
#include "stdafx.h"
#endif

#include "list_conflivesetting.h"
#include <cstdlib>
#include <stdio.h>

CConfLiveSetting::CConfLiveSetting()
{
	m_liveset_id = 0;
	m_conf_id = 0;
	m_liveset_subject = "";
	m_liveset_chairman = "";
	m_liveset_abstract = "";
	m_is_public = 0;
	m_live_pwd = "";
	m_is_userec = 1;
}

CConfLiveSetting::~CConfLiveSetting()
{
	m_liveset_id = 0;
	m_conf_id = 0;
	m_liveset_subject = "";
	m_liveset_chairman = "";
	m_liveset_abstract = "";
	m_is_public = 0;
	m_live_pwd = "";
	m_is_userec = 1;
}


bool CConfLiveSetting::SelectConfLiveSettingInfo()
{
	CCriticalAutoLock MyLock(m_csConfLivesetting);


	char cSelect[1024];
#ifdef WIN32
	sprintf(cSelect, "select ls_id,conf_id,ls_subject,chairman,ls_abstract,is_public,live_pwd,is_userec from live_settings where conf_id='%I64d'", m_conf_id);
#elif defined LINUX
	sprintf(cSelect, "select ls_id,conf_id,ls_subject,chairman,ls_abstract,is_public,live_pwd,is_userec from live_settings where conf_id='%lld'", m_conf_id);
#endif

#ifdef MYSQL_DB_DEF
	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		printf("CConfLiveSetting::SelectConfLiveSettingInfo GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CConfLiveSetting::SelectConfLiveSettingInfo()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return false;
	}
	else
	{
		// 处理结果
		if (llRowNum == 1)
		{
			if (row != NULL && row[0] != NULL)
			{
#ifdef WIN32
				m_liveset_id = _atoi64(row[0]);//
				//printf("CConfLiveSetting::SelectConfLiveSettingInfo() OK liveset_id:%s->%I64d\r\n", row[0], m_liveset_id);
#elif defined LINUX
				m_liveset_id = strtoll(row[0], NULL, 10);//
				//printf("CConfLiveSetting::SelectConfLiveSettingInfo() OK liveset_id:%s->%lld\r\n", row[0], m_liveset_id);
#endif
			}
			if (row != NULL && row[1] != NULL)
			{
#ifdef WIN32
				m_conf_id = _atoi64(row[0]);//
				//printf("CConfLiveSetting::SelectConfLiveSettingInfo() OK conf_id:%s->%I64d\r\n", row[0], m_conf_id);
#elif defined LINUX
				m_conf_id = strtoll(row[0], NULL, 10);//
				//printf("CConfLiveSetting::SelectConfLiveSettingInfo() OK conf_id:%s->%lld\r\n", row[0], m_conf_id);
#endif
			}
			if (row != NULL && row[2] != NULL)
			{
				m_liveset_subject = row[2];
			}
			else
			{
				m_liveset_subject.clear();
			}
			if (row != NULL && row[3] != NULL)
			{
				m_liveset_chairman = row[3];
			}
			else
			{
				m_liveset_chairman.clear();
			}
			if (row != NULL && row[4] != NULL)
			{
				m_liveset_abstract = row[4];
			}
			else
			{
				m_liveset_abstract.clear();
			}
			if (row != NULL && row[5] != NULL)
				m_is_public = atoi(row[5]);
			if (row != NULL && row[6] != NULL)
			{
				m_live_pwd = row[6];
			}
			else
			{
				m_live_pwd.clear();
			}
			if (row != NULL && row[7] != NULL)
				m_is_userec = atoi(row[7]);
		}
	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CConfLiveSetting::SelectConfLiveSettingInfo()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF
	unsigned long long ullRowNum = 0;
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CConfLiveSetting::SelectConfLiveSettingInfo error\r\n");

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
					std::string strtemp;
					unsigned long long ullLivesetid = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(1);
					//#ifdef WIN32
					//					ullLivesetid = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullLivesetid = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//					m_liveset_id = ullLivesetid;

					ullLivesetid = (unsigned long)prs->getNumber(1);
					m_liveset_id = ullLivesetid;

					unsigned long long ullConfid = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(2);
					//#ifdef WIN32
					//					ullConfid = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullConfid = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//					m_conf_id = ullConfid;
					ullConfid = (unsigned long)prs->getNumber(2);
					m_conf_id = ullConfid;

					m_liveset_subject.clear();
					m_liveset_subject = prs->getString(3);

					m_liveset_chairman.clear();
					m_liveset_chairman = prs->getString(4);

					m_liveset_abstract.clear();
					m_liveset_abstract = prs->getString(5);

					int iIspublic = 0;
					iIspublic = prs->getInt(6);
					m_is_public = iIspublic;

					m_live_pwd.clear();
					m_live_pwd = prs->getString(7);

					int iIsUseRec = 1;
					iIsUseRec = prs->getInt(8);
					m_is_userec = iIsUseRec;

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