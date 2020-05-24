
#ifndef _UTILITY_H
#define _UTILITY_H

#include <string>
#include <list>

using std::string;
using std::list;

const bool g_Init = true;

typedef struct _DATAPARAM	//���ݿ��������
{
	char *db_name;			//���ݿ���
	char *tab_name;			//����
	char *col_name;			//����
	char *select_exp;		//ѡ����ʽ
	char *where_def;		//ѡ���޶�����
	char *insert_val;		//����ֵ
	char *set_exp;			//�������ݿ����ݵı��ʽ
}SDataParam;

typedef struct _DATABASEPARAM	//���ݿ����
{
	char *host;					//������
	char *user;					//�û���
	char *password;				//����
	char *db;					//���ݿ���
	unsigned int port;			//�˿ڣ�һ��Ϊ0
	const char *unix_socket;	//�׽��֣�һ��ΪNULL
	unsigned int client_flag;	//һ��Ϊ0
}SDatabaseParam;

#endif //_UTILITY_H