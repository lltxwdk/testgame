
#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_userconfdetail.h"
#include <cstdlib>
#include <stdio.h>

CUserConfDetail::CUserConfDetail(unsigned int uiDevmgrNo)
{
	m_conf_id = 0;
	m_conf_report_id = 0;
	m_userrpt_detail_id = 0;
	m_user_id = 0;
	m_useralias = "";
	m_realjointime = "";
	m_realleavetime = "";

//	time_t timeNow;
//	timeNow = time(NULL);
//	//m_conf_report_id = (int)timeNow;
//	//m_userrpt_detail_id = (int)timeNow;
//
//	//组合iDevmgrNum time_t
//	m_conf_report_id = (((unsigned long long)(iDevmgrNum) << 32) + timeNow) * 1000; // 乘以1000是防止两次（个）devmgr切换时间间隔较短导致ID冲突
//	m_userrpt_detail_id = (((unsigned long long)(iDevmgrNum) << 32) + timeNow) * 1000; // 乘以1000是防止两次（个）devmgr切换时间间隔较短导致ID冲突
//	m_pub_userrpt_detail_id = m_userrpt_detail_id;
//
//#ifdef WIN32
//	printf(">>>>>>>>>In CUserConfDetail Class  iDevmgrNum>>> %d, timeNow>>> %I64d, m_conf_report_id>>> %I64d, m_userrpt_detail_id>>> %I64d\n", iDevmgrNum, timeNow, m_conf_report_id, m_userrpt_detail_id);
//#elif defined LINUX
//	printf(">>>>>>>>>In CUserConfDetail Class  iDevmgrNum>>> %d, timeNow>>> %lld, m_conf_report_id>>> %lld, m_userrpt_detail_id>>> %lld\n", iDevmgrNum, timeNow, m_conf_report_id, m_userrpt_detail_id);
//#endif
}

CUserConfDetail::~CUserConfDetail()
{

}

//CUserConfDetail* CUserConfDetail::GetInstance()
//{
//	static CUserConfDetail g_cUserConfDetail;
//	return &g_cUserConfDetail;
//}

