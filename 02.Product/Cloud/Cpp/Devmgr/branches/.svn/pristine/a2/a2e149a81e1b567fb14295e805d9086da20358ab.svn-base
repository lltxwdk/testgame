
#ifndef LIST_LIVEINFO_H
#define LIST_LIVEINFO_H

#include <iostream>
#include "DBconnector.h"
//#include "sr_timer.h"

using std::string;

static unsigned long long m_pub_liveinfo_id;

class CConfLiveInfo
{
public:
	CConfLiveInfo(unsigned int uiDevmgrNo);
	virtual ~CConfLiveInfo();


	unsigned long long GetConfID()
	{
		return m_conf_id;
	}
	unsigned long long GetConfReportID()
	{
		return m_confreport_id;
	}
	unsigned long long GetLiveInfoID()
	{
		return m_liveinfo_id;
	}
	std::string GetLiveSubject()
	{
		return m_live_subject;
	}
	std::string GetStartTime()
	{
		return m_starttime;
	}
	std::string GetEndTime()
	{
		return m_endtime;
	}
	std::string GetLiveChairman()
	{
		return m_live_chairman;
	}
	std::string GetLiveAbstract()
	{
		return m_live_abstract;
	}
	int GetIsPublic()
	{
		return m_is_public;
	}
	std::string GetLivePWD()
	{
		return m_live_pwd;
	}
	int GetIsUseRec()
	{
		return m_is_userec;
	}
	std::string GetLiveAddr()
	{
		return m_live_addr;
	}
	int GetWatchtimes()
	{
		return m_watch_times;
	}
	int GetLiketimes()
	{
		return m_likes_times;
	}
	std::string GetWatchaddr()
	{
		return m_watch_addr;
	}
	int GetAndroidtimes()
	{
		return m_android_times;
	}
	int GetIOStimes()
	{
		return m_ios_times;
	}
	int GetPCtimes()
	{
		return m_pc_times;
	}
	int GetMobiletimes()
	{
		return m_mobile_times;
	}
	int GetConvertstatus()
	{
		return m_convert_status;
	}
	std::string GetTaskid()
	{
		return m_task_id;
	}


	void SetConfID(unsigned long long conf_id)
	{
		m_conf_id = conf_id;
	}
	void SetConfReportID(unsigned long long confreport_id)
	{
		m_confreport_id = confreport_id;
	}
	void SetLiveInfoID(unsigned long long liveinfo_id)
	{
		m_liveinfo_id = liveinfo_id;
	}
	void SetLiveSubject(std::string live_subject)
	{
		m_live_subject = live_subject;
	}
	void SetStartTime(std::string starttime)
	{
		m_starttime = starttime;
	}
	void SetEndTime(std::string endtime)
	{
		m_endtime = endtime;
	}
	void SetLiveChairman(std::string live_chairman)
	{
		m_live_chairman = live_chairman;
	}
	void SetLiveAbstract(std::string live_abstract)
	{
		m_live_abstract = live_abstract;
	}	
	void SetIsPublic(int is_public)
	{
		m_is_public = is_public;
	}
	void SetLivePWD(std::string live_pwd)
	{
		m_live_pwd = live_pwd;
	}
	void SetIsUseRec(int is_userec)
	{
		m_is_userec = is_userec;
	}
	void SetLiveAddr(std::string live_addr)
	{
		m_live_addr = live_addr;
	}
	void SetWatchtimes(int watch_times)
	{
		m_watch_times = watch_times;
	}
	void SetLiketimes(int likes_times)
	{
		m_likes_times = likes_times;
	}
	void SetWatchaddr(std::string watch_addr)
	{
		m_watch_addr = watch_addr;
	}
	void SetAndroidtimes(int android_times)
	{
		m_android_times = android_times;
	}
	void SetIOStimes(int ios_times)
	{
		m_ios_times = ios_times;
	}
	void SetPCtimes(int pc_times)
	{
		m_pc_times = pc_times;
	}
	void SetMobiletimes(int mobile_times)
	{
		m_mobile_times = mobile_times;
	}
	void SetConvertstatus(int convert_status)
	{
		m_convert_status = convert_status;
	}
	void SetTaskid(std::string task_id)
	{
		m_task_id = task_id;
	}


	////
	//bool InsertDB();
	//bool DeleteDB();
	//bool UpdateDB1();
	//bool UpdateDB2();
	//bool SelectDB();

	bool InsertConfLiveInfoToDB();
	////bool DeleteConfCRSRecordForDB();
	//bool UpdateConfCRSRealEndtimeForDB();
	//bool SelectConfRecordidFromDB();

protected:

private:
	unsigned long long m_liveinfo_id;
	unsigned long long m_conf_id;
	unsigned long long m_confreport_id;
	std::string m_live_subject; // 直播主题
	std::string m_starttime;
	std::string m_endtime;
	std::string m_live_chairman;
	std::string m_live_abstract; // 直播摘要/简介
	int m_is_public;
	std::string m_live_pwd;
	int m_is_userec;
	std::string m_live_addr;
	int m_watch_times;
	int m_likes_times;
	std::string m_watch_addr;// 
	int m_android_times;
	int m_ios_times;
	int m_pc_times;
	int m_mobile_times;
	int m_convert_status;
	std::string m_task_id;

	CCriticalSection m_csConfliveinfo;
};

#endif //LIST_LIVEINFO_H