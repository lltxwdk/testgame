
#ifndef _UTILITY_H
#define _UTILITY_H

#include <string>
#include <list>

using std::string;
using std::list;

const bool g_Init = true;

typedef struct _DATAPARAM	//数据库操作参数
{
	char *db_name;			//数据库名
	char *tab_name;			//表名
	char *col_name;			//列名
	char *select_exp;		//选择表达式
	char *where_def;		//选择限定条件
	char *insert_val;		//插入值
	char *set_exp;			//更新数据库内容的表达式
}SDataParam;

typedef struct _DATABASEPARAM	//数据库参数
{
	char *host;					//主机名
	char *user;					//用户名
	char *password;				//密码
	char *db;					//数据库名
	unsigned int port;			//端口，一般为0
	const char *unix_socket;	//套接字，一般为NULL
	unsigned int client_flag;	//一般为0
}SDatabaseParam;

#endif //_UTILITY_H