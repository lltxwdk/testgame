#ifdef WIN32
#include "stdafx.h"
#endif

#include "list_confparticipant.h"
#include <cstdlib>
#include <stdio.h>


CConfParticipant::CConfParticipant()
{
	m_participant_id = 0;
	m_conf_id = 0;
	m_user_id = 0;
	m_order_no = 0;
}

CConfParticipant::~CConfParticipant()
{
	m_participant_id = 0;
	m_conf_id = 0;
	m_user_id = 0;
	m_order_no = 0;
}

bool CConfParticipant::InsertDB()
{
	return true;
}

bool CConfParticipant::DeleteDB()
{
	return true;
}

bool CConfParticipant::UpdateDB()
{
	return true;
}

// 目前还没用
bool CConfParticipant::SelectDB()
{
	CCriticalAutoLock MyLock(m_csConfParticipant);

	char cSelect[1024];
#ifdef WIN32
	sprintf(cSelect, "select user_id,conf_id,participants_id,order_no from conference_participants where conf_id='%I64d'", m_conf_id);
#elif defined LINUX
	sprintf(cSelect, "select user_id,conf_id,participants_id,order_no from conference_participants where conf_id='%lld'", m_conf_id);
#endif

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CConfParticipant::SelectDB GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CConfParticipant::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return false;
	}
	else
	{
		// 处理结果

		// 测试
		for (std::map<unsigned long long, MYSQL_ROW>::iterator row_itor = rows_set.begin();
			row_itor != rows_set.end(); row_itor++)
		{
			MYSQL_ROW ret_row = row_itor->second;
#ifdef WIN32
			printf("CConfParticipant::RowNum:'%I64d'={%d, '%I64d', '%I64d', %d}\n", (row_itor->first) + 1, atoi(ret_row[0]), _atoi64(ret_row[1]), _atoi64(ret_row[2]), atoi(ret_row[3]));
#elif defined LINUX
			printf("CConfParticipant::RowNum:'%lld'={%d, '%lld', '%lld', %d}\n", (row_itor->first) + 1, atoi(ret_row[0]), strtoll(ret_row[1], NULL, 10), strtoll(ret_row[2], NULL, 10), atoi(ret_row[3]));
#endif

		}
	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CConfParticipant::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF
	return true;
#endif

}


