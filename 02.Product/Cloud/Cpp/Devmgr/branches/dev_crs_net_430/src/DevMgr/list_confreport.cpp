
#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_confreport.h"
#include <cstdlib>
#include <stdio.h>

CConfReport::CConfReport(unsigned int uiDevmgrNo)
{
	m_conf_id = 0;
	m_confreport_id = 0;
	m_starttime = "";
	m_stoptime = "";

//	time_t timeNow;
//	timeNow = time(NULL);
//	//m_confreport_id = (int)timeNow;
//
//	//组合iDevmgrNum time_t
//	m_confreport_id = (((unsigned long long)(iDevmgrNum) << 32) + timeNow) * 1000; // 乘以1000是防止两次（个）devmgr切换时间间隔较短导致ID冲突
//	m_pub_confreport_id = m_confreport_id;
//
//#ifdef WIN32
//	printf(">>>>>>>>>In CConfReport Class  iDevmgrNum>>> %d, timeNow>>> %I64d, m_confreport_id>>> %I64d\n", iDevmgrNum, timeNow, m_confreport_id);
//#elif defined LINUX
//	printf(">>>>>>>>>In CConfReport Class  iDevmgrNum>>> %d, timeNow>>> %lld, m_confreport_id>>> %lld\n", iDevmgrNum, timeNow, m_confreport_id);
//#endif
}

CConfReport::~CConfReport()
{

}

//CConfReport* pConfReport
//{
//	static CConfReport g_cConfReport;
//	return &g_cConfReport;
//}

bool CConfReport::InsertDB()
{
	CCriticalAutoLock MyLock(m_csConfRp);
	
	unsigned long long llRowNum = 0;

#ifdef WIN32
	printf(" CConfReport::InsertDB() GetResult llRowNum = %I64d, m_confreport_id = %I64d, m_conf_id = %I64d, real_starttime = %s\n", llRowNum, m_confreport_id, m_conf_id, m_starttime.c_str());
#elif defined LINUX
	printf(" CConfReport::InsertDB() GetResult llRowNum = %lld, m_confreport_id = %lld, m_conf_id = %lld, real_starttime = %s\n", llRowNum, m_confreport_id, m_conf_id, m_starttime.c_str());
#endif
	
	char cInsert[1024];
#ifdef WIN32
	//sprintf(cInsert, "insert into conf_report(conf_report_id, conf_id, real_starttime, real_endtime) values('%d','%I64d','%s','%s')", m_confreport_id, m_conf_id, m_starttime.c_str(), m_stoptime.c_str());
    //sprintf(cInsert, "insert into conf_report(conf_report_id, conf_id, real_starttime, real_endtime) values('%I64d','%I64d',NOW(), 19700101000000)", m_confreport_id, m_conf_id);
	sprintf(cInsert, "insert into conf_report(conf_report_id, conf_id, real_starttime, real_endtime) values('%I64d','%I64d','%s', 19700101000000)", m_confreport_id, m_conf_id, m_starttime.c_str());
#elif defined LINUX
	//sprintf(cInsert, "insert into conf_report(conf_report_id, conf_id, real_starttime, real_endtime) values('%d','%lld','%s','%s')", m_confreport_id, m_conf_id, m_starttime.c_str(), m_stoptime.c_str());
    //sprintf(cInsert, "insert into conf_report(conf_report_id, conf_id, real_starttime, real_endtime) values('%lld','%lld',NOW(), 19700101000000)", m_confreport_id, m_conf_id);
	sprintf(cInsert, "insert into conf_report(conf_report_id, conf_id, real_starttime, real_endtime) values('%lld','%lld','%s', 19700101000000)", m_confreport_id, m_conf_id, m_starttime.c_str());
#endif
	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_ERROR, "insert conf_report confreport_id:%I64d,conf_id:%I64d error: %s\n", m_confreport_id, m_conf_id, CDBConnector::GetInstance()->OutErrors());
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_ERROR, "insert conf_report confreport_id:%lld,conf_id:%lld error: %s\n", m_confreport_id, m_conf_id, CDBConnector::GetInstance()->OutErrors());
#endif	
		return false;
	}

	return true;
}

