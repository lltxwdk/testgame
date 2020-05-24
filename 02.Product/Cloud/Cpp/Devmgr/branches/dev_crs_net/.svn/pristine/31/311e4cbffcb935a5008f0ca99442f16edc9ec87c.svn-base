#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_compcap.h"
#include <cstdlib>
#include <stdio.h>

CCompCapLincene::CCompCapLincene()
{
	m_compid = 0;
	m_sCompId.clear();
	mapCapbilityInfo.clear();
	m_mapCompLinceneInfo.clear();
}

CCompCapLincene::~CCompCapLincene()
{

}

std::set<int> CCompCapLincene::GetCompIdList()
{
	return m_sCompId;
}

bool CCompCapLincene::SelectLinceneIdDB()
{
	CCriticalAutoLock MyLock(m_csCCompLincene);

	char cSelect[1024];

	sprintf(cSelect, "select cap_id,col_ab from t_capbility");

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CCompLincene::SelectAllCompLinceneDB GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CCompLincene::SelectAllCompLinceneDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return false;
	}
	else
	{
		mapCapbilityInfo.clear();

		for (std::map<unsigned long long, MYSQL_ROW>::iterator row_itor = rows_set.begin();
			row_itor != rows_set.end(); row_itor++)
		{
			MYSQL_ROW ret_row = row_itor->second;
			if (ret_row)
			{

				int cap_id = 0;
				if (ret_row[0] != NULL)
				{
					cap_id = atoi(ret_row[0]);
				}

				std::string col_ab;
				if (ret_row[1] != NULL)
				{
					col_ab = ret_row[1];
				}
				else
				{
					col_ab.clear();
				}

#ifdef WIN32
				printf("SelectLinceneIdDB::RowNum:%I64d={%d, %s,}\n", (row_itor->first) + 1, cap_id, col_ab.c_str());
#elif defined LINUX
				printf("SelectLinceneIdDB::RowNum:%lld={%d, %s,}\n", (row_itor->first) + 1, cap_id, col_ab.c_str());
#endif
				
				if (col_ab.length()>0)
				{
					//将db的数据存入map表
				
					mapCapbilityInfo.insert(map<int, string>::value_type(cap_id, col_ab));
				}
			}
		}

		for (map<int, string>::iterator iter_cap = mapCapbilityInfo.begin(); iter_cap != mapCapbilityInfo.end();iter_cap++)
		{
			sr_printf(SR_LOGLEVEL_DEBUG, "CCompCapLincene::SelectLinceneIdDB() OK mapCapbilityInfo: cap_id->%d, col_ab->%s  \r\n", iter_cap->first, iter_cap->second.c_str() );
		}

	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CCompLincene::SelectAllCompLinceneDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;

#elif SHENTONG_DB_DEF
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CCompLincene::SelectAllCompLinceneDB GetResults error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		if (prs != NULL)
		{
			unsigned long long ullRowNum = 0;

			// 注意：列的编号是从1开始,如果结果为空也会进入while循环,此时获取的值是0
			while (prs->next())
			{
				ullRowNum++;

				int cap_id = 0;
				cap_id = prs->getInt(1);

				std::string col_ab;
				col_ab.clear();
				col_ab = prs->getString(2);

#ifdef WIN32
				printf("SelectPollList::RowNum:%I64d={%d, %s,}\n", ullRowNum, cap_id, col_ab.c_str());
#elif defined LINUX
				printf("SelectPollList::RowNum:%lld={%d, %s,}\n", ullRowNum, cap_id, col_ab.c_str());
#endif


				if (col_ab.length()>0)
				{
					//将db的数据存入map表
					mapCapbilityInfo.insert(map<int, string>::value_type(cap_id, col_ab));
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


bool CCompCapLincene::SelectSingleCompLinceneInfo(int compid, map<std::string, sLinceneInfo>& linceneInfo)
{
	CCriticalAutoLock MyLock(m_csCCompLincene);

	if (compid == 0)
	{
		sr_printf(SR_LOGLEVEL_INFO, "CCompCapLincene::SelectSingleCompLinceneInfo() error compid == 0: \r\n");
		return false;
	}
	char cSelect[1024];

	sprintf(cSelect, "select rel.comp_id,rel.cap_id,rel.cap_amount,rel.create_time,rel.expire_time,cap.col_ab from rel_comp_cap as rel,t_capbility as cap where cap.cap_id=rel.cap_id and rel.comp_id=%d", compid);

	//获取到所有企业的授权信息
#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CCompLincene::SelectSingleCompLinceneInfo GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CCompLincene::SelectSingleCompLinceneInfo()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

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
				m_compid = 0;
				m_compid = atoi(ret_row[0]);

				int cap_id = 0;
				if (ret_row[1] != NULL)
				{
					cap_id = atoi(ret_row[1]);
				}

				int cap_amout = 0;
				if (ret_row[2] != NULL)
				{
					cap_amout = atoi(ret_row[2]);
				}

				std::string create_time;
				if (ret_row[3] != NULL)
				{
					create_time = ret_row[3];
				}
				else
				{
					create_time.clear();
				}

				std::string expire_time;
				if (ret_row[4] != NULL)
				{
					expire_time = ret_row[4];
				}
				else
				{
					expire_time.clear();
				}

				std::string col_ab;
				if (ret_row[5] != NULL)
				{
					col_ab = ret_row[5];
				}
				else
				{
					col_ab.clear();
				}

#ifdef WIN32
				printf("SelectSingleCompLinceneInfo::RowNum:%I64d={%d, %d, %d, %s,}\n", (row_itor->first) + 1, m_compid, cap_id, cap_amout, expire_time.c_str());
#elif defined LINUX
				printf("SelectSingleCompLinceneInfo::RowNum:%lld={%d, %d, %d, %s,}\n", (row_itor->first) + 1, m_compid, cap_id, cap_amout, expire_time.c_str());
#endif

				sLinceneInfo	sTempLinceneInfo;
				std::string linceneType;
				linceneType.clear();
				std::map<int, string>::iterator mTempCap = mapCapbilityInfo.find(cap_id);
				if (mTempCap != mapCapbilityInfo.end())
				{
					linceneType = mTempCap->second;
				}

				sTempLinceneInfo.amount = cap_amout;
				sTempLinceneInfo.starttime = create_time;
				sTempLinceneInfo.expiretime = expire_time;

				map<std::string, sLinceneInfo>::iterator mlinctemp = linceneInfo.find(linceneType);
				if (mlinctemp != linceneInfo.end())
				{
					sr_printf(SR_LOGLEVEL_DEBUG, "CCompCapLincene::SelectSingleCompLinceneInfo() find comp OK :compid =%d,linceneType->%s, amount->%d, starttime->%s, expretime->%s  \r\n", m_compid, linceneType.c_str(), sTempLinceneInfo.amount, sTempLinceneInfo.starttime.c_str(), sTempLinceneInfo.expiretime.c_str());

					//找到对应的授权
					
					linceneInfo[linceneType] = sTempLinceneInfo;

				}
				else
				{
					sr_printf(SR_LOGLEVEL_DEBUG, "CCompCapLincene::SelectSingleCompLinceneInfo() find comp OK type is not ok :compid =%d,linceneType->%s, amount->%d, starttime->%s, expretime->%s  \r\n", m_compid, linceneType.c_str(), sTempLinceneInfo.amount, sTempLinceneInfo.starttime.c_str(), sTempLinceneInfo.expiretime.c_str());

					//未找到对应的授权

					linceneInfo.insert(make_pair(linceneType, sTempLinceneInfo));
				}
				
			}


		}

	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CCompLincene::SelectSingleCompLinceneInfo()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;

#elif SHENTONG_DB_DEF
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CCompLincene::SelectSingleCompLinceneInfo GetResults error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		if (prs != NULL)
		{
			unsigned long long ullRowNum = 0;


			// 注意：列的编号是从1开始,如果结果为空也会进入while循环,此时获取的值是0
			while (prs->next())
			{
				ullRowNum++;

				m_compid = 0;
				m_compid = prs->getInt(1);

				int cap_id = 0;
				cap_id = prs->getInt(2);

				int cap_amout = 0;
				cap_amout = prs->getInt(3);

				std::string create_time;
				create_time.clear();
				create_time = prs->getString(4);

				std::string expire_time;
				expire_time.clear();
				expire_time = prs->getString(5);

				std::string col_ab;
				col_ab.clear();
				col_ab = prs->getString(6);

#ifdef WIN32
				printf("SelectPollList::RowNum:%I64d={%d, %d, %d, %s}\n", ullRowNum, m_compid, cap_id, cap_amout, expire_time.c_str());
#elif defined LINUX
				printf("SelectPollList::RowNum:%lld={%d, %d, %d, %s}\n", ullRowNum, m_compid, cap_id, cap_amout, expire_time.c_str());
#endif
				sLinceneInfo	sTempLinceneInfo;
				std::string linceneType;
				linceneType.clear();
				std::map<int, string>::iterator mTempCap = mapCapbilityInfo.find(cap_id);
				if (mTempCap != mapCapbilityInfo.end())
				{
					linceneType = mTempCap->second;
				}

				sTempLinceneInfo.amount = cap_amout;
				sTempLinceneInfo.starttime = create_time;
				sTempLinceneInfo.expiretime = expire_time;

				map<std::string, sLinceneInfo>::iterator mlinctemp = linceneInfo.find(linceneType);
				if (mlinctemp != linceneInfo.end())
				{
					sr_printf(SR_LOGLEVEL_DEBUG, "CCompCapLincene::SelectSingleCompLinceneInfo() find comp OK :compid =%d,linceneType->%s, amount->%d, starttime->%s, expretime->%s  \r\n", m_compid, linceneType.c_str(), sTempLinceneInfo.amount, sTempLinceneInfo.starttime.c_str(), sTempLinceneInfo.expiretime.c_str());

					//找到对应的授权

					linceneInfo[linceneType] = sTempLinceneInfo;

				}
				else
				{
					sr_printf(SR_LOGLEVEL_DEBUG, "CCompCapLincene::SelectSingleCompLinceneInfo() find comp OK type is not ok :compid =%d,linceneType->%s, amount->%d, starttime->%s, expretime->%s  \r\n", m_compid, linceneType.c_str(), sTempLinceneInfo.amount, sTempLinceneInfo.starttime.c_str(), sTempLinceneInfo.expiretime.c_str());

					//未找到对应的授权

					linceneInfo.insert(make_pair(linceneType, sTempLinceneInfo));
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

bool CCompCapLincene::SelectCompLinceneDB()
{
	CCriticalAutoLock MyLock(m_csCCompLincene);

	char cSelect[1024];

	sprintf(cSelect, "select rel.comp_id,rel.cap_id,rel.cap_amount,rel.create_time,rel.expire_time,cap.col_ab  from rel_comp_cap rel left join t_capbility cap on rel.cap_id = cap.cap_id  where comp_id in (select comp.comp_id from t_company as comp where comp.comp_status=0)");

	//获取到所有企业的授权信息
#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CCompLincene::SelectCompLinceneDB GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CCompLincene::SelectCompLinceneDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return false;
	}
	else
	{
		m_mapCompLinceneInfo.clear();

		for (std::map<unsigned long long, MYSQL_ROW>::iterator row_itor = rows_set.begin();
			row_itor != rows_set.end(); row_itor++)
		{
			MYSQL_ROW ret_row = row_itor->second;
			if (ret_row)
			{
				m_compid = 0;
				m_compid = atoi(ret_row[0]);

				int cap_id = 0;
				if (ret_row[1] != NULL)
				{
					cap_id = atoi(ret_row[1]);
				}

				int cap_amout = 0;
				if (ret_row[2] != NULL)
				{
					cap_amout = atoi(ret_row[2]);
				}

				std::string create_time;
				if (ret_row[3] != NULL)
				{
					create_time = ret_row[3];
				}
				else
				{
					create_time.clear();
				}

				std::string expire_time;
				if (ret_row[4] != NULL)
				{
					expire_time = ret_row[4];
				}
				else
				{
					expire_time.clear();
				}

				std::string col_ab;
				if (ret_row[5] != NULL)
				{
					col_ab = ret_row[5];
				}
				else
				{
					col_ab.clear();
				}

#ifdef WIN32
				printf("SelectCompLinceneDB::RowNum:%I64d={%d, %d, %d, %s,}\n", (row_itor->first) + 1, m_compid, cap_id, cap_amout, expire_time.c_str());
#elif defined LINUX
				printf("SelectCompLinceneDB::RowNum:%lld={%d, %d, %d, %s,}\n", (row_itor->first) + 1, m_compid, cap_id, cap_amout, expire_time.c_str());
#endif

				if (m_compid != 0)
				{
					sLinceneInfo	sTempLinceneInfo;
					std::string linceneType;
					linceneType.clear();
					std::map<int, string>::iterator mTempCap = mapCapbilityInfo.find(cap_id);
					if (mTempCap != mapCapbilityInfo.end())
					{
						linceneType = mTempCap->second;
					}

					sTempLinceneInfo.amount = cap_amout;
					sTempLinceneInfo.starttime = create_time;
					sTempLinceneInfo.expiretime = expire_time;

					std::map<int, map<std::string, sLinceneInfo> >::iterator iter_comp = m_mapCompLinceneInfo.find(m_compid);
					if (iter_comp != m_mapCompLinceneInfo.end())
					{
						//找到企业
						map<std::string, sLinceneInfo>::iterator mlinctemp = iter_comp->second.find(linceneType);
						if (mlinctemp != iter_comp->second.end())
						{
							sr_printf(SR_LOGLEVEL_DEBUG, "CCompCapLincene::SelectCompLinceneDB_New() find comp OK :compid =%d,linceneType->%s, amount->%d, starttime->%s, expretime->%s  \r\n",m_compid, linceneType.c_str(), mlinctemp->second.amount, mlinctemp->second.starttime.c_str(), mlinctemp->second.expiretime.c_str());

							//找到对应的授权
							mlinctemp->second = sTempLinceneInfo;
						}
						else
						{
							sr_printf(SR_LOGLEVEL_DEBUG, "CCompCapLincene::SelectCompLinceneDB_New() find comp OK type is not ok :compid =%d,linceneType->%s, amount->%d, starttime->%s, expretime->%s  \r\n", m_compid,linceneType.c_str(), sTempLinceneInfo.amount, sTempLinceneInfo.starttime.c_str(), sTempLinceneInfo.expiretime.c_str());

							//未找到对应的授权
							map<std::string, sLinceneInfo> mTempLinceneInfo;
							mTempLinceneInfo = iter_comp->second;
							mTempLinceneInfo.insert(make_pair(linceneType, sTempLinceneInfo));
							m_mapCompLinceneInfo[m_compid] = mTempLinceneInfo;

						}
					}
					else
					{
						//未找到

						map<std::string, sLinceneInfo> mTempLinceneInfo;
						mTempLinceneInfo.insert(make_pair(linceneType, sTempLinceneInfo));

						m_mapCompLinceneInfo.insert(make_pair(m_compid, mTempLinceneInfo));

					}

					//将compid存入
					m_sCompId.insert(m_compid);

				}
			}
		}

		//打印日志确认数据的正确
		for (std::map<int, map<std::string, sLinceneInfo> >::iterator iter_log = m_mapCompLinceneInfo.begin(); iter_log != m_mapCompLinceneInfo.end(); iter_log++)
		{
			sr_printf(SR_LOGLEVEL_DEBUG, "CCompLincene::SelectCompLinceneDB_New() OK m_mapCompLinceneInfo: m_compid = %d  \r\n", iter_log->first);
			for (map<std::string, sLinceneInfo>::iterator iter_log_linc = iter_log->second.begin(); iter_log_linc != iter_log->second.end(); iter_log_linc++)
			{
				sr_printf(SR_LOGLEVEL_DEBUG, "SelectCompLinceneDB_New() OK : linceneType = %s,amount->%d, starttime->%s, expretime->%s \n", iter_log_linc->first.c_str(), iter_log_linc->second.amount, iter_log_linc->second.starttime.c_str(), iter_log_linc->second.expiretime.c_str());
			}

		}

		printf("^^^^^^^^22222222^^^^^^^^^^^^^ CCompLincene::SelectCompLinceneDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
		CDBConnector::GetInstance()->FreeResult(pQuery);
	}
	return true;

#elif SHENTONG_DB_DEF
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CCompLincene::SelectAllCompLinceneDB GetResults error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		if (prs != NULL)
		{
			unsigned long long ullRowNum = 0;
			// 注意：列的编号是从1开始,如果结果为空也会进入while循环,此时获取的值是0
			while (prs->next())
			{
				ullRowNum++;

				m_compid = 0;
				m_compid = prs->getInt(1);

				int cap_id = 0;
				cap_id = prs->getInt(2);

				int cap_amout = 0;
				cap_amout = prs->getInt(3);

				std::string create_time;
				create_time.clear();
				create_time = prs->getString(4);

				std::string expire_time;
				expire_time.clear();
				expire_time = prs->getString(5);

				std::string col_ab;
				col_ab.clear();
				col_ab = prs->getString(6);


#ifdef WIN32
				printf("SelectPollList::RowNum:%I64d={%d, %d, %d, %s,%s}\n", ullRowNum, m_compid, cap_id, cap_amout, expire_time.c_str(), col_ab.c_str());
#elif defined LINUX
				printf("SelectPollList::RowNum:%lld={%d, %d, %d, %s,%s}\n", ullRowNum, m_compid, cap_id, cap_amout, expire_time.c_str(), col_ab.c_str());
#endif


				if (m_compid != 0)
				{
					sLinceneInfo	sTempLinceneInfo;
					std::string linceneType;
					linceneType.clear();
					std::map<int, string>::iterator mTempCap = mapCapbilityInfo.find(cap_id);
					if (mTempCap != mapCapbilityInfo.end())
					{
						linceneType = mTempCap->second;
					}

					sTempLinceneInfo.amount = cap_amout;
					sTempLinceneInfo.starttime = create_time;
					sTempLinceneInfo.expiretime = expire_time;

					std::map<int, map<std::string, sLinceneInfo> >::iterator iter_comp = m_mapCompLinceneInfo.find(m_compid);
					if (iter_comp != m_mapCompLinceneInfo.end())
					{
						//找到企业
						map<std::string, sLinceneInfo>::iterator mlinctemp = iter_comp->second.find(linceneType);
						if (mlinctemp != iter_comp->second.end())
						{
							sr_printf(SR_LOGLEVEL_DEBUG, "CCompCapLincene::SelectCompLinceneDB_New() find comp OK :linceneType->%s, amount->%d, starttime->%s, expretime->%s  \r\n", linceneType.c_str(), mlinctemp->second.amount, mlinctemp->second.starttime.c_str(), mlinctemp->second.expretime.c_str());

							//找到对应的授权
							mlinctemp->second = sTempLinceneInfo;
						}
						else
						{
							sr_printf(SR_LOGLEVEL_DEBUG, "CCompCapLincene::SelectCompLinceneDB_New() find comp OK type is not ok :linceneType->%s, amount->%d, starttime->%s, expretime->%s  \r\n", linceneType.c_str(), sTempLinceneInfo.amount, sTempLinceneInfo.starttime.c_str(), sTempLinceneInfo.expretime.c_str());

							//未找到对应的授权
							map<std::string, sLinceneInfo> mTempLinceneInfo;
							mTempLinceneInfo = iter_comp->second;
							mTempLinceneInfo.insert(make_pair(linceneType, sTempLinceneInfo));
							m_mapCompLinceneInfo[m_compid] = mTempLinceneInfo;

						}
					}
					else
					{
						//未找到

						map<std::string, sLinceneInfo> mTempLinceneInfo;
						mTempLinceneInfo.insert(make_pair(linceneType, sTempLinceneInfo));

						m_mapCompLinceneInfo.insert(make_pair(m_compid, mTempLinceneInfo));

					}

					//将compid存入
					m_sCompId.insert(m_compid);
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

bool CCompCapLincene::GetCompLinceneInfo(int compid, map<std::string, sLinceneInfo>& linceneInfo)
{
	std::map<int, map<std::string, sLinceneInfo> >::iterator iter_linceneInfo;
	iter_linceneInfo = m_mapCompLinceneInfo.find(compid);
	if (iter_linceneInfo != m_mapCompLinceneInfo.end())
	{
		linceneInfo = iter_linceneInfo->second;

		return true;
	}
	return false;
}

bool CCompCapLincene::InsertDB()
{
	return true;
}

bool CCompCapLincene::UpdateDB()
{
	return true;
}

bool CCompCapLincene::DeleteDB()
{
	return true;
}