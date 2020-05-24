
/*enum ObjectStateEnum
{	
	adStateClosed	= 0,
	adStateOpen	= 0x1,
	adStateConnecting	= 0x2,
	adStateExecuting	= 0x4,
	adStateFetching	= 0x8
} 	ObjectStateEnum;*/
// Database.cpp: implementation of the CDBConnector class.
//
//////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include "stdafx.h"
#include "winsock2.h"
#elif defined LINUX
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif
#include "parser.h"
#include "DBconnector.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDBConnector::CDBConnector()
: m_bconnflag(false)
{
#ifdef MYSQL_DB_DEF
	mysql_init(&mysql);
	char value = 1;
	mysql_options(&mysql, MYSQL_OPT_RECONNECT, (char *)&value);
	int ml_outtime = 30;
	mysql_options(&mysql, MYSQL_OPT_CONNECT_TIMEOUT, (char *)&ml_outtime);

	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf8");
	//mysql_set_character_set(&mysql, "utf8");

	m_pQueryRes = NULL;
#elif SHENTONG_DB_DEF

	m_penv = NULL;/*环境句柄*/

	InitDB();

	//try
	//{
	//	m_penv = shentong::acci::Environment::createEnvironment(shentong::acci::Environment::DEFAULT);
	//	if (m_penv != NULL)
	//	{
	//		sr_printf(SR_LOGLEVEL_NORMAL, "CDBConnector::CDBConnector createEnvironment ok\n");
	//	}
	//	else
	//	{
	//		sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::CDBConnector createEnvironment error\n");
	//	}
	//}
	//catch (shentong::acci::SQLException &e)
	//{
	//	sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::CDBConnector SQLException msg:%s \n", e.getMessage().c_str());
	//}
#endif
}

CDBConnector::~CDBConnector()
{
#ifdef MYSQL_DB_DEF
	printf("~CDBConnector() begin\n");
	mysql_close(&mysql);
	mysql_free_result(query);

	if (m_pQueryRes != NULL)
	{
		mysql_free_result(m_pQueryRes);
	}
	printf("~CDBConnector() end\n");
#elif SHENTONG_DB_DEF
	if (m_penv != NULL)
	{
		shentong::acci::Environment::terminateEnvironment(m_penv);
		m_penv = NULL;
	}
#endif
}

CDBConnector* CDBConnector::GetInstance()
{
	static CDBConnector g_cDBConnector;
	return &g_cDBConnector;
}

void CDBConnector::InitDB()
{
#ifdef MYSQL_DB_DEF
	//mysql_init(&mysql);
	//char value = 1;
	//mysql_options(&mysql, MYSQL_OPT_RECONNECT, (char *)&value);
	//int ml_outtime = 30;
	//mysql_options(&mysql, MYSQL_OPT_CONNECT_TIMEOUT, (char *)&ml_outtime);

	//mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf8");
	////mysql_set_character_set(&mysql, "utf8");

	//m_pQueryRes = NULL;
#elif SHENTONG_DB_DEF
	ReleaseDB();
	if (m_penv == NULL)
	{
		try
		{
			m_penv = shentong::acci::Environment::createEnvironment(shentong::acci::Environment::DEFAULT);
			if (m_penv != NULL)
			{
				sr_printf(SR_LOGLEVEL_NORMAL, "CDBConnector::Init shentong DB createEnvironment ok\n");
			}
			else
			{
				m_penv = NULL;
				sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::Init shentong DB createEnvironment error\n");
			}
		}
		catch (shentong::acci::SQLException &err)
		{
			m_penv = NULL;
			sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::Init shentong DB SQLException errorcode:%d<-->msg:%s \n", err.getErrorCode(), err.getMessage().c_str());
		}
	}
#endif
}

