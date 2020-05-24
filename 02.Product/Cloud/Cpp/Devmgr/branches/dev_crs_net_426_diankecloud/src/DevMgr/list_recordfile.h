
#ifndef LIST_RECORDFILE_H
#define LIST_RECORDFILE_H

#include <iostream>
#include "DBconnector.h"
//#include "sr_timer.h"

using std::string;

static unsigned long long m_pub_recordfile_id;

class CRecordFile
{
public:
	CRecordFile(unsigned int uiDevmgrNo);
	virtual ~CRecordFile();
	//static CConfReport* GetInstance();

	//
	//unsigned long long GetConfID()
	//{
	//	return m_conf_id;
	//}
	unsigned long long GetRecordFileID()
	{
		return m_recordfile_id;
	}
	//unsigned long long GetConfReportID()
	//{
	//	return m_confreport_id;
	//}
	unsigned long long GetConfRecordID()
	{
		return m_confrecord_id;
	}
	unsigned long long GetRecordFileSize()
	{
		return m_record_filesize;
	}
	//std::string GetFilePath()
	//{
	//	return m_strfilepath;
	//}
	//std::string GetSdeFilePath()
	//{
	//	return m_strsdefilepath;
	//}
	//std::string GetFileStorSvrIP()
	//{
	//	return m_strfilestorsvrip;
	//}
	char* GetFilePath()
	{
		return m_cfilepath;
	}
	char* GetSdeFilePath()
	{
		return m_csdefilepath;
	}
	char* GetFileStorSvrIP()
	{
		return m_cfilestorsvrip;
	}
	char* GetFileRootPath()
	{
		return m_cfilerootpath;
	}

	//void SetConfID(unsigned long long conf_id)
	//{
	//	m_conf_id = conf_id;
	//}
	void SetRecordFileID(unsigned long long recordfile_id)
	{
		m_recordfile_id = recordfile_id;
	}
	//void SetConfReportID(unsigned long long confreport_id)
	//{
	//	m_confreport_id = confreport_id;
	//}
	void SetConfRecordID(unsigned long long confrecord_id)
	{
		m_confrecord_id = confrecord_id;
	}
	void SetRecordFileSize(unsigned long long recordfilesize)
	{
		m_record_filesize = recordfilesize;
	}
	//void SetFilePath(std::string filepath)
	//{
	//	m_strfilepath = filepath;
	//}
	//void SetSdeFilePath(std::string sdefilepath)
	//{
	//	m_strsdefilepath = sdefilepath;
	//}
	//void SetFileStorSvrIP(std::string filestorsvrip)
	//{
	//	m_strfilestorsvrip = filestorsvrip;
	//}
	void SetFilePath(char* filepath, unsigned int len)
	{
		if (filepath != NULL)
		{
			memset(m_cfilepath, 0, sizeof(m_cfilepath));
			memcpy(m_cfilepath, filepath, len);
		}
		//else
		//{
		//	memset(m_cfilepath, 0, sizeof(m_cfilepath));
		//}
	}
	void SetSdeFilePath(char* sdefilepath, unsigned int len)
	{
		if (sdefilepath != NULL)
		{
			memset(m_csdefilepath, 0, sizeof(m_csdefilepath));
			memcpy(m_csdefilepath, sdefilepath, len);
		}
		//else
		//{
		//	memset(m_csdefilepath, 0, sizeof(m_csdefilepath));
		//}
	}
	void SetFileStorSvrIP(char* filestorsvrip, unsigned int len)
	{
		if (filestorsvrip != NULL)
		{
			memset(m_cfilestorsvrip, 0, sizeof(m_cfilestorsvrip));
			memcpy(m_cfilestorsvrip, filestorsvrip, len);
		}
		//else
		//{
		//	memset(m_cfilestorsvrip, 0, sizeof(m_cfilestorsvrip));
		//}
	}

	void SetFileRootPath(char* filerootpath, unsigned int len)
	{
		if (filerootpath != NULL)
		{
			memset(m_cfilerootpath, 0, sizeof(m_cfilerootpath));
			memcpy(m_cfilerootpath, filerootpath, len);
		}
		//else
		//{
		//	memset(m_cfilestorsvrip, 0, sizeof(m_cfilestorsvrip));
		//}
	}
	////
	//bool InsertDB();
	//bool DeleteDB();
	//bool UpdateDB1();
	//bool UpdateDB2();
	//bool SelectDB();

	bool InsertRecordFileInfoToDB();
	//bool DeleteRecordFileInfoForDB();
	//bool UpdateRecordFileInfoForDB();

protected:
	//CConfReport();

private:
	//unsigned long long m_conf_id;
	//unsigned long long m_confreport_id;
	unsigned long long m_recordfile_id;
	unsigned long long m_confrecord_id;
	unsigned long long m_record_filesize;
	//std::string m_strfilepath;
	//std::string m_strsdefilepath;
	//std::string m_strfilestorsvrip;
	char m_cfilepath[300];
	char m_csdefilepath[300];
	char m_cfilestorsvrip[128];
	char m_cfilerootpath[128];
	
	CCriticalSection m_csRecfile;
};

#endif //LIST_RECORDFILE_H