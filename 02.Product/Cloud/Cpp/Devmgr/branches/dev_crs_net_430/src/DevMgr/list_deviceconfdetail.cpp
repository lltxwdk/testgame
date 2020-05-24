
#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_deviceconfdetail.h"
#include <cstdlib>
#include <stdio.h>

CDeviceConfDetail::CDeviceConfDetail(unsigned int uiDevmgrNo)
{
	m_conf_id = 0;
	m_conf_report_id = 0;
	m_device_detail_id = 0;
	m_device_id = 0;
	m_realuse_starttime = "";
	m_realuse_stoptime = "";

//	time_t timeNow;
//	timeNow = time(NULL);
//	//m_conf_report_id = (int)timeNow;
//	//m_device_detail_id = (int)timeNow;
//
//	//组合iDevmgrNum time_t
//	m_conf_report_id = (((unsigned long long)(iDevmgrNum) << 32) + timeNow) * 1000; // 乘以1000是防止两次（个）devmgr切换时间间隔较短导致ID冲突
//	m_device_detail_id = (((unsigned long long)(iDevmgrNum) << 32) + timeNow) * 1000; // 乘以1000是防止两次（个）devmgr切换时间间隔较短导致ID冲突
//	m_pub_device_detail_id = m_device_detail_id;
//
//#ifdef WIN32
//	printf(">>>>>>>>>In CDeviceConfDetail Class  iDevmgrNum>>> %d, timeNow>>> %I64d, m_conf_report_id>>> %I64d, m_device_detail_id>>> %I64d\n", iDevmgrNum, timeNow, m_conf_report_id, m_device_detail_id);
//#elif defined LINUX
//	printf(">>>>>>>>>In CDeviceConfDetail Class  iDevmgrNum>>> %d, timeNow>>> %lld, m_conf_report_id>>> %lld, m_device_detail_id>>> %lld\n", iDevmgrNum, timeNow, m_conf_report_id, m_device_detail_id);
//#endif
}

CDeviceConfDetail::~CDeviceConfDetail()
{

}

//CDeviceConfDetail* pDeviceConfDetail
//{
//	static CDeviceConfDetail g_cDeviceConfDetail;
//	return &g_cDeviceConfDetail;
//}