void CDBConnector::ReleaseDB()
{
#ifdef MYSQL_DB_DEF
	//mysql_init(&mysql);
	//char value = 1;
	//mysql_options(&mysql, MYSQL_OPT_RECONNECT, (char *)&value);
	//int ml_outtime = 30;
	//mysql_options(&mysql, MYSQL_OPT_CONNECT_TIMEOUT, (char *)&ml_outtime);

	//mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf8");
	////mysql_set_character_set(&mysql, "utf8");

	//m_pQueryRes = NULL;
#elif SHENTONG_DB_DEF	
	try
	{
		if (m_penv != NULL)
		{
			if (m_pconn != NULL)
			{
				sr_printf(SR_LOGLEVEL_NORMAL, "CDBConnector::ReleaseDB terminateConnection begin\n");
				m_penv->terminateConnection(m_pconn);
				m_pconn = NULL;

				sr_printf(SR_LOGLEVEL_NORMAL, "CDBConnector::ReleaseDB terminateConnection over\n");
			}

			sr_printf(SR_LOGLEVEL_NORMAL, "CDBConnector::ReleaseDB terminateEnvironment begin\n");
			shentong::acci::Environment::terminateEnvironment(m_penv);
			m_penv = NULL;

			sr_printf(SR_LOGLEVEL_NORMAL, "CDBConnector::ReleaseDB terminateEnvironment over\n");
		}
	}
	catch (shentong::acci::SQLException &err)
	{
		m_pconn = NULL;
		m_penv = NULL;
		sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::Release shentong DB SQLException errorcode:%d<-->msg:%s \n", err.getErrorCode(), err.getMessage().c_str());
	}
#endif
}

// 目前只供Insert、Delete、Update用
bool CDBConnector::ExectCommand(char *para)
{	
	CCriticalAutoLock mylock(m_cs);

	if (!m_bconnflag)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::ExectCommand== m_bconnflag == false so reconnect db----->>\n");

		bool bres = ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
			CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
			CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
		if (!bres)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::ExectCommand==m_bconnflag == false reconnect db error !!!----->> \n");
			return false;
		}
	}
#ifdef MYSQL_DB_DEF
		
	if ((&mysql) == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::ExectCommand==---- mysql == NULL error !!!----->> \n");
		return false;
	}

	if (mysql_query(&mysql, para))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::ExectCommand==---- mysql_query  error !!!----->> \n");
		return false;
	}
	return true;
#elif SHENTONG_DB_DEF

	bool bexectok = false;

	try
	{
		if (m_pconn != NULL)
		{
			Statement *stmt = m_pconn->createStatement(para);
			if (stmt != NULL)
			{
				stmt->setAutoCommit(true);
				//Statement::Status sms = stmt->execute();
				//printf("CDBConnector::ExectCommand stmt->execute()=%d\r\n", sms);

				unsigned int uiRowNum = 0;
				uiRowNum = stmt->executeUpdate();
				if (uiRowNum > 0)
				{
					bexectok = true;
				}

				m_pconn->commit();

				m_pconn->terminateStatement(stmt);
			}
			else
			{
				bexectok = false;
			}
		}
		else
		{
			bexectok = false;
		}
	}
	catch (shentong::acci::SQLException &err)
	{
		bexectok = false;
		sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::ExectCommand shentong DB SQLException errorcode:%d<-->msg:%s \n", err.getErrorCode(), err.getMessage().c_str());

		// 3113	连接已经断开、3114 无法建立连接
		if (err.getErrorCode() == 3113
			|| err.getErrorCode() == 3114)
		{
			m_bconnflag = false;
		}
	}
	
	sr_printf(SR_LOGLEVEL_INFO, "CDBConnector::ExectCommand[bexectok=%d] finish.\n", bexectok);

	return bexectok;
#endif
}

#ifdef MYSQL_DB_DEF
bool CDBConnector::SelectRecord(char *para)
{
	CCriticalAutoLock mylock(m_cs);
	if (!m_bconnflag)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::SelectRecord== m_bconnflag == false so reconnect db----->>\n");

		bool bres = ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
			CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
			CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
		if (!bres)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::SelectRecord==m_bconnflag == false reconnect db error !!!----->> \n");
			return false;
		}
	}
	if ((&mysql) != NULL)
	{
		if (mysql_query(&mysql, para))
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::SelectRecord==---- mysql_query  error !!!----->> \n");
			return false;
		}
		query = mysql_store_result(&mysql);
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::SelectRecord==---- mysql == NULL error !!!----->> \n");
		return false;
	}
	return true;
}

my_ulonglong CDBConnector::GetRowNum()
{
	CCriticalAutoLock mylock(m_cs);
	if (!m_bconnflag)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetRowNum== m_bconnflag == false so reconnect db----->>\n");

		bool bres = ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
			CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
			CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
		if (!bres)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetRowNum==m_bconnflag == false reconnect db error !!!----->> \n");
			return 0;
		}
	}

	if (&mysql == NULL
		|| query == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetRowNum==mysql == null or query == null  !!!----->> \n");
		return 0;
	}
	else
	{
		return (mysql_num_rows(query));
	}
}

