#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_confrecord.h"
#include <cstdlib>
#include <stdio.h>

CConfRecord::CConfRecord(unsigned int uiDevmgrNo)
{
	//m_conf_id = 0;
	m_confreport_id = 0;
	m_confrecord_id = 0;
	m_starttime = "";
	m_stoptime = "";
	m_confname = "";
}

CConfRecord::~CConfRecord()
{

}

bool CConfRecord::InsertConfCRSRecordToDB()
{
	CCriticalAutoLock MyLock(m_csConfrec);

	char cInsert[1024];
#ifdef WIN32
	sprintf(cInsert, "insert into conf_record(conf_record_id, conf_report_id, starttime, endtime, record_alias) values('%I64d','%I64d','%s','%s','%s')", m_confrecord_id, m_confreport_id, m_starttime.c_str(), m_stoptime.c_str(), m_confname.c_str());
#elif defined LINUX
	sprintf(cInsert, "insert into conf_record(conf_record_id, conf_report_id, starttime, endtime, record_alias) values('%lld','%lld','%s','%s','%s')", m_confrecord_id, m_confreport_id, m_starttime.c_str(), m_stoptime.c_str(), m_confname.c_str());
#endif

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cInsert);

	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_ERROR, "insert conf_record conf_record_id:%I64d,conf_report_id:%I64d error: %s\n", m_confrecord_id, m_confreport_id, CDBConnector::GetInstance()->OutErrors());
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_ERROR, "insert conf_record conf_record_id:%lld,conf_report_id:%lld error: %s\n", m_confrecord_id, m_confreport_id, CDBConnector::GetInstance()->OutErrors());
#endif
		return false;
	}

	return true;
}

bool CConfRecord::UpdateConfCRSRealEndtimeForDB()
{
	CCriticalAutoLock MyLock(m_csConfrec);

	char cUpdate[1024];
#ifdef WIN32
	sprintf(cUpdate, "update conf_record set endtime = '%s' where conf_record_id ='%I64d' and conf_report_id = '%I64d'", m_stoptime.c_str(), m_confrecord_id, m_confreport_id);
#elif defined LINUX
	sprintf(cUpdate, "update conf_record set endtime = '%s' where conf_record_id ='%lld' and conf_report_id = '%lld'", m_stoptime.c_str(), m_confrecord_id, m_confreport_id);
#endif	

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cUpdate);

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "update conf_record endtime error: %s\n", CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CConfRecord::SelectConfRecordidFromDB()
{
	CCriticalAutoLock MyLock(m_csConfrec);

	//获取后，更新本地变量
	char cSelect[1024];
#ifdef WIN32
	sprintf(cSelect, "select conf_record_id,conf_report_id,starttime,endtime from conf_record where conf_report_id='%I64d' and endtime='1970-01-01 00:00:00' order by starttime desc limit 1", m_confreport_id);
#elif defined LINUX
	sprintf(cSelect, "select conf_record_id,conf_report_id,starttime,endtime from conf_record where conf_report_id='%lld' and endtime='1970-01-01 00:00:00' order by starttime desc limit 1", m_confreport_id);
	//printf("===-->> %s ：：%s\n", "pConfReport->SelectDB()", cSelect);
#endif

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		printf("CConfRecord::SelectConfRecordidFromDB() GetResult error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		printf("\n");

		printf("^^^^^^^^11111111^^^^^^^^^^^^^ CConfRecord::SelectConfRecordidFromDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

#ifdef WIN32
	printf(" CConfRecord::SelectConfRecordidFromDB() GetResult llRowNum = %I64d\n", llRowNum);
#elif defined LINUX
	printf(" CConfRecord::SelectConfRecordidFromDB() GetResult llRowNum = %lld\n", llRowNum);
#endif

	//if (nRow == 1)
	if (llRowNum == 1)
	{
		//MYSQL_ROW row = CDBConnector::GetInstance()->GetRecord();
		if (row != NULL && row[0] != NULL)
		{
#ifdef WIN32
			m_confrecord_id = _atoi64(row[0]);//
			printf("CConfRecord::SelectConfRecordidFromDB() OK confrecord_id:%s->%I64d\r\n", row[0], m_confrecord_id);
#elif defined LINUX
			m_confrecord_id = strtoll(row[0], NULL, 10);//
			printf("CConfRecord::SelectConfRecordidFromDB() OK confrecord_id:%s->%lld\r\n", row[0], m_confrecord_id);
#endif
		}

		if (row != NULL && row[1] != NULL)
		{
#ifdef WIN32
			m_confreport_id = _atoi64(row[1]);//
			printf("CConfRecord::SelectConfRecordidFromDB() OK confreport_id:%s->%I64d\r\n", row[1], m_confreport_id);
#elif defined LINUX
			m_confreport_id = strtoll(row[1], NULL, 10);//
			printf("CConfRecord::SelectConfRecordidFromDB() OK confreport_id:%s->%lld\r\n", row[1], m_confreport_id);
#endif	
		}
		if (row != NULL && row[2] != NULL)
		{
			m_starttime = row[2];
		}
		else
		{
			m_starttime.clear();
		}
		if (row != NULL && row[3] != NULL)
		{
			m_stoptime = row[3];
		}
		else
		{
			m_stoptime.clear();
		}
	}


	printf("^^^^^^^^2222222^^^^^^^^^^^^^ CConfRecord::SelectConfRecordidFromDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF
	unsigned long long ullRowNum = 0;
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;
	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CConfRecord::SelectConfRecordidFromDB error\r\n");

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
					unsigned long long ullConfrecordid = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(1);
					//#ifdef WIN32
					//					ullConfrecordid = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullConfrecordid = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//					m_confrecord_id = ullConfrecordid;
					ullConfrecordid = (unsigned long)prs->getNumber(1);
					m_confrecord_id = ullConfrecordid;

					unsigned long long ullConfreportid = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(2);
					//#ifdef WIN32
					//					ullConfreportid = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullConfreportid = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//					m_confreport_id = ullConfreportid;
					ullConfreportid = (unsigned long)prs->getNumber(2);
					m_confreport_id = ullConfreportid;

					m_starttime.clear();
					m_starttime = prs->getString(3);

					m_stoptime.clear();
					m_stoptime = prs->getString(4);

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