bool CUserConfDetail::InsertDB()
{
	CCriticalAutoLock MyLock(m_csUCD);

#ifdef MYSQL_DB_DEF
	bool bTempUser = false;
	//m_device_detail_id = 1;
	//插入前，先判断userid是否合法
	char cSelect[1024];
	char userbuf[16];
	sprintf(userbuf, "%d", m_user_id);
	//if (userbuf[0] != '4')
	sprintf(cSelect, "select * from user where user_id='%d'", m_user_id);
	//else
	//{
	//	bTempUser = true;
	//	sprintf(cSelect, "select * from user_temp where usert_id='%d'", m_user_id);
	//}


	//if (!CDBConnector::GetInstance()->SelectRecord(cSelect))
	//{
	//	return false;
	//}
	//else{
	//	long long nRow = CDBConnector::GetInstance()->GetRowNum();
	//	if (nRow == 0)
	//		return false;
	//}

	{
		unsigned long long llRowNum = 0;
		MYSQL_ROW row;
		MYSQL_RES *pQuery = NULL;

		if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
		{
			printf("CUserConfDetail::InsertDB GetResult error\r\n");
			printf(CDBConnector::GetInstance()->OutErrors());

			printf("^^^^^^^^111111111111^^^^^^^^^^^^^ CUserConfDetail::InsertDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

			CDBConnector::GetInstance()->FreeResult(pQuery);
			return false;
		}
		else
		{
			if (llRowNum == 0)
			{
				printf("CUserConfDetail::InsertDB GetResult error llRowNum == 0\r\n");

				printf("^^^^^^^^222222222222^^^^^^^^^^^^^ CUserConfDetail::InsertDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

				CDBConnector::GetInstance()->FreeResult(pQuery);
				return false;
			}
		}

		printf("^^^^^^^^3333333333333333^^^^^^^^^^^^^ CUserConfDetail::InsertDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
		CDBConnector::GetInstance()->FreeResult(pQuery);

#ifdef WIN32
		printf(" CUserConfDetail::InsertDB GetResult(m_user_id = %d) llRowNum = %I64d\n", m_user_id, llRowNum);
#elif defined LINUX
		printf(" CUserConfDetail::InsertDB GetResult(m_user_id = %d) llRowNum = %lld\n", m_user_id, llRowNum);
#endif
	}

	////插入前，获取当前数据库最大index
	//if (!CDBConnector::GetInstance()->SelectRecord("select * from user_conf_detail"))
	//{
	//	m_userrpt_detail_id = 1;
	//}
	//else
	//{
	//	long long nRow = CDBConnector::GetInstance()->GetRowNum();
	//	if (nRow == 0)
	//		m_userrpt_detail_id = 1;
	//	else
	//	{
	//		m_userrpt_detail_id = nRow;
	//		m_userrpt_detail_id++;
	//	}
	//}

	{
		unsigned long long llRowNum = 0;
		//MYSQL_ROW row;
		//char cSelect2[1024];
		//sprintf(cSelect2, "select * from user_conf_detail");
		//MYSQL_RES *pQuery = NULL;

		//if (!CDBConnector::GetInstance()->GetResult(cSelect2, llRowNum, row, pQuery))
		//{
		//	printf("CUserConfDetail::InsertDB GetResult user_conf_detail error\r\n");
		//	printf(CDBConnector::GetInstance()->OutErrors());
		//	m_userrpt_detail_id = 1;
		//}
		//else
		//{
		//	if (llRowNum == 0)
		//	{
		//		m_userrpt_detail_id = 1;
		//	}
		//	else
		//	{
		//		m_userrpt_detail_id = (int)llRowNum;
		//		m_userrpt_detail_id++;
		//	}
		//}

		//CDBConnector::GetInstance()->FreeResult(pQuery);

		//m_userrpt_detail_id = m_pub_userrpt_detail_id++;

#ifdef WIN32
		printf(" CUserConfDetail::InsertDB GetResult user_conf_detail llRowNum = %I64d, m_conf_id = %I64d, m_conf_report_id = %I64d, m_userrpt_detail_id = %I64d, m_user_id = %d, m_useralias = %s, m_realjointime = %s\n", llRowNum, m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str(), m_realjointime.c_str());
#elif defined LINUX
		printf(" CUserConfDetail::InsertDB GetResult user_conf_detail llRowNum = %lld, m_conf_id = %lld, m_conf_report_id = %lld, m_userrpt_detail_id = %lld, m_user_id = %d, m_useralias = %s, m_realjointime = %s\n", llRowNum, m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str(), m_realjointime.c_str());
#endif
	}


	char cInsert[1024];
#ifdef WIN32
	//sprintf(cInsert, "insert into user_conf_detail(conf_id, conf_report_id, use_rpt_detaild_id, user_id, user_alias, real_jointime, real_leavetime) values('%I64d','%d','%d','%d','%s','%s','%s')",
	//m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str() ,m_realjointime.c_str(), m_realleavetime.c_str());
	//sprintf(cInsert, "insert into user_conf_detail(conf_id, conf_report_id, use_rpt_detaild_id, user_id_ext, user_alias, real_jointime, real_leavetime) values('%I64d','%I64d','%I64d','%d','%s',NOW(),19700101000000)",
	//	m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str());

	sprintf(cInsert, "insert into user_conf_detail(conf_id, conf_report_id, use_rpt_detaild_id, user_id_ext, user_alias, real_jointime, real_leavetime) values('%I64d','%I64d','%I64d','%d','%s','%s',19700101000000)",
		m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str(), m_realjointime.c_str());

#elif defined LINUX
	//sprintf(cInsert, "insert into user_conf_detail(conf_id, conf_report_id, use_rpt_detaild_id, user_id, user_alias, real_jointime, real_leavetime) values('%lld','%d','%d','%d','%s','%s','%s')", 
	//m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str() ,m_realjointime.c_str(), m_realleavetime.c_str());
	//sprintf(cInsert, "insert into user_conf_detail(conf_id, conf_report_id, use_rpt_detaild_id, user_id_ext, user_alias, real_jointime, real_leavetime) values('%lld','%lld','%lld','%d','%s',NOW(),19700101000000)",
	//    m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str());

	sprintf(cInsert, "insert into user_conf_detail(conf_id, conf_report_id, use_rpt_detaild_id, user_id_ext, user_alias, real_jointime, real_leavetime) values('%lld','%lld','%lld','%d','%s','%s',19700101000000)",
		m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str(), m_realjointime.c_str());
#endif	
	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_ERROR, "insert user_conf_detail m_conf_id:%I64d, m_conf_report_id:%I64d, m_userrpt_detail_id:%I64d, m_user_id:%d, m_useralias: %s error: %s\n", m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str(), CDBConnector::GetInstance()->OutErrors());
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_ERROR, "insert user_conf_detail m_conf_id:%lld, m_conf_report_id:%lld, m_userrpt_detail_id:%lld, m_user_id:%d, m_useralias:%s error: %s\n", m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str(), CDBConnector::GetInstance()->OutErrors());
#endif
		return false;
	}

	//if (bTempUser)
	//{
	//	//更新临时user_temp库
	//	UpdateDB3(1);
	//}

#elif SHENTONG_DB_DEF

#endif

	return true;
}

