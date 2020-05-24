
#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_device.h"
#include "sr_type.h"
//#include "list_deviceconfig.h"
#include <cstdlib>
#include <stdio.h>

CDevice::CDevice(unsigned int uiDevmgrNo)
{
	//m_device_id = 0;
	m_mediagroup_id = 0;
	m_device_config_serial = "";
	m_device_token = "";
	m_device_type = 0;
	m_device_ipandport = 0;
	//int m_device_ip;
	m_device_ip[16] = {0};
	m_device_port = 0;
	m_max_terms = 0;
	m_status = 0;
	m_max_bandwidth = 0;
	m_uiDevmgrNo = uiDevmgrNo;

	//unsigned int iDevice_id = SelectDB(m_uiDevmgrNo); // uiDevmgrNo最多支持8bit，即0-255个编号

	//////组合iDevmgrNum 和 当前数据库行数
	////m_device_id = ((uiDevmgrNo << 24) + iDevice_id); // 如果mysql中的设备数iDevice_id超过256可能会有问题
	////m_pub_device_id = m_device_id;
	//m_device_id = iDevice_id;
	//m_pub_device_id = m_device_id;

	//printf(">>>>>>>>>In CDevice Class  uiDevmgrNum>>> %d, iDevice_id = %d, m_device_id>>> %d\n", uiDevmgrNo, iDevice_id, m_device_id);

	GenerateDeviceID();
}

CDevice::~CDevice()
{

}

//CDevice* CDevice::GetInstance()
//{
//	static CDevice g_cDevice;
//	return &g_cDevice;
//}

