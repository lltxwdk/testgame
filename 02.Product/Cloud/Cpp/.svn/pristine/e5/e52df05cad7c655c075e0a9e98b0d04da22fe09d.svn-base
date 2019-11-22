#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_recordfile.h"
#include <cstdlib>
#include <stdio.h>

CRecordFile::CRecordFile(unsigned int uiDevmgrNo)
{
	//m_conf_id = 0;
	//m_confreport_id = 0;
	m_confrecord_id = 0;
	m_recordfile_id = 0;
	//m_strfilepath = "";
	//m_strsdefilepath = "";
	//m_strfilestorsvrip = "";
	memset(m_cfilepath, 0, sizeof(m_cfilepath));
	memset(m_csdefilepath, 0, sizeof(m_csdefilepath));
	memset(m_cfilestorsvrip, 0, sizeof(m_cfilestorsvrip));
	memset(m_cfilerootpath, 0, sizeof(m_cfilerootpath));
}

CRecordFile::~CRecordFile()
{

}

bool CRecordFile::InsertRecordFileInfoToDB()
{
	CCriticalAutoLock MyLock(m_csRecfile);

	char cInsert[1024];
#ifdef WIN32
	sprintf(cInsert, "insert into record_file(record_file_id, conf_record_id, filepath, sdepath, serverip, relativepath, file_size) values('%I64d','%I64d','%s', '%s', '%s', '%s','%I64d')", m_recordfile_id, m_confrecord_id, m_cfilepath, m_csdefilepath, m_cfilestorsvrip, m_cfilerootpath, m_record_filesize);
#elif defined LINUX
	sprintf(cInsert, "insert into record_file(record_file_id, conf_record_id, filepath, sdepath, serverip, relativepath, file_size) values('%lld','%lld','%s', '%s', '%s', '%s','%lld')", m_recordfile_id, m_confrecord_id, m_cfilepath, m_csdefilepath, m_cfilestorsvrip, m_cfilerootpath, m_record_filesize);
#endif

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cInsert);

	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_ERROR, "insert record_file record_file_id:%I64d,conf_record_id:%I64d error: %s\n", m_recordfile_id, m_confrecord_id, CDBConnector::GetInstance()->OutErrors());
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_ERROR, "insert record_file record_file_id:%lld,conf_record_id:%lld error: %s\n", m_recordfile_id, m_confrecord_id, CDBConnector::GetInstance()->OutErrors());
#endif
		return false;
	}

	return true;
}

//bool CRecordFile::UpdateRecordFileInfoForDB()
//{
//	CCriticalAutoLock MyLock(m_csRecfile);
//
//	char cUpdate[1024];
//#ifdef WIN32
//	sprintf(cUpdate, "update conf_report set real_endtime = '%s' where conf_id='%I64d' and conf_report_id = '%I64d'", m_stoptime.c_str(), m_conf_id, m_confreport_id);
//#elif defined LINUX
//	sprintf(cUpdate, "update conf_report set real_endtime = '%s' where conf_id='%lld' and conf_report_id = '%lld'", m_stoptime.c_str(), m_conf_id, m_confreport_id);
//#endif	
//
//	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cUpdate);
//
//	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
//	{
//		//printf("update conf_report error\r\n");
//		//printf("%s\n", cUpdate);
//		//printf(CDBConnector::GetInstance()->OutErrors());
//
//		sr_printf(SR_LOGLEVEL_ERROR, "update conf_report error: %s\n", CDBConnector::GetInstance()->OutErrors());
//		return false;
//	}
//
//	return true;
//}