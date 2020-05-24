
#ifndef LIST_DEVICE_H
#define LIST_DEVICE_H

#include <iostream>
#include <string.h>
#include "DBconnector.h"

#include "list_deviceconfig.h"
//#include "sr_timer.h"

using std::string;
static int m_pub_device_id;

class CDevice
{
public:
	CDevice(unsigned int uiDevmgrNo);
	virtual ~CDevice();
	//static CDevice* GetInstance();

	//
	int GetDeviceID() { return m_device_id; }
	int GetMediaGroupID() { return m_mediagroup_id; }
	std::string GetDeviceConfigSerial() { return m_device_config_serial; }
	std::string GetDeviceToken() { return m_device_token; }
	int GetDeviceType() { return m_device_type; }
	unsigned long long GetDeviceIPandPort() { return m_device_ipandport; }
	char* GetDeviceIp() { return m_device_ip; }
	int GetDevicePort() { return m_device_port; }
	int GetMaxTerms() { return m_max_terms; }
	int GetStatus() { return m_status; }
	int GetMaxBandwidth() { return m_max_bandwidth; }
	std::string SystemLicence() { return m_system_licence; }
	std::string GetVersion() { return m_version; }

	void SetDeviceID(int device_id) { m_device_id = device_id; }
	void SetMediaGroupID(int mediagroup_id) { m_mediagroup_id = mediagroup_id; }
	void SetDeviceConfigSerial(std::string device_config_serial) { m_device_config_serial = device_config_serial; }
	void SetDeviceToken(std::string device_token) { m_device_token = device_token; }
	void SetDeviceType(int device_type) { m_device_type = device_type; }
	void SetDeviceIPandPort(unsigned long long device_ipandport) { m_device_ipandport = device_ipandport; }
	void SetDeviceIP(char * device_ip) { strcpy(m_device_ip , device_ip); }
	void SetDevicePort(int device_port) { m_device_port = device_port; }
	void SetMaxTerms(int max_terms) { m_max_terms = max_terms; }
	void SetStatus(int status) { m_status = status; }
	void SetMaxBandwidth(int max_bandwidth) { m_max_bandwidth = max_bandwidth; }
	void SetSystemLicence(std::string system_licence) { m_system_licence = system_licence; }
	void SetVersion(std::string version) { m_version = version; }

	//
	bool InsertDB(CDeviceConfig * pDeviceConfig);
	bool UpdateDB1();
	bool UpdateDB2();
	bool UpdateDB3();
	bool SelectDB1();
	bool SelectDB2();
	bool SelectDB3();
	bool SelectDB4();

	int GenerateDeviceID();

protected:
	//CDevice();
	bool DeleteDB();

private:
	int m_device_id;
	int m_mediagroup_id;
	std::string m_device_config_serial;
	std::string m_device_token;
	int m_device_type;
	unsigned long long  m_device_ipandport;
	//int m_device_ip;
    char m_device_ip[16];
	int m_device_port;
	int m_max_terms;
	int m_status;
	int m_max_bandwidth;
	std::string m_system_licence;
	std::string m_version;

	CCriticalSection m_csDevice;
	unsigned int SelectDB(unsigned int uiDevmgrNo);
	unsigned int m_uiDevmgrNo;
};

#endif //LIST_DEVICE_H