bool CConfParticipant::SelectParticipantUserInfo(std::map<unsigned int, ParticipantInfo*> &participantInfos)
{
	CCriticalAutoLock MyLock(m_csConfParticipant);

	char cSelect[1024];

#ifdef WIN32
	sprintf(cSelect, "select cp.conf_id,cp.participants_id,cp.order_no,u.user_id,u.user_type,u.user_alias,u.third_uid,ter.terminal_ip,ter.protocol_type,ter.terminal_bandwidth,ter.terminal_type,ter.terminal_phone  from conference_participants cp inner join user u on cp.user_id=u.user_id left join terminal ter on u.user_id=ter.terminal_user_id where conf_id='%I64d' and u.user_status=1", m_conf_id);
#elif defined LINUX
	sprintf(cSelect, "select cp.conf_id,cp.participants_id,cp.order_no,u.user_id,u.user_type,u.user_alias,u.third_uid,ter.terminal_ip,ter.protocol_type,ter.terminal_bandwidth,ter.terminal_type,ter.terminal_phone  from conference_participants cp inner join user u on cp.user_id=u.user_id left join terminal ter on u.user_id=ter.terminal_user_id where conf_id='%lld' and u.user_status=1", m_conf_id);
#endif

#ifdef MYSQL_DB_DEF

	unsigned long long llRowNum = 0;
	MYSQL_RES *pQuery = NULL;

	std::map<unsigned long long, MYSQL_ROW> rows_set;
	rows_set.clear();

	if (!CDBConnector::GetInstance()->GetResults(cSelect, llRowNum, rows_set, &pQuery))
	{
		printf("CConfParticipant::SelectDB GetResults error\r\n");
		printf(CDBConnector::GetInstance()->OutErrors());

		printf("^^^^^^^^^^111111111111^^^^^^^^^^^^^^ CConfParticipant::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);

		CDBConnector::GetInstance()->FreeResult(pQuery);

		return false;
	}
	else
	{
		// 处理结果

		participantInfos.clear();
		// 测试
		for (std::map<unsigned long long, MYSQL_ROW>::iterator row_itor = rows_set.begin();
			row_itor != rows_set.end(); row_itor++)
		{
			MYSQL_ROW ret_row = row_itor->second;
			if (ret_row)
			{
				//ret_row[0]; // conference_participants.conf_id
				//ret_row[1]; // conference_participants.participants_id
				//ret_row[2]; // conference_participants.order_no
				unsigned int uiOrderno = 0;
				if (ret_row[2] != NULL)
				{
					uiOrderno = atoi(ret_row[2]);
				}

				unsigned int uiUserid = 0;
				uiUserid = atoi(ret_row[3]);

				unsigned int uiUserType = 0;
				if (ret_row[4] != NULL)
				{
					uiUserType = atoi(ret_row[4]); // 0-临时用户,1-正式用户,2-Ubox,3-标准终端(H.323/sip),4-监控前端
				}

				std::string str_user_alias;
				str_user_alias.clear();
				if (ret_row[5] != NULL)
				{
					str_user_alias = ret_row[5];
				}

				std::string str_user_third_uid;
				str_user_third_uid.clear();
				if (uiUserType == 4)
				{
					if (ret_row[6] != NULL)
					{
						str_user_third_uid = ret_row[6];
					}
				}

				std::string str_user_ip;
				str_user_ip.clear();
				unsigned int uiUserProtocol = 0;
				unsigned int uiUserBandwidth = 0;
				unsigned int uiDevicetype = 0;
				std::string str_short_name;
				str_short_name.clear();
				if (uiUserType == 3
					|| uiUserType == 4)
				{
					if (ret_row[7] != NULL)
					{
						str_user_ip = ret_row[7];
					}

					if (ret_row[8] != NULL)
					{
						uiUserProtocol = atoi(ret_row[8]); // 标准终端的呼叫协议类型,0-自动,1-sip,2-H.323,3-GB28181
					}
					if (ret_row[9] != NULL)
					{
						uiUserBandwidth = atoi(ret_row[9]);
					}
					if (ret_row[10] != NULL)
					{
						uiDevicetype = atoi(ret_row[10]); // 设备类型,0-终端、1-MCU、2-监控、3-PSTN语音电话(包括固话、手机)
					}
					if (ret_row[11] != NULL)
					{
						str_short_name = ret_row[11];// 标准终端的短号
					}
				}

#ifdef WIN32
				printf("SelectParticipantUserInfo::RowNum:%I64d={%I64d, %I64d, %d, %d, %d, %s, %s, %d, %d, %d, %s}\n", (row_itor->first) + 1, _atoi64(ret_row[0]), _atoi64(ret_row[1]), uiOrderno, uiUserid, uiUserType, str_user_alias.c_str(), str_user_ip.c_str(), uiUserProtocol, uiUserBandwidth, uiDevicetype, str_short_name.c_str());
#elif defined LINUX
				printf("SelectParticipantUserInfo::RowNum:%lld={%lld, %lld, %d, %d, %d, %s, %s, %d, %d, %d, %s}\n", (row_itor->first) + 1, strtoll(ret_row[0], NULL, 10), strtoll(ret_row[1], NULL, 10), uiOrderno, uiUserid, uiUserType, str_user_alias.c_str(), str_user_ip.c_str(), uiUserProtocol, uiUserBandwidth, uiDevicetype, str_short_name.c_str());
#endif

				if (uiUserid != 0)
				{
					ParticipantInfo* pPartInfo = new ParticipantInfo();
					pPartInfo->m_psuid = uiUserid;
					pPartInfo->m_usertype = uiUserType;
					pPartInfo->m_nickname = str_user_alias;
					pPartInfo->m_ip = str_user_ip;
					pPartInfo->m_protocoltype = uiUserProtocol;
					pPartInfo->m_bandwidth = uiUserBandwidth;
					pPartInfo->m_devicetype = uiDevicetype;
					pPartInfo->m_devicecode = str_user_third_uid;
					pPartInfo->m_shortname = str_short_name;
					pPartInfo->m_orderno = uiOrderno;
					participantInfos.insert(std::make_pair(uiUserid, pPartInfo));
				}
			}

		}
	}

	printf("^^^^^^^^22222222^^^^^^^^^^^^^ CConfParticipant::SelectDB()  FreeResult( pQuery = %p ) ^^^^^^^^^^^^^^^^^^\n", pQuery);
	CDBConnector::GetInstance()->FreeResult(pQuery);

	return true;
#elif SHENTONG_DB_DEF

	Statement *pstmt = NULL;
	ResultSet *prs = NULL;
	bool bgetok = false;

	if (!CDBConnector::GetInstance()->GetResults(cSelect, &pstmt, &prs))
	{
		printf("CConfParticipant::SelectDB GetResults error\r\n");

		CDBConnector::GetInstance()->FreeResult(pstmt, prs);

		return false;
	}
	else
	{
		participantInfos.clear();
		if (prs != NULL)
		{
			unsigned long long ullRowNum = 0;
			// 注意：列的编号是从1开始,如果结果为空也会进入while循环,此时获取的值是0
			while (prs->next())
			{
				ullRowNum++;
				//prs[1]; // conference_participants.conf_id
				//prs[2]; // conference_participants.participants_id
				//prs[3]; // conference_participants.order_no
				std::string strtemp;

				unsigned long long ullConfid = 0;
				//				strtemp.clear();
				//				strtemp = prs->getString(1);
				//#ifdef WIN32
				//				ullConfid = _atoi64(strtemp.c_str());
				//#elif defined LINUX
				//				ullConfid = strtoll(strtemp.c_str(), NULL, 10);
				//#endif	
				ullConfid = (unsigned long)prs->getNumber(1);

				unsigned long long ullPartid = 0;
				//				strtemp.clear();
				//				strtemp = prs->getString(2);
				//#ifdef WIN32
				//				ullPartid = _atoi64(strtemp.c_str());
				//#elif defined LINUX
				//				ullPartid = strtoll(strtemp.c_str(), NULL, 10);
				//#endif
				ullPartid = (unsigned long)prs->getNumber(2);

				unsigned int uiOrderno = 0;
				uiOrderno = prs->getInt(3);

				unsigned int uiUserid = 0;
				uiUserid = prs->getInt(4);

				unsigned int uiUserType = 0;
				uiUserType = prs->getInt(5);// 0-临时用户,1-正式用户,2-Ubox,3-标准终端(H.323/sip),4-监控前端

				std::string str_user_alias;
				str_user_alias.clear();
				str_user_alias = prs->getString(6);

				std::string str_user_third_uid;
				str_user_third_uid.clear();
				if (uiUserType == 4)
				{
					str_user_third_uid = prs->getString(7);
				}

				std::string str_user_ip;
				str_user_ip.clear();
				unsigned int uiUserProtocol = 0;
				unsigned int uiUserBandwidth = 0;
				unsigned int uiDevicetype = 0;
				std::string str_short_name;
				str_short_name.clear();
				if (uiUserType == 3
					|| uiUserType == 4)
				{
					str_user_ip = prs->getString(8);
					uiUserProtocol = prs->getInt(9);// 标准终端的呼叫协议类型,0-自动,1-sip,2-H.323,3-GB28181
					uiUserBandwidth = prs->getInt(10);
					uiDevicetype = prs->getInt(11);// 设备类型,0-终端、1-MCU、2-监控、3-PSTN语音电话(包括固话、手机)
					str_short_name = prs->getString(12);// 标准终端的短号
				}

#ifdef WIN32
				printf("SelectParticipantUserInfo::RowNum:%I64d={%I64d, %I64d, %d, %d, %d, %s, %s, %d, %d, %d, %s}\n", ullRowNum, ullConfid, ullPartid, uiOrderno, uiUserid, uiUserType, str_user_alias.c_str(), str_user_ip.c_str(), uiUserProtocol, uiUserBandwidth, uiDevicetype, str_short_name.c_str());
#elif defined LINUX
				printf("SelectParticipantUserInfo::RowNum:%lld={%lld, %lld, %d, %d, %d, %s, %s, %d, %d, %d, %s}\n", ullRowNum, ullConfid, ullPartid, uiOrderno, uiUserid, uiUserType, str_user_alias.c_str(), str_user_ip.c_str(), uiUserProtocol, uiUserBandwidth, uiDevicetype, str_short_name.c_str());
#endif

				if (uiUserid != 0)
				{
					ParticipantInfo* pPartInfo = new ParticipantInfo();
					pPartInfo->m_psuid = uiUserid;
					pPartInfo->m_usertype = uiUserType;
					pPartInfo->m_nickname = str_user_alias;
					pPartInfo->m_ip = str_user_ip;
					pPartInfo->m_protocoltype = uiUserProtocol;
					pPartInfo->m_bandwidth = uiUserBandwidth;
					pPartInfo->m_devicetype = uiDevicetype;
					pPartInfo->m_devicecode = str_user_third_uid;
					pPartInfo->m_shortname = str_short_name;
					pPartInfo->m_orderno = uiOrderno;
					participantInfos.insert(std::make_pair(uiUserid, pPartInfo));
				}
			}

			if (ullRowNum > 0)
			{
				bgetok = true;
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