MYSQL_ROW CDBConnector::GetRecord()
{
	CCriticalAutoLock mylock(m_cs);
	if (!m_bconnflag)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetRecord== m_bconnflag == false so reconnect db----->>\n");

		bool bres = ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
			CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
			CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
		if (!bres)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetRecord==m_bconnflag == false reconnect db error !!!----->> \n");
			return NULL;
		}
	}
	
	if (&mysql == NULL
		|| query == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetRecord==mysql == null or query == null  !!!----->> \n");
		return NULL;
	}
	else
	{
		return (row = mysql_fetch_row(query));
	}
}
#endif
const char * CDBConnector::OutErrors()
{
	if (!m_bconnflag)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::OutErrors== m_bconnflag == false so reconnect db----->>\n");

		bool bres = ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
			CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
			CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
		if (!bres)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::OutErrors==m_bconnflag == false reconnect db error !!!----->> \n");
			return NULL;
		}
	}

#ifdef MYSQL_DB_DEF
	if (&mysql == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::OutErrors== mysql == null !!!----->> \n");
		return NULL;
	}
	else
	{
		return (mysql_error(&mysql));
	}
#elif SHENTONG_DB_DEF

	return NULL;
#endif
}

bool CDBConnector::ConnectDB(const char* host, const char* user, const char* pass, const char* libname, const unsigned int port)
{
	m_bconnflag = false;
#ifdef MYSQL_DB_DEF
	do
	{
		if ((&mysql) != NULL)
		{
			if (!mysql_real_connect(&mysql, host, user, pass, libname, port, 0, 0))
			{
				sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::ConnectDB==----mysql_real_connect  error !!!----->> \n");
				break;
			}

			mysql_query(&mysql, "set names UTF-8");//防止乱码。设置和数据库的编码一致就不会乱码
			m_bconnflag = true;
		}
	} while (0);

	return m_bconnflag;
#elif SHENTONG_DB_DEF

	if (m_penv == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::ConnectDB shentong env not create.\n");
		// 再次尝试创建环境变量
		InitDB();
		return m_bconnflag;
	}

	//char *m_dblink = "127.0.0.1:2003/OSRDB";
	//char *m_dbuser = "SYSDBA";
	//char *m_dbpwd = "szoscar55";

	char dblink[256] = { 0 };
	sprintf(dblink, "%s:%u/%s", host, port, libname);

	CloseDB();

	try
	{
		m_pconn = m_penv->createConnection(user, pass, dblink);
		if (m_pconn != NULL)
		{
			m_bconnflag = true;

			sr_printf(SR_LOGLEVEL_NORMAL, "CDBConnector::ConnectDB shentong m_penv=%p createConnection(%p) ok.\n", m_penv, m_pconn);
		}
		else
		{
			m_pconn = NULL;
			sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::ConnectDB shentong createConnection error, so set m_pconn is NULL.\n");
		}
	}
	catch (shentong::acci::SQLException &err)
	{
		m_pconn = NULL;
		sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::ConnectDB shentong SQLException errorcode:%d<-->msg:%s and set m_pconn is NULL\n", err.getErrorCode(), err.getMessage().c_str());
	}
	
	return m_bconnflag;
#endif
}

void CDBConnector::CloseDB(bool bClose)
{
	sr_printf(SR_LOGLEVEL_NORMAL, "CDBConnector::CloseDB begin\n");
#ifdef MYSQL_DB_DEF
	if ((&mysql) != NULL)
		mysql_close(&mysql);
#elif SHENTONG_DB_DEF
	try
	{
		//if (m_bconnflag)
		{
			if (m_penv != NULL)
			{
				if (m_pconn != NULL)
				{
					sr_printf(SR_LOGLEVEL_INFO, "CDBConnector::CloseDB m_penv=%p terminateConnection(%p) and set m_pconn is NULL.\n", m_penv, m_pconn);
					m_penv->terminateConnection(m_pconn);
					m_pconn = NULL;
				}
				else
				{
					sr_printf(SR_LOGLEVEL_INFO, "CDBConnector::CloseDB m_pconn is NULL.\n");
				}
			}
			else
			{
				sr_printf(SR_LOGLEVEL_INFO, "CDBConnector::CloseDB m_penv is NULL.\n");
			}
		}
	}
	catch (shentong::acci::SQLException &err)
	{
		m_pconn = NULL;
		sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::CloseDB shentong SQLException errorcode:%d<-->msg:%s and set m_pconn is NULL\n", err.getErrorCode(), err.getMessage().c_str());
	}
#endif
	sr_printf(SR_LOGLEVEL_NORMAL, "CDBConnector::CloseDB end and m_bconnflag is false\n");
	m_bconnflag = false;
}

