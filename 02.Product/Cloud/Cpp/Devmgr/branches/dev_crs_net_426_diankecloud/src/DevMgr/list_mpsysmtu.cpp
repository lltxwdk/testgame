#ifdef WIN32
#include "stdafx.h"
#endif
#include "list_mpsysmtu.h"
#include <cstdlib>
#include <stdio.h>

CMpSysMtu::CMpSysMtu()
{
	m_MP_MaxMtu = 1100;
}

CMpSysMtu::~CMpSysMtu()
{

}

bool CMpSysMtu::SelectMPSysMtu()
{
	CCriticalAutoLock MyLock(m_csCCMpSysMtu);

	char cSelect[1024];

	sprintf(cSelect, "select sp_value from t_sysparameter where sp_code = '%s' ", "sys.config.network.mtu");

#ifdef MYSQL_DB_DEF
	unsigned long long llRowNum = 0;
	MYSQL_ROW row;
	MYSQL_RES *pQuery = NULL;

	if (!CDBConnector::GetInstance()->GetResult(cSelect, llRowNum, row, &pQuery))
	{
		printf("CMpSysMtu::SelectMPSysMtu() GetResult error\r\n");

		sr_printf(SR_LOGLEVEL_WARNING, "CMpSysMtu::SelectMPSysMtu() error  \r\n");

		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^1111111^^^^^^^^^^^^^ CMpSysMtu::SelectMPSysMtu()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);
		return false;
	}

	if (llRowNum == 1)
	{
		if (row != NULL && row[0] != NULL)
		{
			m_MP_MaxMtu = atoi(row[0]);
			printf("CMpSysMtu::SelectMPSysMtu() OK m_MP_MaxMtu:%s->%d\r\n", row[0], m_MP_MaxMtu);
			sr_printf(SR_LOGLEVEL_INFO, "CMpSysMtu::SelectMPSysMtu() OK m_MP_MaxMtu:%s->%d\r\n", row[0], m_MP_MaxMtu);
		}

	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CMpSysMtu::SelectMPSysMtu()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif  SHENTONG_DB_DEF
	unsigned long long ullRowNum = 0;
	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("select t_sysparameter error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		if (prs != NULL)
		{
			while (prs->next())
			{
				ullRowNum++;
				if (ullRowNum == 1)
				{
					unsigned int uiMP_MaxMtu = 0;
					uiMP_MaxMtu = prs->getInt(1);
					m_MP_MaxMtu = uiMP_MaxMtu;

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