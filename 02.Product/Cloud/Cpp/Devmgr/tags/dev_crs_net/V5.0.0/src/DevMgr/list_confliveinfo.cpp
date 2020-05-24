#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_confliveinfo.h"
#include <cstdlib>
#include <stdio.h>

CConfLiveInfo::CConfLiveInfo(unsigned int uiDevmgrNo)
{
	m_liveinfo_id = 0;
	m_conf_id = 0;
	m_confreport_id = 0;
	m_live_subject = "";
	m_starttime = "";
	m_endtime = "";
	m_live_chairman = "";
	m_live_abstract = "";
	m_is_public = 0;
	m_live_pwd = "";
	m_is_userec = 1;
	m_live_addr = "";
	m_watch_times = 0;
	m_likes_times = 0;
	m_watch_addr = "";
	m_android_times = 0;
	m_ios_times = 0;
	m_pc_times = 0;
	m_mobile_times = 0;
	m_convert_status = 0;
	m_task_id = "";
}

CConfLiveInfo::~CConfLiveInfo()
{

}

bool CConfLiveInfo::InsertConfLiveInfoToDB()
{
	CCriticalAutoLock MyLock(m_csConfliveinfo);

	char cInsert[2560];
#ifdef WIN32
	sprintf(cInsert, "insert into live_info(li_id, conf_id, conf_report_id, li_subject, starttime, endtime, chairman, li_abstract, is_public, live_pwd, live_addr, watch_times, likes_times, watch_addr, is_userec, android_times, ios_times, pc_times, mobile_times, convert_status, task_id) values('%I64d','%I64d','%I64d','%s','%s','%s','%s','%s','%d','%s','%s','%d','%d','%s','%d','%d','%d','%d','%d','%d','%s')", 
		m_liveinfo_id,m_conf_id,m_confreport_id,m_live_subject.c_str(),m_starttime.c_str(),m_endtime.c_str(),m_live_chairman.c_str(),m_live_abstract.c_str(),m_is_public,m_live_pwd.c_str(),m_live_addr.c_str(),m_watch_times,m_likes_times,m_watch_addr.c_str(),m_is_userec,m_android_times,m_ios_times,m_pc_times,m_mobile_times,m_convert_status,m_task_id.c_str());
#elif defined LINUX
	sprintf(cInsert, "insert into live_info(li_id, conf_id, conf_report_id, li_subject, starttime, endtime, chairman, li_abstract, is_public, live_pwd, live_addr, watch_times, likes_times, watch_addr, is_userec, android_times, ios_times, pc_times, mobile_times, convert_status, task_id) values('%lld','%lld','%lld','%s','%s','%s','%s','%s','%d','%s','%s','%d','%d','%s','%d','%d','%d','%d','%d','%d','%s')",
		m_liveinfo_id, m_conf_id, m_confreport_id, m_live_subject.c_str(), m_starttime.c_str(), m_endtime.c_str(), m_live_chairman.c_str(), m_live_abstract.c_str(), m_is_public, m_live_pwd.c_str(), m_live_addr.c_str(), m_watch_times, m_likes_times, m_watch_addr.c_str(), m_is_userec, m_android_times, m_ios_times, m_pc_times, m_mobile_times, m_convert_status, m_task_id.c_str());
#endif

	sr_printf(SR_LOGLEVEL_DEBUG, " %s\n", cInsert);

	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
#ifdef WIN32
		sr_printf(SR_LOGLEVEL_DEBUG, "insert live_info liveinfo_id:%I64d,conf_id:%I64d,conf_report_id:%I64d error\r\n", m_liveinfo_id, m_conf_id, m_confreport_id, CDBConnector::GetInstance()->OutErrors());
#elif defined LINUX
		sr_printf(SR_LOGLEVEL_DEBUG, "insert live_info liveinfo_id:%lld,conf_id:%lld,conf_report_id:%lld error\r\n", m_liveinfo_id, m_conf_id, m_confreport_id, CDBConnector::GetInstance()->OutErrors());
#endif
		return false;
	}

	return true;
}