bool CUserConfDetail::InsertUserConfDetailToDB()
{
	CCriticalAutoLock MyLock(m_csUCD);

	char cInsert[1024];
#ifdef WIN32
	sprintf(cInsert, "insert into user_conf_detail(conf_id, conf_report_id, use_rpt_detaild_id, user_id_ext, user_alias, real_jointime, real_leavetime, user_fromtype, user_termtype) values('%I64d','%I64d','%I64d','%d','%s','%s','%s','%d','%d')",
		m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str(), m_realjointime.c_str(), m_realleavetime.c_str(), m_fromtype, m_termtype);
#elif defined LINUX
	sprintf(cInsert, "insert into user_conf_detail(conf_id, conf_report_id, use_rpt_detaild_id, user_id_ext, user_alias, real_jointime, real_leavetime, user_fromtype, user_termtype) values('%lld','%lld','%lld','%d','%s','%s','%s','%d','%d')", 
		m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str() ,m_realjointime.c_str(), m_realleavetime.c_str(), m_fromtype, m_termtype);
#endif	

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cInsert);

	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_ERROR, "insert user_conf_detail m_conf_id:%I64d, m_conf_report_id:%I64d, m_userrpt_detail_id:%I64d, m_user_id:%d, m_useralias: %s error: %s\n", m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str(), CDBConnector::GetInstance()->OutErrors());
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_ERROR, "insert user_conf_detail m_conf_id:%lld, m_conf_report_id:%lld, m_userrpt_detail_id:%lld, m_user_id:%d, m_useralias:%s error: %s\n", m_conf_id, m_conf_report_id, m_userrpt_detail_id, m_user_id, m_useralias.c_str(), CDBConnector::GetInstance()->OutErrors());
#endif
		return false;
	}

	return true;
}

