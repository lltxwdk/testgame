
#ifndef LIST_DEVICECONFIG_H
#define LIST_DEVICECONFIG_H

#include <iostream>
#include "DBconnector.h"

using std::string;

class CDeviceConfig
{
public:
	CDeviceConfig();
	virtual ~CDeviceConfig();
	//static CDeviceConfig* GetInstance();

	//
	std::string GetDeviceConfigSerial() { return m_device_config_serial; }
	int GetMediaGroupID() { return m_mediagroup_id; }
	int GetDeviceConfigType() { return m_device_config_type; }
	int GetConfigMaxTerms() { return m_config_max_terms; }

	void SetDeviceConfigSerial(std::string device_config_serial) { m_device_config_serial = device_config_serial; }
	void SetMediaGroupID(int mediagroup_id) { m_mediagroup_id = mediagroup_id; }
	void SetDeviceConfigType(int device_config_type) { m_device_config_type = device_config_type; }
	void SetConfigMaxTerms(int config_max_terms) { m_config_max_terms = config_max_terms; }
	
	//
	bool InsertDB();
	bool DeleteDB();
	bool UpdateDB();
	bool SelectDB();

protected:
	//CDeviceConfig();

private:
	std::string m_device_config_serial;
	int m_mediagroup_id;
	int m_device_config_type;
	int m_config_max_terms;

	CCriticalSection m_csDevConfig;
};

#endif //LIST_DEVICECONFIG_H