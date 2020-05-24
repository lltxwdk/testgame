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
		
	bool ConnectDB(const char* host, const char* user, const char* pass, const char* libname, const unsigned int port);	//�������ݿ�
	void CloseDB(bool bClose=true);		//�Ͽ����ݿ�

	bool ExectCommand(char *para);			//���¼�¼
	
	const char * OutErrors();				//���������Ϣ
	
#ifdef MYSQL_DB_DEF
	bool SelectRecord(char *para);			//��ѯ��¼
	my_ulonglong GetRowNum();				//�õ���¼��
	MYSQL_ROW GetRecord();					//�õ������һ����¼��

	bool GetResult(char *para, unsigned long long &irowsnum, MYSQL_ROW &row, MYSQL_RES **query); // ����mysqlpp��װ����ֻ�ܻ�ȡһ��
	bool GetResults(char *para, unsigned long long &irowsnum, std::map<unsigned long long, MYSQL_ROW> &rows_set, MYSQL_RES **query); // ����mysqlpp��װ����ȡ����
	bool FreeResult(MYSQL_RES *query);
#elif SHENTONG_DB_DEF
	//// ���� sql
	//// ����/��� pstmt
	//// ����/��� prs
	//bool ExectCommand(char *sql, Statement **pstmt, ResultSet **prs);
	// ���� sql
	// ����/��� pstmt
	// ����/��� prs
	bool GetResults(char *sql, Statement **pstmt, ResultSet **prs);

	// ���� pstmt
	// ���� prs
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
	MYSQL mysql;							//���ݿ����Ӿ��
	MYSQL_RES *query;						//�����
	MYSQL_ROW row;							//��¼��
	MYSQL_FIELD *field;						//�ֶ���Ϣ���ṹ�壩

	MYSQL_RES *m_pQueryRes; //�����
#elif SHENTONG_DB_DEF
	//shentong::acci::Environment *m_penv;/*�������*/
	//shentong::acci::Connection *m_pconn;/*���Ӿ��*/
	//shentong::acci::Statement *m_pstmt;/*�����*/

	Environment *m_penv;/*�������*/
	Connection *m_pconn;/*���Ӿ��*/
	Statement *m_pstmt;/*�����*/
#endif

};

#endif