bool CDeviceConfDetail::InsertDB()
{
	CCriticalAutoLock MyLock(m_csDevConfDetail);

	////m_device_detail_id = 1;
	////插入前，获取当前数据库最大index
	//if (!CDBConnector::GetInstance()->SelectRecord("select * from device_conf_detail"))
	//{
	//	m_device_detail_id = 1;
	//}
	//else
	//{
	//	long long nRow = CDBConnector::GetInstance()->GetRowNum();
	//	if (nRow == 0)
	//		m_device_detail_id = 1;
	//	else
	//	{
	//		m_device_detail_id = nRow;
	//		m_device_detail_id++;
	//	}
	//}


	unsigned long long llRowNum = 0;
	//MYSQL_ROW row;
	//char cSelect[1024];
	//sprintf(cSelect, "select * from device_conf_detail");
	//MYSQL_RES *pQuery = NULL;

	//if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, pQuery))
	//{
	//	//printf("CDeviceConfDetail::InsertDB GetResult error\r\n");
	//	printf(CDBConnector::GetInstance()->OutErrors());
	//	m_device_detail_id = 1;
	//}
	//else
	//{
	//	if (llRowNum == 0)
	//	{
	//		m_device_detail_id = 1;
	//	}
	//	else
	//	{
	//		m_device_detail_id = (int)llRowNum;
	//		m_device_detail_id++;
	//	}
	//}

	//CDBConnector::GetInstance()->FreeResult(pQuery);

	//m_device_detail_id = m_pub_device_detail_id++;

#ifdef WIN32
	printf(" CDeviceConfDetail::InsertDB GetResult llRowNum = %I64d, m_conf_id =  %I64d, m_conf_report_id =  %I64d, m_device_detail_id =  %I64d, m_device_id =  %d, m_realuse_starttime =  %s\n", llRowNum, m_conf_id, m_conf_report_id, m_device_detail_id, m_device_id, m_realuse_starttime.c_str());
#elif defined LINUX
	printf(" CDeviceConfDetail::InsertDB GetResult llRowNum = %lld, m_conf_id =  %lld, m_conf_report_id =  %lld, m_device_detail_id =  %lld, m_device_id =  %d, m_realuse_starttime =  %s\n", llRowNum, m_conf_id, m_conf_report_id, m_device_detail_id, m_device_id, m_realuse_starttime.c_str());
#endif
	

	char cInsert[1024];
#ifdef WIN32
	//sprintf(cInsert, "insert into device_conf_detail(conf_id, conf_report_id, device_detail_id, device_id, realuse_starttime, realuse_stoptime) values('%I64d','%d','%d','%d','%s','%s')",
		//m_conf_id, m_conf_report_id, m_device_detail_id, m_device_id, m_realuse_starttime.c_str(), m_realuse_stoptime.c_str());
    //sprintf(cInsert, "insert into device_conf_detail(conf_id, conf_report_id, device_detail_id, device_id, realuse_starttime, realuse_stoptime) values('%I64d','%I64d','%I64d','%d',NOW(),19700101000000)",
    //    m_conf_id, m_conf_report_id, m_device_detail_id, m_device_id);

	sprintf(cInsert, "insert into device_conf_detail(conf_id, conf_report_id, device_detail_id, device_id, realuse_starttime, realuse_stoptime) values('%I64d','%I64d','%I64d','%d','%s',19700101000000)",
		m_conf_id, m_conf_report_id, m_device_detail_id, m_device_id, m_realuse_starttime.c_str());
#elif defined LINUX
	//sprintf(cInsert, "insert into device_conf_detail(conf_id, conf_report_id, device_detail_id, device_id, realuse_starttime, realuse_stoptime) values('%lld','%d','%d','%d','%s','%s')", 
		//m_conf_id, m_conf_report_id, m_device_detail_id, m_device_id, m_realuse_starttime.c_str(), m_realuse_stoptime.c_str());
    //sprintf(cInsert, "insert into device_conf_detail(conf_id, conf_report_id, device_detail_id, device_id, realuse_starttime, realuse_stoptime) values('%lld','%lld','%lld','%d',NOW(),19700101000000)",
    //    m_conf_id, m_conf_report_id, m_device_detail_id, m_device_id);

	sprintf(cInsert, "insert into device_conf_detail(conf_id, conf_report_id, device_detail_id, device_id, realuse_starttime, realuse_stoptime) values('%lld','%lld','%lld','%d','%s',19700101000000)",
		m_conf_id, m_conf_report_id, m_device_detail_id, m_device_id, m_realuse_starttime.c_str());
#endif	
	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		printf("insert device_conf_detail device_detail_id:%I64d,conf_report_id:%I64d,conf_id:%I64d,device_id:%d error\r\n", m_device_detail_id, m_conf_report_id, m_conf_id, m_device_id);
#elif defined LINUX
		printf("insert device_conf_detail device_detail_id:%lld,conf_report_id:%lld,conf_id:%lld,device_id:%d error\r\n", m_device_detail_id, m_conf_report_id, m_conf_id, m_device_id);
#endif	
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDeviceConfDetail::InsertDeviceConfDetailToDB()
{
	CCriticalAutoLock MyLock(m_csDevConfDetail);

	unsigned long long llRowNum = 0;

	char cInsert[1024];
#ifdef WIN32
	sprintf(cInsert, "insert into device_conf_detail(conf_id, conf_report_id, device_detail_id, device_id, realuse_starttime, realuse_stoptime) values('%I64d','%I64d','%I64d','%d','%s','%s')",
	m_conf_id, m_conf_report_id, m_device_detail_id, m_device_id, m_realuse_starttime.c_str(), m_realuse_stoptime.c_str());
#elif defined LINUX
	sprintf(cInsert, "insert into device_conf_detail(conf_id, conf_report_id, device_detail_id, device_id, realuse_starttime, realuse_stoptime) values('%lld','%lld','%lld','%d','%s','%s')",
		m_conf_id, m_conf_report_id, m_device_detail_id, m_device_id, m_realuse_starttime.c_str(), m_realuse_stoptime.c_str());
#endif	

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cInsert);

	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_ERROR, "insert device_conf_detail device_detail_id:%I64d,conf_report_id:%I64d,conf_id:%I64d,device_id:%d error:%s\n", m_device_detail_id, m_conf_report_id, m_conf_id, m_device_id, CDBConnector::GetInstance()->OutErrors());
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_ERROR, "insert device_conf_detail device_detail_id:%lld,conf_report_id:%lld,conf_id:%lld,device_id:%d error:%s\n", m_device_detail_id, m_conf_report_id, m_conf_id, m_device_id, CDBConnector::GetInstance()->OutErrors());
#endif	
		return false;
	}

	return true;
}

