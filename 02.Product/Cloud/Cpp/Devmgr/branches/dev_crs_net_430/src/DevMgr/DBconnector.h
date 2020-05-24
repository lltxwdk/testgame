// DBconnector.h: interface for the CDatabase class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _DBCONNECTOR_H
#define _DBCONNECTOR_H

#ifdef MYSQL_DB_DEF
#include	<mysql.h>
#elif SHENTONG_DB_DEF
#include "ACCI.h"
using namespace shentong::acci;
using namespace std;
#endif

#ifdef MYSQL_DB_DEF

#elif SHENTONG_DB_DEF

#endif

#ifndef WIN32
#include <dlfcn.h>
#include "stdlib.h"
#endif
#include "BaseThread.h"

#include "sr_type.h"

#include <vector>
#include <map>

class CParser;
class CDBConnector  
{
public:
	virtual ~CDBConnector();
	
	static CDBConnector* GetInstance();
		
	bool ConnectDB(const char* host, const char* user, const char* pass, const char* libname, const unsigned int port);	//连接数据库
	void CloseDB(bool bClose=true);		//断开数据库

	bool ExectCommand(char *para);			//更新记录
	
	const char * OutErrors();				//输出错误信息
	
#ifdef MYSQL_DB_DEF
	bool SelectRecord(char *para);			//查询记录
	my_ulonglong GetRowNum();				//得到记录数
	MYSQL_ROW GetRecord();					//得到结果（一个记录）

	bool GetResult(char *para, unsigned long long &irowsnum, MYSQL_ROW &row, MYSQL_RES **query); // 参照mysqlpp封装，但只能获取一行
	bool GetResults(char *para, unsigned long long &irowsnum, std::map<unsigned long long, MYSQL_ROW> &rows_set, MYSQL_RES **query); // 参照mysqlpp封装，获取多行
	bool FreeResult(MYSQL_RES *query);
#elif SHENTONG_DB_DEF
	//// 输入 sql
	//// 输入/输出 pstmt
	//// 输入/输出 prs
	//bool ExectCommand(char *sql, Statement **pstmt, ResultSet **prs);
	// 输入 sql
	// 输入/输出 pstmt
	// 输入/输出 prs
	bool GetResults(char *sql, Statement **pstmt, ResultSet **prs);

	// 输入 pstmt
	// 输入 prs
	bool FreeResult(Statement *pstmt, ResultSet *prs);
#endif

protected:
	CDBConnector();
	void InitDB();
	void ReleaseDB();

private:	

	CCriticalSection m_cs;
	bool m_bconnflag;

#ifdef MYSQL_DB_DEF
	MYSQL mysql;							//数据库连接句柄
	MYSQL_RES *query;						//结果集
	MYSQL_ROW row;							//记录集
	MYSQL_FIELD *field;						//字段信息（结构体）

	MYSQL_RES *m_pQueryRes; //结果集
#elif SHENTONG_DB_DEF
	//shentong::acci::Environment *m_penv;/*环境句柄*/
	//shentong::acci::Connection *m_pconn;/*连接句柄*/
	//shentong::acci::Statement *m_pstmt;/*语句句柄*/

	Environment *m_penv;/*环境句柄*/
	Connection *m_pconn;/*连接句柄*/
	Statement *m_pstmt;/*语句句柄*/
#endif

};

#endif