bool CConfReport::InsertConfReportRecordToDB()
{
	CCriticalAutoLock MyLock(m_csConfRp);
	char cInsert[1024];
#ifdef WIN32
	sprintf(cInsert, "insert into conf_report(conf_id, conf_report_id, real_starttime, real_endtime, user_id, conf_subject, conf_chairman, audio_enable, video_enable, conf_name) select conf_id,'%I64d','%s','%s',user_id,conf_subject,conf_chairman,audio_enable,video_enable,conf_name from conference where conf_id='%I64d'", m_confreport_id, m_starttime.c_str(), m_stoptime.c_str(), m_conf_id);
#elif defined LINUX
	sprintf(cInsert, "insert into conf_report(conf_id, conf_report_id, real_starttime, real_endtime, user_id, conf_subject, conf_chairman, audio_enable, video_enable, conf_name) select conf_id,'%lld','%s','%s',user_id,conf_subject,conf_chairman,audio_enable,video_enable,conf_name from conference where conf_id='%lld'", m_confreport_id, m_starttime.c_str(), m_stoptime.c_str(), m_conf_id);
#endif

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cInsert);

	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_ERROR, "insert conf_report confreport_id:%I64d,conf_id:%I64d error: %s\n", m_confreport_id, m_conf_id, CDBConnector::GetInstance()->OutErrors());
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_ERROR, "insert conf_report confreport_id:%lld,conf_id:%lld error: %s\n", m_confreport_id, m_conf_id, CDBConnector::GetInstance()->OutErrors());
#endif
		return false;
	}

	return true;
}