#ifdef MYSQL_DB_DEF
bool CDBConnector::GetResult(char *para, unsigned long long &llrowsnum, MYSQL_ROW &row, MYSQL_RES **query)
{
	CCriticalAutoLock mylock(m_cs);
	if (!m_bconnflag)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetResult== m_bconnflag == false so reconnect mysql db----->>\n");

		bool bres = ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
			CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
			CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
		if (!bres)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetResult==m_bconnflag == false reconnect mysql db error !!!----->> \n");
			return false;
		}
	}

	if (&mysql == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "############ CDBConnector::GetResult mysql == NULL error!!!!! ############ \n");
		return false;
	}
	if (mysql_query(&mysql, para))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "@@@@@@@@@@@@@@@@ CDBConnector::GetResult mysql_query error!!!!! @@@@@@@@@@ \n");
		return false;
	}

	(*query) = mysql_store_result(&mysql); // 该结果集用完一定要调用mysql_free_result进行释放

	if ((*query) != NULL)
	{
		llrowsnum = mysql_num_rows((*query));
		if (llrowsnum <= 0)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "@@@@@@@@@@@@@@@@ CDBConnector::GetResult mysql_store_result error!!!!! @@@@  (0 == llrowsnum) @@@@@@ \n");
			return false;
		}
		else
		{
			row = mysql_fetch_row((*query));

#ifdef WIN32
			printf("@@@@@@@@@@@@@@@@ CDBConnector::GetResult mysql_query and mysql_store_result OK !!!!! llrowsnum = %I64d , query = %p, (*query) = %p\n", llrowsnum, query, (*query));
#elif defined LINUX
			printf("@@@@@@@@@@@@@@@@ CDBConnector::GetResult mysql_query and mysql_store_result OK !!!!! llrowsnum = %lld , query = %p, (*query) = %p\n", llrowsnum, query, (*query));
#endif

			return true;
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "$$$$$$$$$$$$$$$ CDBConnector::GetResult mysql query == NULL  memleak !!!!!!!!!!!!!!!!!!!\n");
		return false;
	}

}

bool CDBConnector::GetResults(char *para, unsigned long long &llrowsnum, std::map<unsigned long long, MYSQL_ROW> &rows_set, MYSQL_RES **query)
{
	CCriticalAutoLock mylock(m_cs);
	if (!m_bconnflag)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetResults== m_bconnflag == false so reconnect mysql db----->>\n");

		bool bres = ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
			CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
			CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
		if (!bres)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetResults==m_bconnflag == false reconnect mysql db error !!!----->> \n");
			return false;
		}
	}

	if (&mysql == NULL)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "############ CDBConnector::GetResults mysql == NULL error!!!!! ############ \n");
		return false;
	}
	if (mysql_query(&mysql, para))
	{
		sr_printf(SR_LOGLEVEL_ERROR, "@@@@@@@@@@@@@@@@ CDBConnector::GetResults mysql_query error!!!!! @@@@@@@@@@ \n");
		return false;
	}
	
	(*query) = mysql_store_result(&mysql); // 该结果集用完一定要调用mysql_free_result进行释放

	if ((*query) != NULL)
	{
		llrowsnum = mysql_num_rows((*query));
		if (llrowsnum <= 0)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "@@@@@@@@@@@@@@@@ CDBConnector::GetResults mysql_store_result error!!!!! @@@@  (0 == llrowsnum) @@@@@@ \n");
			return false;
		}
		else
		{
			rows_set.clear();
			for (my_ulonglong i = 0; i < llrowsnum; i++)
			{
				MYSQL_ROW row = mysql_fetch_row((*query));
				rows_set.insert(std::make_pair(i, row));
			}

#ifdef WIN32
			printf("@@@@@@@@@@@@@@@@ CDBConnector::GetResults mysql_query and mysql_store_result OK !!!!! llrowsnum = %I64d , query = %p, (*query) = %p\n", llrowsnum, query, (*query));
#elif defined LINUX
			printf("@@@@@@@@@@@@@@@@ CDBConnector::GetResults mysql_query and mysql_store_result OK !!!!! llrowsnum = %lld , query = %p, (*query) = %p\n", llrowsnum, query, (*query));
#endif
			return true;
		}
	}
	else
	{
		sr_printf(SR_LOGLEVEL_ERROR, "$$$$$$$$$$$$$$$ CDBConnector::GetResults  mysql query == NULL  memleak !!!!!!!!!!!!!!!!!!!\n");
		return false;
	}
	
}