bool CDevice::InsertDB(CDeviceConfig * pDeviceConfig)
{
	CCriticalAutoLock MyLock(m_csDevice);
	m_mediagroup_id = pDeviceConfig->GetMediaGroupID();
	m_max_terms = pDeviceConfig->GetConfigMaxTerms();

	//if (!CDBConnector::GetInstance()->SelectRecord("select * from device"))
	//{
	//	m_device_id = 1;
	//}
	//else
	//{
	//	long long nRow = CDBConnector::GetInstance()->GetRowNum();
	//	if (nRow == 0)
	//		m_device_id = 1;
	//	else
	//	{
	//		m_device_id = (int)nRow;
	//		m_device_id++;
	//	}
	//}

	unsigned long long llRowNum = 0;
	//MYSQL_ROW row;
	//char cSelect[1024];
	//sprintf(cSelect, "select * from device");

	//MYSQL_RES *pQuery = NULL;

	//if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, pQuery))
	//{
	//	//printf("CDevice::InsertDB GetResult error\r\n");
	//	printf(CDBConnector::GetInstance()->OutErrors());
	//	m_device_id = 1;
	//}
	//else
	//{
	//	if (llRowNum == 0)
	//	{
	//		m_device_id = 1;
	//	}
	//	else
	//	{
	//		m_device_id = (int)llRowNum;
	//		m_device_id++;
	//	}
	//}

	//CDBConnector::GetInstance()->FreeResult(pQuery);

	sr_printf(SR_LOGLEVEL_INFO, "====CDevice::InsertDB ---->>this=%p, m_mediagroup_id=%d, m_max_terms=%d, m_device_id=%d, m_pub_device_id=%d\n", this, m_mediagroup_id, m_max_terms, m_device_id, m_pub_device_id);

	m_device_id = m_pub_device_id++;

//#ifdef WIN32
//	printf(" CDevice::InsertDB GetResult llRowNum = %I64d, m_device_id = %d\n", llRowNum, m_device_id);
//#elif defined LINUX
//	printf(" CDevice::InsertDB GetResult llRowNum = %lld, m_device_id = %d\n", llRowNum, m_device_id);
//#endif
	

	char buf[32] = { 0 };
	sprintf(buf, "%d", m_device_id);
	m_device_token = buf;

	char cInsert[1024];
#ifdef WIN32
	sprintf(cInsert, "insert into device(device_id, mediagroup_id, device_config_serial, device_token,\
					 			device_type, device_ip, device_port, device_ipandport, max_terms, status, max_bandwidth, license, device_version) values('%d','%d','%s','%s','%d','%s','%d',%I64d,'%d','%d','%d','%s','%s')", \
								m_device_id, m_mediagroup_id, m_device_config_serial.c_str(), m_device_token.c_str(), m_device_type,
								m_device_ip, m_device_port, m_device_ipandport, m_max_terms, m_status, m_max_bandwidth, m_system_licence.c_str(), m_version.c_str());
#elif defined LINUX
	sprintf(cInsert, "insert into device(device_id, mediagroup_id, device_config_serial, device_token,\
					 			device_type, device_ip, device_port, device_ipandport, max_terms, status, max_bandwidth, license, device_version) values('%d','%d','%s','%s','%d','%s','%d',%lld,'%d','%d','%d','%s','%s')", \
								m_device_id, m_mediagroup_id, m_device_config_serial.c_str(), m_device_token.c_str(), m_device_type,
								m_device_ip, m_device_port, m_device_ipandport, m_max_terms, m_status, m_max_bandwidth, m_system_licence.c_str(), m_version.c_str());

#endif	

	sr_printf(SR_LOGLEVEL_INFO, "====insert into device ---->> %s\n", cInsert);

	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
		printf("insert device error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDevice::DeleteDB()
{
	CCriticalAutoLock MyLock(m_csDevice);
	char cDelete[1024];
	sprintf(cDelete, "delete from device where device_token = '%s'", m_device_token.c_str());
	if (!CDBConnector::GetInstance()->ExectCommand(cDelete))
	{
		printf("delete device error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDevice::UpdateDB1()
{
	CCriticalAutoLock MyLock(m_csDevice);
	char cUpdate[1024];
	sprintf(cUpdate, "update device set max_bandwidth = '%d', status = '%d', license = '%s', device_version = '%s' where device_id = '%d'", \
		m_max_bandwidth, m_status, m_system_licence.c_str(), m_version.c_str(), m_device_id);

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update1 device error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDevice::UpdateDB2()
{
	CCriticalAutoLock MyLock(m_csDevice);
	char cUpdate[1024];
	sprintf(cUpdate, "update device set status = '%d' where device_token='%s'",\
				m_status, m_device_token.c_str());

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update2 device error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDevice::UpdateDB3()
{
	CCriticalAutoLock MyLock(m_csDevice);
	char cUpdate[1024];
	sprintf(cUpdate, "update device set status = '%d' where device_id='%d'", \
		m_status, m_device_id);

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update3 device error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDevice::SelectDB1()
{
	CCriticalAutoLock MyLock(m_csDevice);

	//获取后，更新本地变量
	char cSelect[1024];
	sprintf(cSelect, "select device_id,mediagroup_id,device_config_serial,device_token,device_type,device_ip,device_port,device_ipandport,max_terms,status,max_bandwidth,license,device_version from device where device_config_serial='%s' and device_ip='%s' and device_port=%d and is_del=0", m_device_config_serial.c_str(), m_device_ip, m_device_port);

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		//printf("CDevice::SelectDB1 GetResult error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^11111111^^^^^^^^^^^^^ CDevice::SelectDB1()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

	//#ifdef WIN32
	//	printf(" CDevice::SelectDB1 GetResult llRowNum = %I64d\n", llRowNum);
	//#elif defined LINUX
	//	printf(" CDevice::SelectDB1 GetResult llRowNum = %lld\n", llRowNum);
	//#endif

	//if (nRow == 1)
	if (llRowNum == 1)
	{
		//MYSQL_ROW row = CDBConnector::GetInstance()->GetRecord();
		if (row != NULL && row[0] != NULL)
			m_device_id = atoi(row[0]);
		if (row != NULL && row[1] != NULL)
			m_mediagroup_id = atoi(row[1]);
		if (row != NULL && row[2] != NULL)
		{
			m_device_config_serial = row[2];
		}
		else
		{
			m_device_config_serial.clear();
		}
		if (row != NULL && row[3] != NULL)
		{
			m_device_token = row[3];
		}
		else
		{
			m_device_token.clear();
		}
		if (row != NULL && row[4] != NULL)
			m_device_type = atoi(row[4]);
		if (row != NULL && row[5] != NULL)
		{
#ifdef WIN32
			strcpy(m_device_ip, row[5]);
#elif defined LINUX
			strcpy(m_device_ip, (char*)row[5]);
#endif
		}
		if (row != NULL && row[6] != NULL)
			m_device_port = atoi(row[6]);
		if (row != NULL && row[7] != NULL)
		{
#ifdef WIN32
			m_device_ipandport = _atoi64(row[7]);//
#elif defined LINUX
			m_device_ipandport = strtoll(row[7], NULL, 10);//
#endif	
		}
		if (row != NULL && row[8] != NULL)
			m_max_terms = atoi(row[8]);
		if (row != NULL && row[9] != NULL)
			m_status = atoi(row[9]);
		if (row != NULL && row[10] != NULL)
			m_max_bandwidth = atoi(row[10]);

		if (row != NULL && row[11] != NULL)
		{
			m_system_licence = row[11];
		}
		else
		{
			m_system_licence.clear();
		}

		if (row != NULL && row[12] != NULL)
		{
			m_version = row[12];
		}
		else
		{
			m_version.clear();
		}

	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CDevice::SelectDB1()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF
	unsigned long long ullRowNum = 0;
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;
	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CDevice::SelectDB1 error\r\n");

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

					int iDeviceid = 0;
					iDeviceid = prs->getInt(1);
					m_device_id = iDeviceid;

					printf("CDevice::SelectDB1 m_device_id=%d\n", iDeviceid);

					int iMediagroupid = 0;
					iMediagroupid = prs->getInt(2);
					m_mediagroup_id = iMediagroupid;

					printf("CDevice::SelectDB1 m_mediagroup_id=%d\n", iMediagroupid);

					m_device_config_serial.clear();
					m_device_config_serial = prs->getString(3);

					printf("CDevice::SelectDB1 m_device_config_serial=%s\n", m_device_config_serial.c_str());

					m_device_token.clear();
					m_device_token = prs->getString(4);

					printf("CDevice::SelectDB1 m_device_token=%s\n", m_device_token.c_str());

					int iDevicetype = 0;
					iDevicetype = prs->getInt(5);
					m_device_type = iDevicetype;

					printf("CDevice::SelectDB1 m_device_type=%d\n", iDevicetype);

					strtemp.clear();
					strtemp = prs->getString(6);

					printf("CDevice::SelectDB1 m_device_ip=%s\n", strtemp.c_str());
#ifdef WIN32
					strcpy(m_device_ip, strtemp.c_str());
#elif defined LINUX
					strcpy(m_device_ip, (char*)(strtemp.c_str()));
#endif

					int iDeviceport = 0;
					iDeviceport = prs->getInt(7);
					m_device_port = iDeviceport;

					printf("CDevice::SelectDB1 m_device_port=%d\n", iDeviceport);

					unsigned long long ullDeviceipandport = 0;
					//					strtemp.clear();
					//					strtemp = prs->getString(8);
					//#ifdef WIN32
					//					ullDeviceipandport = _atoi64(strtemp.c_str());
					//#elif defined LINUX
					//					ullDeviceipandport = strtoll(strtemp.c_str(), NULL, 10);
					//#endif	
					//
					//					m_device_ipandport = ullDeviceipandport;

					Number num = prs->getNumber(8);
					ullDeviceipandport = (unsigned long)num;
					m_device_ipandport = ullDeviceipandport;

					int iMaxterms = 0;
					iMaxterms = prs->getInt(9);
					m_max_terms = iMaxterms;

					int iStatus = 0;
					iStatus = prs->getInt(10);
					m_status = iStatus;

					int iMaxbandwidth = 0;
					iMaxbandwidth = prs->getInt(11);
					m_max_bandwidth = iMaxbandwidth;

					m_system_licence.clear();
					m_system_licence = prs->getString(12);

					m_version.clear();
					m_version = prs->getString(13);

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

bool CDevice::SelectDB2()
{
	CCriticalAutoLock MyLock(m_csDevice);

	//获取后，更新本地变量
	char cSelect[1024];
	sprintf(cSelect, "select device_id,mediagroup_id,device_config_serial,device_token,device_type,device_ip,device_port,device_ipandport,max_terms,status,max_bandwidth from device where device_type=%d", m_device_type);

	//if (!CDBConnector::GetInstance()->SelectRecord(cSelect))
	//{
	//	printf("select device error\r\n");
	//	printf(CDBConnector::GetInstance()->OutErrors());
	//	return false;
	//}

	//long long nRow = CDBConnector::GetInstance()->GetRowNum();
	//if (nRow <= 0)
	//	return false;

#ifdef MYSQL_DB_DEF
	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		//printf("CDevice::SelectDB2 GetResult error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());


		printf("^^^^^^^^11111111^^^^^^^^^^^^^ CDevice::SelectDB2()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

	//if (nRow == 1)
	if (llRowNum == 1)
	{
		//MYSQL_ROW row = CDBConnector::GetInstance()->GetRecord();
		if (row != NULL && row[0] != NULL)
			m_device_id = atoi(row[0]);
		if (row != NULL && row[1] != NULL)
			m_mediagroup_id = atoi(row[1]);
		if (row != NULL && row[2] != NULL)
		{
			m_device_config_serial = row[2];
		}
		else
		{
			m_device_config_serial.clear();
		}
		if (row != NULL && row[3] != NULL)
		{
			m_device_token = row[3];
		}
		else
		{
			m_device_token.clear();
		}
		if (row != NULL && row[4] != NULL)
			m_device_type = atoi(row[4]);
		if (row != NULL && row[5] != NULL)
		{
#ifdef WIN32
			strcpy(m_device_ip, row[5]);
#elif defined LINUX
			strcpy(m_device_ip, (char*)row[5]);
#endif
		}
		if (row != NULL && row[6] != NULL)
			m_device_port = atoi(row[6]);
		if (row != NULL && row[7] != NULL)
		{
#ifdef WIN32
			m_device_ipandport = _atoi64(row[7]);//
#elif defined LINUX
			m_device_ipandport = strtoll(row[7], NULL, 10);//
#endif	
		}
		if (row != NULL && row[8] != NULL)
			m_max_terms = atoi(row[8]);
		if (row != NULL && row[9] != NULL)
			m_status = atoi(row[9]);
		if (row != NULL && row[10] != NULL)
			m_max_bandwidth = atoi(row[10]);
		}

	printf("^^^^^^^^222222222222^^^^^^^^^^^^^ CDevice::SelectDB2()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);
#elif SHENTONG_DB_DEF

#endif

	return true;
}

bool CDevice::SelectDB3()
{
	CCriticalAutoLock MyLock(m_csDevice);

	//获取后，更新本地变量
	char cSelect[1024];
	sprintf(cSelect, "select device_id,mediagroup_id,device_config_serial,device_token,device_type,device_ip,device_port,device_ipandport,max_terms,status,max_bandwidth from device where device_token=%s", m_device_token.c_str());

	//if (!CDBConnector::GetInstance()->SelectRecord(cSelect))
	//{
	//	printf("select device error\r\n");
	//	printf(CDBConnector::GetInstance()->OutErrors());
	//	return false;
	//}

	//long long nRow = CDBConnector::GetInstance()->GetRowNum();
	//if (nRow <= 0)
	//	return false;

#ifdef MYSQL_DB_DEF
	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		//printf("CDevice::SelectDB3 GetResult error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^111111111111^^^^^^^^^^^^^ CDevice::SelectDB3()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

	//#ifdef WIN32
	//	printf(" CDevice::SelectDB3 GetResult llRowNum = %I64d\n", llRowNum);
	//#elif defined LINUX
	//	printf(" CDevice::SelectDB3 GetResult llRowNum = %lld\n", llRowNum);
	//#endif

	//if (nRow == 1)
	if (llRowNum == 1)
	{
		//MYSQL_ROW row = CDBConnector::GetInstance()->GetRecord();
		if (row != NULL && row[0] != NULL)
			m_device_id = atoi(row[0]);
		if (row != NULL && row[1] != NULL)
			m_mediagroup_id = atoi(row[1]);
		if (row != NULL && row[2] != NULL)
		{
			m_device_config_serial = row[2];
		}
		else
		{
			m_device_config_serial.clear();
		}
		if (row != NULL && row[3] != NULL)
		{
			m_device_token = row[3];
		}
		else
		{
			m_device_token.clear();
		}
		if (row != NULL && row[4] != NULL)
			m_device_type = atoi(row[4]);
		if (row != NULL && row[5] != NULL)
		{
#ifdef WIN32
			strcpy(m_device_ip, row[5]);
#elif defined LINUX
			strcpy(m_device_ip, (char*)row[5]);
#endif
		}
		if (row != NULL && row[6] != NULL)
			m_device_port = atoi(row[6]);
		if (row != NULL && row[7] != NULL)
		{
#ifdef WIN32
			m_device_ipandport = _atoi64(row[7]);//
#elif defined LINUX
			m_device_ipandport = strtoll(row[7], NULL, 10);//
#endif	
		}
		if (row != NULL && row[8] != NULL)
			m_max_terms = atoi(row[8]);
		if (row != NULL && row[9] != NULL)
			m_status = atoi(row[9]);
		if (row != NULL && row[10] != NULL)
			m_max_bandwidth = atoi(row[10]);
	}


	printf("^^^^^^^^2222222222^^^^^^^^^^^^^ CDevice::SelectDB3()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);

#elif SHENTONG_DB_DEF

#endif

	return true;
}

bool CDevice::SelectDB4()
{
	CCriticalAutoLock MyLock(m_csDevice);
	//获取后，更新本地变量
	char cSelect[1024];
	sprintf(cSelect, "select device_id,mediagroup_id,device_config_serial,device_token,device_type,device_ip,device_port,device_ipandport,max_terms,status,max_bandwidth from device where device_id=%d", m_device_id);

	//if (!CDBConnector::GetInstance()->SelectRecord(cSelect))
	//{
	//	printf("select device error\r\n");
	//	printf(CDBConnector::GetInstance()->OutErrors());
	//	return false;
	//}

	//long long nRow = CDBConnector::GetInstance()->GetRowNum();
	//if (nRow <= 0)
	//	return false;

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		printf("CDevice::SelectDB4 GetResult error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^11111111^^^^^^^^^^^^^ CDevice::SelectDB4()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

#ifdef WIN32
	printf(" CDevice::SelectDB4 GetResult llRowNum = %I64d\n", llRowNum);
#elif defined LINUX
	printf(" CDevice::SelectDB4 GetResult llRowNum = %lld\n", llRowNum);
#endif

	//if (nRow == 1)
	if (llRowNum == 1)
	{
		//MYSQL_ROW row = CDBConnector::GetInstance()->GetRecord();
		if (row != NULL && row[0] != NULL)
			m_device_id = atoi(row[0]);
		if (row != NULL && row[1] != NULL)
			m_mediagroup_id = atoi(row[1]);
		if (row != NULL && row[2] != NULL)
		{
			m_device_config_serial = row[2];
		}
		else
		{
			m_device_config_serial.clear();
		}
		if (row != NULL && row[3] != NULL)
		{
			m_device_token = row[3];
		}
		else
		{
			m_device_token.clear();
		}
		if (row != NULL && row[4] != NULL)
			m_device_type = atoi(row[4]);
		if (row != NULL && row[5] != NULL)
		{
#ifdef WIN32
			strcpy(m_device_ip, row[5]);
#elif defined LINUX
			strcpy(m_device_ip, (char*)row[5]);
#endif
		}
		if (row != NULL && row[6] != NULL)
			m_device_port = atoi(row[6]);
		if (row != NULL && row[7] != NULL)
		{
#ifdef WIN32
			m_device_ipandport = _atoi64(row[7]);//
#elif defined LINUX
			m_device_ipandport = strtoll(row[7], NULL, 10);//
#endif	
		}
		if (row != NULL && row[8] != NULL)
			m_max_terms = atoi(row[8]);
		if (row != NULL && row[9] != NULL)
			m_status = atoi(row[9]);
		if (row != NULL && row[10] != NULL)
			m_max_bandwidth = atoi(row[10]);
	}

	printf("^^^^^^^^22222222222222^^^^^^^^^^^^^ CDevice::SelectDB4()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);
#elif SHENTONG_DB_DEF

#endif

	return true;
}


unsigned int CDevice::SelectDB(unsigned int uiDevmgrNo)
{
	CCriticalAutoLock MyLock(m_csDevice);
	unsigned int uiDeviceID = 0;

	if (uiDevmgrNo > 255
		|| uiDevmgrNo == 0)
	{
		return uiDeviceID;
	}

	//unsigned int uiDeviceIDRangMax = ((uiDevmgrNo+1) << 24) - 1;
	//unsigned int uiDeviceIDRangMin = (uiDevmgrNo << 24);

	unsigned int uiDeviceIDRangMax = (uiDevmgrNo << 10);
	unsigned int uiDeviceIDRangMin = ((uiDevmgrNo - 1) << 10) + 1;

	unsigned long long llRowNum = 0;
	char cSelect[1024];

#ifdef MYSQL_DB_DEF

	sprintf(cSelect, "select max(device_id) from device where device_id > %d and device_id <= %d", uiDeviceIDRangMin, uiDeviceIDRangMax);

	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CDevice::SelectDB GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CDevice::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return uiDeviceID;
	}
	else
	{
		// 出厂时，该表是空表,得到的结果是<llRowNum,NULL>
		if (llRowNum == 1)
		{
			// 测试
			for (std::map<unsigned long long, MYSQL_ROW>::iterator row_itor = rows_set.begin();
				row_itor != rows_set.end(); row_itor++)
			{
				MYSQL_ROW ret_row = row_itor->second;
				if (ret_row[0] == NULL)
				{
					// 出厂时，该表是空表
					uiDeviceID = uiDeviceIDRangMin;
					break;
				}
				uiDeviceID = atoi(ret_row[0]);
#ifdef WIN32
				printf("RowNum:%I64d={%u}\n", (row_itor->first) + 1, uiDeviceID);
#elif defined LINUX
				printf("RowNum:%lld={%u}\n", (row_itor->first) + 1, uiDeviceID);
#endif
				uiDeviceID++;
				break;
			}
		}
		else
		{
			// 出厂时，该表是空表
			uiDeviceID = uiDeviceIDRangMin;
		}
	}


	printf("^^^^^^^^^22222222222222^^^^^^^^^^^^^^^ CDevice::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);

#elif SHENTONG_DB_DEF

	sprintf(cSelect, "select max(device_id) from device where device_id >= %d and device_id <= %d", uiDeviceIDRangMin, uiDeviceIDRangMax);
	//sprintf(cSelect, "select max(mediagroup_id) from device_config where mediagroup_id > %d and mediagroup_id <= %d", 1, uiDeviceIDRangMax);
	//sprintf(cSelect, "select * from device_config where mediagroup_id > %d and mediagroup_id <= %d", 1, uiDeviceIDRangMax);
	//sprintf(cSelect, "select mediagroup_id from device_config where mediagroup_id > %d and mediagroup_id <= %d", 1, uiDeviceIDRangMax);
	//sprintf(cSelect, "select * from device where device_id > %d and device_id <= %d", 1, uiDeviceIDRangMax);

	unsigned long long ullRowNum = 0;
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("select device error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return uiDeviceID;
	}
	else
	{
		if (prs != NULL)
		{
			//ResultSet::Status rss = prs->next();
			//printf("prs->next()=%d\r\n", rss);
			////printf("ResultSet::status()=%d\r\n", prs->status());

			// 注意：列的编号是从1开始,如果是获取max，肯定是有结果的，只不过结果可能是空，所以会进入while循环
			while (prs->next())
			{
				ullRowNum++;
				if (ullRowNum == 1)
				{
					uiDeviceID = prs->getInt(1);

					printf("max(device_id)=%d\r\n", uiDeviceID);

					// 有值但是为空
					if (uiDeviceID == 0)
					{
						uiDeviceID = uiDeviceIDRangMin;
						break;
					}

					uiDeviceID++;
					break;
				}
			}

			// 出厂时，该表是空表,此时获得值为0
			if (uiDeviceID == 0)
			{
				uiDeviceID = uiDeviceIDRangMin;
			}
		}
		else
		{
			printf("prs== NULL max(device_id)=%d\r\n", uiDeviceID);
		}

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);
	}
#endif
	return uiDeviceID;
}


int CDevice::GenerateDeviceID()
{
	int devid = 0;

	devid = SelectDB(m_uiDevmgrNo); // uiDevmgrNo最多支持8bit，即0-255个编号

	////组合iDevmgrNum 和 当前数据库行数
	//m_device_id = ((uiDevmgrNo << 24) + iDevice_id); // 如果mysql中的设备数iDevice_id超过256可能会有问题
	//m_pub_device_id = m_device_id;
	m_device_id = devid;
	m_pub_device_id = m_device_id;

	sr_printf(SR_LOGLEVEL_INFO, ">>>>>>>>>CDevice::GenerateDeviceID  m_uiDevmgrNo>>> %d, devid = %d, m_device_id = %d, m_pub_device_id = %d>>>\n", m_uiDevmgrNo, devid, m_device_id, m_pub_device_id);
	//printf(">>>>>>>>>CDevice::GenerateDeviceID  m_uiDevmgrNo>>> %d, devid = %d, m_device_id>>> %d\n", m_uiDevmgrNo, devid, m_device_id);

	return devid;
}