
#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_deviceconfig.h"
#include <cstdlib>
#include <stdio.h>

CDeviceConfig::CDeviceConfig()
{
	m_device_config_serial = "";
	m_mediagroup_id = 0;
	m_device_config_type = 0;
	m_config_max_terms = 0;
}

CDeviceConfig::~CDeviceConfig()
{

}

//CDeviceConfig* pDeviceConfig
//{
//	static CDeviceConfig g_cDeviceConfig;
//	return &g_cDeviceConfig;
//}

bool CDeviceConfig::InsertDB()
{
	return true;
	char cInsert[1024];
	sprintf(cInsert, "insert into device_config(device_config_serial, mediagroup_id, device_config_type, config_max_terms\
					   ) values('%s','%d','%d','%d')", \
			m_device_config_serial.c_str(), m_mediagroup_id, m_device_config_type, m_config_max_terms);
	if (!CDBConnector::GetInstance()->ExectCommand(cInsert))
	{
		printf("insert device error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDeviceConfig::DeleteDB()
{
	return true;
	char cDelete[1024];
	sprintf(cDelete, "delete from device_config where device_config_serial = '%s'", m_device_config_serial.c_str());
	if (!CDBConnector::GetInstance()->ExectCommand(cDelete))
	{
		printf("delete device error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDeviceConfig::UpdateDB()
{
	return true;
	char cUpdate[1024];
	sprintf(cUpdate, "update device_config set mediagroup_id = '%d', device_config_type = '%d', config_max_terms = '%d', \
				where device_config_serial='%s'",\
				m_mediagroup_id, m_device_config_type, m_config_max_terms, m_device_config_serial.c_str());

	if (!CDBConnector::GetInstance()->ExectCommand(cUpdate))
	{
		printf("update device_config error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());
		return false;
	}

	return true;
}

bool CDeviceConfig::SelectDB()
{
	CCriticalAutoLock MyLock(m_csDevConfig);

	//获取后，更新本地变量
	char cSelect[1024];
	sprintf(cSelect, "select device_config_serial,mediagroup_id,device_config_type,config_max_terms from device_config where device_config_serial='%s' and device_config_type='%d' and is_del=0", m_device_config_serial.c_str(), m_device_config_type);

#ifdef MYSQL_DB_DEF
	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		//printf("CDeviceConfig::SelectDB GetResult error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^111111111111^^^^^^^^^^^^^ CDeviceConfig::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

	//#ifdef WIN32
	//	printf(" CDeviceConfig::SelectDB GetResult llRowNum = %I64d\n", llRowNum);
	//#elif defined LINUX
	//	printf(" CDeviceConfig::SelectDB GetResult llRowNum = %lld\n", llRowNum);
	//#endif


	//if (nRow == 1)
	if (llRowNum == 1)
	{
		//MYSQL_ROW row = CDBConnector::GetInstance()->GetRecord();
		if (row != NULL && row[0] != 0)
		{
			m_device_config_serial = row[0];
		}
		else
		{
			m_device_config_serial.clear();
		}
		if (row != NULL && row[1] != 0)
			m_mediagroup_id = atoi(row[1]);
		if (row != NULL && row[2] != 0)
			m_device_config_type = atoi(row[2]);
		if (row != NULL && row[3] != 0)
			m_config_max_terms = atoi(row[3]);
	}


	printf("^^^^^^^^2222222222222^^^^^^^^^^^^^ CDeviceConfig::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF
	unsigned long long ullRowNum = 0;
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;
	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CDeviceConfig::SelectDB error\r\n");

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
					m_device_config_serial.clear();
					m_device_config_serial = prs->getString(1);

					int iMediagroupid = 0;
					iMediagroupid = prs->getInt(2);
					m_mediagroup_id = iMediagroupid;

					int iDeviceconfigtype = 0;
					iDeviceconfigtype = prs->getInt(3);
					m_device_config_type = iDeviceconfigtype;

					int iConfigmaxterms = 0;
					iConfigmaxterms = prs->getInt(4);
					m_config_max_terms = iConfigmaxterms;

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