bool CDBConnector::FreeResult(MYSQL_RES *query)
{
	CCriticalAutoLock mylock(m_cs);
	if (!m_bconnflag)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::FreeResult== m_bconnflag == false so reconnect mysql db----->>\n");

		bool bres = ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
			CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
			CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
		if (!bres)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::FreeResult==m_bconnflag == false reconnect mysql db error !!!----->> \n");
			return false;
		}
	}

	if (query != NULL)
	{
		sr_printf(SR_LOGLEVEL_WARNING, "@@@@@@@@@@@@@@@@ CDBConnector::FreeResult  query = %p, (*query) = %p\n\n", query, *query);

		mysql_free_result(query);
	}

	return true;
}

#elif SHENTONG_DB_DEF
// 输入 sql
// 输入/输出 pstmt
// 输入/输出 prs
bool CDBConnector::GetResults(char *sql, Statement **pstmt, ResultSet **prs)
{
	CCriticalAutoLock mylock(m_cs);

	if (!m_bconnflag)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetResults== m_bconnflag == false so reconnect shentong db----->>\n");

		bool bres = ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
			CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
			CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
		if (!bres)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::GetResults==m_bconnflag == false reconnect shentong db error !!!----->> \n");
			return false;
		}
	}

	bool bgetok = true;
	
	try
	{
		if (m_pconn != NULL)
		{
			(*pstmt) = m_pconn->createStatement(sql);
			if ((*pstmt) != NULL)
			{
				//(*pstmt)->execute();
				(*prs) = (*pstmt)->executeQuery(sql);
				if ((*prs) == NULL)
				{
					bgetok = false;
				}
			}
			else
			{
				bgetok = false;
			}
		}
		else
		{
			bgetok = false;
		}
	}
	catch (shentong::acci::SQLException &err)
	{
		bgetok = false;
		sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::GetResults shentong DB SQLException errorcode:%d<-->msg:%s \n", err.getErrorCode(), err.getMessage().c_str());

		// 3113	连接已经断开、3114 无法建立连接
		if (err.getErrorCode() == 3113
			|| err.getErrorCode() == 3114)
		{
			m_bconnflag = false;
		}
	}
	
	sr_printf(SR_LOGLEVEL_INFO, "CDBConnector::GetResults[bgetok=%d] executeQuery shentong db finish.\n", bgetok);

	return bgetok;
}
bool CDBConnector::FreeResult(Statement *pstmt, ResultSet *prs)
{
	CCriticalAutoLock mylock(m_cs);

	if (!m_bconnflag)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::FreeResult== m_bconnflag == false so reconnect shentong db----->>\n");

		bool bres = ConnectDB(CParser::GetInstance()->GetMysqlIpAddr().c_str(),
			CParser::GetInstance()->GetMysqlUser().c_str(), CParser::GetInstance()->GetMysqlPass().c_str(),
			CParser::GetInstance()->GetMysqlServName().c_str(), CParser::GetInstance()->GetMysqlPort());
		if (!bres)
		{
			sr_printf(SR_LOGLEVEL_ERROR, "====CDBConnector::FreeResult==m_bconnflag == false reconnect shentong db error !!!----->> \n");
			return false;
		}
	}
	
	try
	{
		if (pstmt != NULL)
		{
			if (prs != NULL)
			{
				pstmt->closeResultSet(prs);
				sr_printf(SR_LOGLEVEL_DEBUG, "CDBConnector::FreeResult shentong db Statement=%p closeResultSet(%p).\n", pstmt, prs);
			} 
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::FreeResult shentong db cur prs is NULL.\n");
			}

			if (m_pconn != NULL)
			{
				m_pconn->terminateStatement(pstmt);

				sr_printf(SR_LOGLEVEL_DEBUG, "CDBConnector::FreeResult shentong db m_pconn=%p terminateStatement(%p).\n", m_pconn, pstmt);
			}
			else
			{
				sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::FreeResult shentong db cur m_pconn is NULL.\n");
			}
		}
		else
		{
			sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::FreeResult shentong db cur Statement is NULL.\n");
		}
	}
	catch (shentong::acci::SQLException &err)
	{
		sr_printf(SR_LOGLEVEL_ERROR, "CDBConnector::FreeResult shentong DB SQLException errorcode:%d<-->msg:%s \n", err.getErrorCode(), err.getMessage().c_str());

		// 3113	连接已经断开、3114 无法建立连接
		if (err.getErrorCode() == 3113
			|| err.getErrorCode() == 3114)
		{
			m_bconnflag = false;
		}
	}
		
	sr_printf(SR_LOGLEVEL_INFO, "CDBConnector::FreeResult shentong db finish.\n");
	return true;
}
#endif
