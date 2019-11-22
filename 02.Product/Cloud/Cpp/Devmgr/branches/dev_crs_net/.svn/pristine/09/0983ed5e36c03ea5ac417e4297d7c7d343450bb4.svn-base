
#ifndef LIST_DEVICECONFDETAIL_H
#define LIST_DEVICECONFDETAIL_H

#include <iostream>
#include "DBconnector.h"
//#include "sr_timer.h"

using std::string;

static unsigned long long m_pub_device_detail_id;

class CDeviceConfDetail
{
public:
	CDeviceConfDetail(unsigned int uiDevmgrNo);
	virtual ~CDeviceConfDetail();
	//static CDeviceConfDetail* GetInstance();

	//
	unsigned long long GetConfID() { return m_conf_id; }
	unsigned long long GetConfReportID() { return m_conf_report_id; }
	unsigned long long GetDeviceDetailID() { return m_device_detail_id; }
	int GetDeviceID() { return m_device_id; }
	std::string GetRealuseStarttime() { return m_realuse_starttime; }
	std::string GetRealuseStoptime() { return m_realuse_stoptime; }

	void SetConfID(unsigned long long conf_id) { m_conf_id = conf_id; }
	//void SetConfReportID(int conf_report_id) { m_conf_report_id = conf_report_id; }
	void SetConfReportID(unsigned long long conf_report_id) { m_conf_report_id = conf_report_id; }
	void SetDeviceDetailID(unsigned long long device_detail_id) { m_device_detail_id = device_detail_id; }
	void SetDeviceID(int device_id) { m_device_id = device_id; }
	void SetRealuseStarttime(std::string realuse_starttime) { m_realuse_starttime = realuse_starttime; }
	void SetRealuseStoptime(std::string realuse_stoptime) { m_realuse_stoptime = realuse_stoptime; }
	
	//
	bool InsertDB();
	bool DeleteDB();
	bool UpdateDB1();
	bool UpdateDB2();
	bool SelectDB();

	bool InsertDeviceConfDetailToDB();
protected:
	//CDeviceConfDetail();

private:
	unsigned long long m_conf_id;
	unsigned long long m_conf_report_id;
	unsigned long long m_device_detail_id;
	int m_device_id;
	std::string m_realuse_starttime;
	std::string m_realuse_stoptime;

	CCriticalSection m_csDevConfDetail;
};

#endif //LIST_DEVICECONFDETAIL_H