bool CConfReport::DeleteDB()
{
	CCriticalAutoLock MyLock(m_csConfRp);

	return true;
	char cDelete[1024];
#ifdef WIN32
	sprintf(cDelete, "delete from conf_report where conf_id = '%I64d'", m_conf_id);
#elif defined LINUX
	sprintf(cDelete, "delete from conf_report where conf_id = '%lld'", m_conf_id);
#endif	
	if (!CDBConnector::GetInstance()->ExectCommand(cDelete))
	{
		printf("delete conf_report error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CConfReport::DeleteConfReportRecordForDB()
{
	CCriticalAutoLock MyLock(m_csConfRp);

	char cDelete[1024];
#ifdef WIN32
	sprintf(cDelete, "delete from conf_report where conf_id = '%I64d' and conf_report_id = '%I64d'", m_conf_id, m_confreport_id);
#elif defined LINUX
	sprintf(cDelete, "delete from conf_report where conf_id = '%lld' and conf_report_id = '%lld'", m_conf_id, m_confreport_id);
#endif

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cDelete);

	if (!CDBConnector::GetInstance()->ExectCommand(cDelete))
	{
#ifdef WIN32
		printf("delete from conf_report where conf_id =%I64d,conf_report_id =%I64d error\r\n", m_conf_id, m_confreport_id);
#elif defined LINUX
		printf("delete from conf_report where conf_id =%lld,conf_report_id =%lld error\r\n", m_conf_id, m_confreport_id);
#endif
		sr_printf(SR_LOGLEVEL_ERROR, "delete conf_report error: %s\n", CDBConnector::GetInstance()->OutErrors());
		//printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CConfReport::UpdateDB1()
{
	CCriticalAutoLock MyLock(m_csConfRp);

	char cUpdate[1024];
#ifdef WIN32
	sprintf(cUpdate, "update conf_report set real_endtime = '%s' where conf_id='%I64d' and real_endtime='1970-01-01 00:00:00'", m_stoptime.c_str(), m_conf_id);
#elif defined LINUX
	sprintf(cUpdate, "update conf_report set real_endtime = '%s' where conf_id='%lld' and real_endtime='1970-01-01 00:00:00'", m_stoptime.c_str(), m_conf_id);
#endif	

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cUpdate);

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		//printf("update conf_report error\r\n");
		//printf("%s\n", cUpdate);
		//printf(CDBConnector::GetInstance()->OutErrors());

		sr_printf(SR_LOGLEVEL_DEBUG, "update conf_report real_endtime error: %s\n", CDBConnector::GetInstance()->OutErrors());

		return false;
	}

	return true;
}

bool CConfReport::UpdateConfRealEndtimeForDB()
{
	CCriticalAutoLock MyLock(m_csConfRp);

	char cUpdate[1024];
#ifdef WIN32
	sprintf(cUpdate, "update conf_report set real_endtime = '%s' where conf_id='%I64d' and conf_report_id = '%I64d'", m_stoptime.c_str(), m_conf_id, m_confreport_id);
#elif defined LINUX
	sprintf(cUpdate, "update conf_report set real_endtime = '%s' where conf_id='%lld' and conf_report_id = '%lld'", m_stoptime.c_str(), m_conf_id, m_confreport_id);
#endif	

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cUpdate);

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		//printf("update conf_report error\r\n");
		//printf("%s\n", cUpdate);
		//printf(CDBConnector::GetInstance()->OutErrors());

		sr_printf(SR_LOGLEVEL_ERROR, "update conf_report real_endtime error: %s\n", CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CConfReport::UpdateConfHasRecordForDB()
{
	CCriticalAutoLock MyLock(m_csConfRp);

	char cUpdate[1024];
#ifdef WIN32
	sprintf(cUpdate, "update conf_report set hasrecord='%d' where conf_id='%I64d' and conf_report_id='%I64d'", m_hasrecord, m_conf_id, m_confreport_id);
#elif defined LINUX
	sprintf(cUpdate, "update conf_report set hasrecord='%d' where conf_id='%lld' and conf_report_id='%lld'", m_hasrecord, m_conf_id, m_confreport_id);
#endif	

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cUpdate);

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		//printf("update conf_report error\r\n");
		//printf("%s\n", cUpdate);
		//printf(CDBConnector::GetInstance()->OutErrors());

		sr_printf(SR_LOGLEVEL_ERROR, "update conf_report hasrecord error: %s\n", CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CConfReport::UpdateDB2()
{
	CCriticalAutoLock MyLock(m_csConfRp);

	char cUpdate[1024];
	//sprintf(cUpdate, "update conf_report, device_conf_detail set conf_report.real_endtime = device_conf_detail.realuse_stoptime where conf_report.conf_report_id = device_conf_detail.conf_report_id and conf_report.real_endtime=''");
    sprintf(cUpdate, "update conf_report, device_conf_detail set conf_report.real_endtime = device_conf_detail.realuse_stoptime where conf_report.conf_report_id = device_conf_detail.conf_report_id and conf_report.real_endtime='1970-01-01 00:00:00'");

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update conf_report error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CConfReport::SelectDB()
{
	CCriticalAutoLock MyLock(m_csConfRp);

	//获取后，更新本地变量
	char cSelect[1024];
#ifdef WIN32
	sprintf(cSelect, "select conf_report_id,conf_id,real_starttime,real_endtime from conf_report where conf_id='%I64d' and real_endtime='1970-01-01 00:00:00' order by real_starttime desc limit 1", m_conf_id);
#elif defined LINUX
	sprintf(cSelect, "select conf_report_id,conf_id,real_starttime,real_endtime from conf_report where conf_id='%lld' and real_endtime='1970-01-01 00:00:00' order by real_starttime desc limit 1", m_conf_id);
	//printf("===-->> %s ：：%s\n", "pConfReport->SelectDB()", cSelect);
#endif	

#ifdef MYSQL_DB_DEF
	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		printf("CConfReport::SelectDB() GetResult error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^11111111^^^^^^^^^^^^^ CConfReport::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

#ifdef WIN32
	printf(" CConfReport::SelectDB() GetResult llRowNum = %I64d\n", llRowNum);
#elif defined LINUX
	printf(" CConfReport::SelectDB() GetResult llRowNum = %lld\n", llRowNum);
#endif

	//if (nRow == 1)
	if (llRowNum == 1)
	{
		//MYSQL_ROW row = CDBConnector::GetInstance()->GetRecord();
		if (row != NULL && row[0] != NULL)
		{
#ifdef WIN32
			m_confreport_id = _atoi64(row[0]);//
			printf("CConfReport::SelectDB() OK confreport_id:%s->%I64d\r\n", row[0], m_confreport_id);
#elif defined LINUX
			m_confreport_id = strtoll(row[0], NULL, 10);//
			printf("CConfReport::SelectDB() OK confreport_id:%s->%lld\r\n", row[0], m_confreport_id);
#endif
		}

		if (row != NULL && row[1] != NULL)
		{
#ifdef WIN32
			m_conf_id = _atoi64(row[1]);//
			printf("CConfReport::SelectDB() OK conf_id:%s->%I64d\r\n", row[1], m_conf_id);
#elif defined LINUX
			m_conf_id = strtoll(row[1], NULL, 10);//
			printf("CConfReport::SelectDB() OK conf_id:%s->%lld\r\n", row[1], m_conf_id);
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


	printf("^^^^^^^^2222222^^^^^^^^^^^^^ CConfReport::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF
	unsigned long long ullRowNum = 0;
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;
	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CConfReport::SelectDB error\r\n");

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
					unsigned long long ullConfreportid = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(1);
					//#ifdef WIN32
					//					ullConfreportid = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullConfreportid = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//					m_confreport_id = ullConfreportid;
					ullConfreportid = (unsigned long)prs->getNumber(1);
					m_confreport_id = ullConfreportid;

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