bool CDeviceConfDetail::DeleteDB()
{
	CCriticalAutoLock MyLock(m_csDevConfDetail);

	return true;
	char cDelete[1024];
/*	sprintf(cDelete, "delete from device where device_id = '%d'", m_device_id);*/
	if (!CDBConnector::GetInstance()->ExectCommand(cDelete))
	{
		printf("delete device_conf_detail error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDeviceConfDetail::UpdateDB1()
{
	CCriticalAutoLock MyLock(m_csDevConfDetail);

	char cUpdate[1024];
#ifdef WIN32
	//sprintf(cUpdate, "update device_conf_detail set realuse_stoptime = '%s' where conf_id='%I64d' and device_id='%d' and realuse_stoptime=''", m_realuse_stoptime.c_str(), m_conf_id, m_device_id);
    //sprintf(cUpdate, "update device_conf_detail set realuse_stoptime = NOW() where conf_id='%I64d' and device_id='%d' and realuse_stoptime='1970-01-01 00:00:00'", m_conf_id, m_device_id);

	sprintf(cUpdate, "update device_conf_detail set realuse_stoptime = '%s' where conf_id='%I64d' and device_id='%d' and realuse_stoptime='1970-01-01 00:00:00'", m_realuse_stoptime.c_str(), m_conf_id, m_device_id);
#elif defined LINUX
	//sprintf(cUpdate, "update device_conf_detail set realuse_stoptime = '%s' where conf_id='%lld' and device_id='%d' and realuse_stoptime=''", m_realuse_stoptime.c_str(), m_conf_id, m_device_id);
    //sprintf(cUpdate, "update device_conf_detail set realuse_stoptime = NOW() where conf_id='%lld' and device_id='%d' and realuse_stoptime='1970-01-01 00:00:00'", m_conf_id, m_device_id);

	sprintf(cUpdate, "update device_conf_detail set realuse_stoptime = '%s' where conf_id='%lld' and device_id='%d' and realuse_stoptime='1970-01-01 00:00:00'", m_realuse_stoptime.c_str(), m_conf_id, m_device_id);
#endif	

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update device_conf_detail error\r\n");
		printf("%s\n", cUpdate);
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDeviceConfDetail::UpdateDB2()
{
	CCriticalAutoLock MyLock(m_csDevConfDetail);

	char cUpdate[1024];
#ifdef WIN32
	//sprintf(cUpdate, "update device_conf_detail set realuse_stoptime = '%s' where device_id='%d' and realuse_stoptime=''", m_realuse_stoptime.c_str(), m_device_id);
    sprintf(cUpdate, "update device_conf_detail set realuse_stoptime = NOW() where device_id='%d' and realuse_stoptime='1970-01-01 00:00:00'", m_device_id);
#elif defined LINUX
	//sprintf(cUpdate, "update device_conf_detail set realuse_stoptime = '%s' where device_id='%d' and realuse_stoptime=''", m_realuse_stoptime.c_str(), m_device_id);
    sprintf(cUpdate, "update device_conf_detail set realuse_stoptime = NOW() where device_id='%d' and realuse_stoptime='1970-01-01 00:00:00'", m_device_id);
#endif	

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update device_conf_detail error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDeviceConfDetail::SelectDB()
{
	CCriticalAutoLock MyLock(m_csDevConfDetail);

	//获取后，更新本地变量
	char cSelect[1024];
#ifdef WIN32
	sprintf(cSelect, "select device_detail_id,conf_id,conf_report_id,device_id,realuse_starttime,realuse_stoptime from device_conf_detail where conf_id='%I64d' and device_id='%d' and realuse_stoptime='1970-01-01 00:00:00'", m_conf_id, m_device_id);
#elif defined LINUX
	sprintf(cSelect, "select device_detail_id,conf_id,conf_report_id,device_id,realuse_starttime,realuse_stoptime from device_conf_detail where conf_id='%lld' and device_id='%d' and realuse_stoptime='1970-01-01 00:00:00'", m_conf_id, m_device_id);
#endif	

#ifdef MYSQL_DB_DEF
	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		printf("CDeviceConfDetail::SelectDB GetResult error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^111111111111^^^^^^^^^^^^^ CDeviceConfDetail::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

	//#ifdef WIN32
	//	printf(" CDeviceConfDetail::SelectDB GetResult llRowNum = %I64d\n", llRowNum);
	//#elif defined LINUX
	//	printf(" CDeviceConfDetail::SelectDB GetResult llRowNum = %lld\n", llRowNum);
	//#endif



	//if (nRow == 1)
	if (llRowNum == 1)
	{
		//MYSQL_ROW row = CDBConnector::GetInstance()->GetRecord();
		if (row != NULL && row[0] != NULL)
		{
#ifdef WIN32
			m_device_detail_id = _atoi64(row[0]);//
			printf("CDeviceConfDetail::SelectDB() OK device_detail_id:%s->%I64d\r\n", row[0], m_device_detail_id);
#elif defined LINUX
			m_device_detail_id = strtoll(row[0], NULL, 10);//
			printf("CDeviceConfDetail::SelectDB() OK device_detail_id:%s->%lld\r\n", row[0], m_device_detail_id);
#endif
		}
		if (row != NULL && row[1] != NULL)
		{
#ifdef WIN32
			m_conf_id = _atoi64(row[1]);//
			printf("CDeviceConfDetail::SelectDB() OK conf_id:%s->%I64d\r\n", row[1], m_conf_id);
#elif defined LINUX
			m_conf_id = strtoll(row[1], NULL, 10);//
			printf("CDeviceConfDetail::SelectDB() OK conf_id:%s->%lld\r\n", row[1], m_conf_id);
#endif
		}
		if (row != NULL && row[2] != NULL)
		{
#ifdef WIN32
			m_conf_report_id = _atoi64(row[2]);//
			printf("CDeviceConfDetail::SelectDB() OK conf_report_id:%s->%I64d\r\n", row[2], m_conf_report_id);
#elif defined LINUX
			m_conf_report_id = strtoll(row[2], NULL, 10);//
			printf("CDeviceConfDetail::SelectDB() OK conf_report_id:%s->%lld\r\n", row[2], m_conf_report_id);
#endif
		}
		if (row != NULL && row[3] != NULL)
			m_device_id = atoi(row[3]);
		if (row != NULL && row[4] != NULL)
		{
			m_realuse_starttime = row[4];
		}
		else
		{
			m_realuse_starttime.clear();
		}
		if (row != NULL && row[5] != NULL)
		{
			m_realuse_stoptime = row[5];
		}
		else
		{
			m_realuse_stoptime.clear();
		}
	}

	printf("^^^^^^^^2222222222^^^^^^^^^^^^^ CDeviceConfDetail::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);
	return true;
#elif SHENTONG_DB_DEF
	unsigned long long ullRowNum = 0;
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;
	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CDeviceConfDetail::SelectDB error\r\n");

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
					unsigned long long ullDevicedetailid = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(1);
					//#ifdef WIN32
					//					ullDevicedetailid = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullDevicedetailid = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//					m_device_detail_id = ullDevicedetailid;
					ullDevicedetailid = (unsigned long)prs->getNumber(1);
					m_device_detail_id = ullDevicedetailid;

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

					unsigned long long ullConfreportid = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(3);
					//#ifdef WIN32
					//					ullConfreportid = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullConfreportid = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//					m_conf_report_id = ullConfreportid;
					ullConfreportid = (unsigned long)prs->getNumber(3);
					m_conf_report_id = ullConfreportid;

					int iDeviceid = 0;
					iDeviceid = prs->getInt(4);
					m_device_id = iDeviceid;

					m_realuse_starttime.clear();
					m_realuse_starttime = prs->getString(5);

					m_realuse_stoptime.clear();
					m_realuse_stoptime = prs->getString(6);

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