bool CUserConfDetail::DeleteDB()
{
	CCriticalAutoLock MyLock(m_csUCD);

	return true;
	char cDelete[1024];
/*	sprintf(cDelete, "delete from user_conf_detail where device_id = '%d'", m_device_id);*/
	if (!CDBConnector::GetInstance()->ExectCommand(cDelete))
	{
		printf("delete user_conf_detail error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CUserConfDetail::UpdateDB1()
{
	CCriticalAutoLock MyLock(m_csUCD);

	char cUpdate[1024];
#ifdef WIN32
	//sprintf(cUpdate, "update user_conf_detail set real_leavetime='%s' where user_id='%d' and conf_id='%I64d' and real_leavetime=''", m_realleavetime.c_str(), m_user_id, m_conf_id);
    //sprintf(cUpdate, "update user_conf_detail set real_leavetime=NOW() where user_id_ext='%d' and conf_id='%I64d' and real_leavetime='1970-01-01 00:00:00'", m_user_id, m_conf_id);
	sprintf(cUpdate, "update user_conf_detail set real_leavetime='%s' where user_id_ext='%d' and conf_id='%I64d' and real_leavetime='1970-01-01 00:00:00'", m_realleavetime.c_str(), m_user_id, m_conf_id);
#elif defined LINUX
	//sprintf(cUpdate, "update user_conf_detail set real_leavetime='%s' where user_id='%d' and conf_id='%lld' and real_leavetime=''", m_realleavetime.c_str(), m_user_id, m_conf_id);
    //sprintf(cUpdate, "update user_conf_detail set real_leavetime=NOW() where user_id_ext='%d' and conf_id='%lld' and real_leavetime='1970-01-01 00:00:00'", m_user_id, m_conf_id);
	sprintf(cUpdate, "update user_conf_detail set real_leavetime='%s' where user_id_ext='%d' and conf_id='%lld' and real_leavetime='1970-01-01 00:00:00'", m_realleavetime.c_str(), m_user_id, m_conf_id);
#endif	

//#ifdef WIN32
//	printf(" CDeviceConfDetail::UpdateDB1  user_conf_detail set real_leavetime=%s, where user_id_ext=%d, conf_id=%I64d\n", m_realleavetime.c_str(), m_user_id, m_conf_id);
//#elif defined LINUX
//	printf(" CDeviceConfDetail::UpdateDB1  user_conf_detail set real_leavetime=%s, where user_id_ext=%d, conf_id=%lld\n", m_realleavetime.c_str(), m_user_id, m_conf_id);
//#endif

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update user_conf_detail error\r\n");
		printf("%s\n", cUpdate);
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	//char userbuf[16];
	//sprintf(userbuf, "%d", m_user_id);
	//if (userbuf[0] == '4')
	//	UpdateDB3(1);

	return true;
}

bool CUserConfDetail::UpdateDB2()
{
	CCriticalAutoLock MyLock(m_csUCD);

	char cUpdate[1024];
#ifdef WIN32
	//sprintf(cUpdate, "update user_conf_detail set user_alias='%s', real_jointime='%s' where user_id='%d' and conf_id='%I64d' and real_leavetime=''", m_useralias.c_str(), m_realjointime.c_str(), m_user_id, m_conf_id);
    sprintf(cUpdate, "update user_conf_detail set user_alias='%s', real_jointime=NOW() where user_id_ext='%d' and conf_id='%I64d' and real_leavetime='1970-01-01 00:00:00'", m_useralias.c_str(), m_user_id, m_conf_id);
#elif defined LINUX
	//sprintf(cUpdate, "update user_conf_detail set user_alias='%s', real_jointime='%s' where user_id='%d' and conf_id='%lld' and real_leavetime=''", m_useralias.c_str(), m_realjointime.c_str(), m_user_id, m_conf_id);
    sprintf(cUpdate, "update user_conf_detail set user_alias='%s', real_jointime=NOW() where user_id_ext='%d' and conf_id='%lld' and real_leavetime='1970-01-01 00:00:00'", m_useralias.c_str(), m_user_id, m_conf_id);
#endif	

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update user_conf_detail error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	//char userbuf[16];
	//sprintf(userbuf, "%d", m_user_id);
	//if (userbuf[0] == '4')
	//	UpdateDB3(0);

	return true;
}

bool CUserConfDetail::UpdateDB3(int status)
{
	CCriticalAutoLock MyLock(m_csUCD);

	//更新临时user_temp库
	char cUpdate[1024];
	sprintf(cUpdate, "update user_temp set online_status='%d' where usert_id='%d'", 1, m_user_id);
	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update user_temp usert_id ='%d', error\r\n", m_user_id);
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}
	return true;
}

bool CUserConfDetail::UpdateDB4()
{
	CCriticalAutoLock MyLock(m_csUCD);

	char cUpdate[1024];
	//sprintf(cUpdate, "update user_conf_detail, device_conf_detail set user_conf_detail.real_leavetime = device_conf_detail.realuse_stoptime where user_conf_detail.conf_report_id = device_conf_detail.conf_report_id and user_conf_detail.real_leavetime=''");
    sprintf(cUpdate, "update user_conf_detail, device_conf_detail set user_conf_detail.real_leavetime = device_conf_detail.realuse_stoptime where user_conf_detail.conf_report_id = device_conf_detail.conf_report_id and user_conf_detail.real_leavetime='1970-01-01 00:00:00'");
	//printf(" ==CUserConfDetail::UpdateDB4 ==-->> %s\n", cUpdate);
	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update user_conf_detail error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}
	return true;
}

bool CUserConfDetail::SelectDB()
{
	CCriticalAutoLock MyLock(m_csUCD);

	//获取后，更新本地变量
	char cSelect[1024];
#ifdef WIN32
	sprintf(cSelect, "select conf_id,conf_report_id,use_rpt_detaild_id,user_id_ext,user_alias,real_jointime,real_leavetime from user_conf_detail where conf_id='%I64d' && user_id_ext='%d' && real_leavetime='1970-01-01 00:00:00'", m_conf_id, m_user_id);
#elif defined LINUX
	sprintf(cSelect, "select conf_id,conf_report_id,use_rpt_detaild_id,user_id_ext,user_alias,real_jointime,real_leavetime from user_conf_detail where conf_id='%lld' && user_id_ext='%d' && real_leavetime='1970-01-01 00:00:00'", m_conf_id, m_user_id);
#endif	

#ifdef MYSQL_DB_DEF
	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		printf("CUserConfDetail::SelectDB GetResult error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^11111111111^^^^^^^^^^^^^ CUserConfDetail::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

	//#ifdef WIN32
	//	printf(" CUserConfDetail::SelectDB GetResult llRowNum = %I64d\n", llRowNum);
	//#elif defined LINUX
	//	printf(" CUserConfDetail::SelectDB GetResult llRowNum = %lld\n", llRowNum);
	//#endif


	//if (nRow == 1)
	if (llRowNum == 1)
	{
		//MYSQL_ROW row = CDBConnector::GetInstance()->GetRecord();
		if (row != NULL && row[0] != NULL)
		{
#ifdef WIN32
			m_conf_id = _atoi64(row[0]);//
			printf("CUserConfDetail::SelectDB() OK conf_id:%s->%I64d\r\n", row[0], m_conf_id);
#elif defined LINUX
			m_conf_id = strtoll(row[0], NULL, 10);//
			printf("CUserConfDetail::SelectDB() OK conf_id:%s->%lld\r\n", row[0], m_conf_id);
#endif
		}
		if (row != NULL && row[1] != NULL)
		{
#ifdef WIN32
			m_conf_report_id = _atoi64(row[1]);//
			printf("CUserConfDetail::SelectDB() OK conf_report_id:%s->%I64d\r\n", row[1], m_conf_report_id);
#elif defined LINUX
			m_conf_report_id = strtoll(row[1], NULL, 10);//
			printf("CUserConfDetail::SelectDB() OK conf_report_id:%s->%lld\r\n", row[1], m_conf_report_id);
#endif
		}
		if (row != NULL && row[2] != NULL)
		{
#ifdef WIN32
			m_userrpt_detail_id = _atoi64(row[2]);//
			printf("CUserConfDetail::SelectDB() OK userrpt_detail_id:%s->%I64d\r\n", row[2], m_userrpt_detail_id);
#elif defined LINUX
			m_userrpt_detail_id = strtoll(row[2], NULL, 10);//
			printf("CUserConfDetail::SelectDB() OK userrpt_detail_id:%s->%lld\r\n", row[2], m_userrpt_detail_id);
#endif
		}
		if (row != NULL && row[3] != NULL)
			m_user_id = atoi(row[3]);
		if (row != NULL && row[4] != NULL)
		{
			m_useralias = row[4];
		}
		else
		{
			m_useralias.clear();
		}
		if (row != NULL && row[5] != NULL)
		{
			m_realjointime = row[5];
		}
		else
		{
			m_realjointime.clear();
		}
		if (row != NULL && row[6] != NULL)
		{
			m_realleavetime = row[6];
		}
		else
		{
			m_realleavetime.clear();
		}
	}

	printf("^^^^^^^^222222222222^^^^^^^^^^^^^ CUserConfDetail::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);
	return true;
#elif SHENTONG_DB_DEF
	unsigned long long ullRowNum = 0;
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;
	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CUserConfDetail::SelectDB error\r\n");

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
					unsigned long long ullConfid = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(1);
					//#ifdef WIN32
					//					ullConfid = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullConfid = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//					m_conf_id = ullConfid;
					ullConfid = (unsigned long)prs->getNumber(1);
					m_conf_id = ullConfid;

					unsigned long long ullConfreportid = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(2);
					//#ifdef WIN32
					//					ullConfreportid = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullConfreportid = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//					m_conf_report_id = ullConfreportid;
					ullConfreportid = (unsigned long)prs->getNumber(2);
					m_conf_report_id = ullConfreportid;

					unsigned long long ullUserrptdetailid = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(3);
					//#ifdef WIN32
					//					ullUserrptdetailid = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullUserrptdetailid = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//					m_userrpt_detail_id = ullUserrptdetailid;
					ullUserrptdetailid = (unsigned long)prs->getNumber(3);
					m_userrpt_detail_id = ullUserrptdetailid;

					int iUserid = 0;
					iUserid = prs->getInt(4);
					m_user_id = iUserid;

					m_useralias.clear();
					m_useralias = prs->getString(5);

					m_realjointime.clear();
					m_realjointime = prs->getString(6);

					m_realleavetime.clear();
					m_realleavetime = prs->getString(7);

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
