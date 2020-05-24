// RcParser.cpp: implementation of the CRcParser class.
//
//////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include "stdafx.h"
#endif
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "parser.h"
#include "fcntl.h"
#ifdef WIN32
HMODULE g_hModule = NULL;
#endif

#ifdef LINUX
#include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParser::CParser()
: m_szFileUrl("")
{
	char pathname[256] = {0};
	printf("app path:::::::::%s\n", GetAppDir().c_str());
	strcat(pathname, GetAppDir().c_str());
	strcat(pathname, "devmgr.conf");
	m_szFileUrl = pathname;

	Parser();
}

CParser::~CParser()
{
	
}

CParser* CParser::GetInstance()
{
	static CParser g_cParser;
	return &g_cParser;
}

bool CParser::Parser()
{
	//char m_buf[255];   
	//数据处理, 判断文件是否存在
    if (m_szFileUrl.length() > 0)
	{		
		m_nDeviceNum = GetIniKeyInt("devicenum", "DeviceNum", m_szFileUrl.c_str());
		m_nOPDBNum = GetIniKeyInt("operatdbnum", "OperatDBNum", m_szFileUrl.c_str());
		m_nWriteDBTimespan = GetIniKeyInt("timespan", "WriteDBTimespan", m_szFileUrl.c_str());
		m_nDevHeartbeatTimeout = GetIniKeyInt("timespan", "DevHeartbeatTimeout", m_szFileUrl.c_str());
		m_nConfdtlinfoTimeout = GetIniKeyInt("timespan", "ConfDetailInfoTimeout", m_szFileUrl.c_str());
		m_nMCLinceneInfoTime = GetIniKeyInt("timespan", "DevLinceneCheckTime", m_szFileUrl.c_str());
		if (m_nConfdtlinfoTimeout == 0)
		{
			m_nConfdtlinfoTimeout = 5 * 60;//默认5分钟
		}

		int isCloseTimeoutSocket = GetIniKeyInt("timespan", "IsCloseTimeoutSocket", m_szFileUrl.c_str());
		m_bCloseTimeoutSocket = (isCloseTimeoutSocket != 0) ? true : false;
		
		// generate
		m_nMySvrType = GetIniKeyInt("generate", "dev_svr_type", m_szFileUrl.c_str());
		m_szMySerialNumber = GetIniKeyString("generate", "serial_number", m_szFileUrl.c_str());
		m_szSelfRegSerialNO = GetIniKeyString("generate", "selfreg_serialno", m_szFileUrl.c_str());

		// listenaddr
		m_szMyListenIpAddr = GetIniKeyString("listenaddr", "listenip", m_szFileUrl.c_str());
		m_nMyListenPort = GetIniKeyInt("listenaddr", "listenport", m_szFileUrl.c_str());
		m_nMyHTTPListenPort = GetIniKeyInt("listenaddr", "httplistenport", m_szFileUrl.c_str());

		//m_nDevicePort = GetIniKeyInt("deviceport", "Deviceport", m_szFileUrl.c_str());

		// master_%d
		m_szMaster1IpAddr = GetIniKeyString("master_1", "deviceip", m_szFileUrl.c_str());
		m_nMaster1Port = GetIniKeyInt("master_1", "deviceport", m_szFileUrl.c_str());

		m_szMaster2IpAddr = GetIniKeyString("master_2", "deviceip", m_szFileUrl.c_str());
		m_nMaster2Port = GetIniKeyInt("master_2", "deviceport", m_szFileUrl.c_str());

		m_szMaster3IpAddr = GetIniKeyString("master_3", "deviceip", m_szFileUrl.c_str());
		m_nMaster3Port = GetIniKeyInt("master_3", "deviceport", m_szFileUrl.c_str());

		m_szMaster4IpAddr = GetIniKeyString("master_4", "deviceip", m_szFileUrl.c_str());
		m_nMaster4Port = GetIniKeyInt("master_4", "deviceport", m_szFileUrl.c_str());
		
		//MYSQL配置信息
		//::GetPrivateProfileString("mysqlinfo", "MysqlIpAddr", "", m_buf, sizeof(m_buf), m_szFileUrl.c_str());
		m_szMysqlIpAddr = GetIniKeyString("mysqlinfo", "MysqlIpAddr", m_szFileUrl.c_str());
		
		//::GetPrivateProfileString("mysqlinfo", "MysqlPort", "", m_buf, sizeof(m_buf), m_szFileUrl.c_str());
		m_nMysqlPort = GetIniKeyInt("mysqlinfo", "MysqlPort", m_szFileUrl.c_str());
		
		//::GetPrivateProfileString("mysqlinfo", "MysqlServ", "", m_buf, sizeof(m_buf), m_szFileUrl.c_str());
		m_szMysqlServName = GetIniKeyString("mysqlinfo", "MysqlServ", m_szFileUrl.c_str());
		
		//::GetPrivateProfileString("mysqlinfo", "MysqlUser", "", m_buf, sizeof(m_buf), m_szFileUrl.c_str());
		m_szMysqlUser = GetIniKeyString("mysqlinfo", "MysqlUser", m_szFileUrl.c_str());
		
		//::GetPrivateProfileString("mysqlinfo", "MysqlPass", "", m_buf, sizeof(m_buf), m_szFileUrl.c_str());
		m_szMysqlPass = GetIniKeyString("mysqlinfo", "MysqlPass", m_szFileUrl.c_str());		
			
		//REDIS配置信息
		//::GetPrivateProfileString("redisinfo", "RedisIpAddr", "", m_buf, sizeof(m_buf), m_szFileUrl.c_str());
		m_szRedisIpAddr = GetIniKeyString("redisinfo", "RedisIpAddr", m_szFileUrl.c_str());
		
		//::GetPrivateProfileString("redisinfo", "RedisPort", "", m_buf, sizeof(m_buf), m_szFileUrl.c_str());
		m_nRedisPort = GetIniKeyInt("redisinfo", "RedisPort", m_szFileUrl.c_str());
		
		//::GetPrivateProfileString("redisinfo", "RedisServ", "", m_buf, sizeof(m_buf), m_szFileUrl.c_str());
		m_szRedisServName = GetIniKeyString("redisinfo", "RedisServ", m_szFileUrl.c_str());
		
		//::GetPrivateProfileString("redisinfo", "RedisUser", "", m_buf, sizeof(m_buf), m_szFileUrl.c_str());
		m_szRedisUser = GetIniKeyString("redisinfo", "RedisUser", m_szFileUrl.c_str());
		
		//::GetPrivateProfileString("redisinfo", "RedisPass", "", m_buf, sizeof(m_buf), m_szFileUrl.c_str());
		m_szRedisPass = GetIniKeyString("redisinfo", "RedisPass", m_szFileUrl.c_str());

		// LOG日志等级、是否打印到终端控制台
		m_nLoglevel = GetIniKeyInt("logconf", "LogLevel", m_szFileUrl.c_str());

		int isStdout = GetIniKeyInt("logconf", "IsStdout", m_szFileUrl.c_str());
		m_bIsStdout = (isStdout != 0) ? true : false;
	}
	
    return true;
}

void CParser::Splitpathex(const char *path, char *dirname, char *filename)
{
	char*	lastSlash;
	lastSlash = (char*)strrchr(path, '\\');
	char tmpChar = *lastSlash;
	*lastSlash = '\0';

	if (dirname)
		strcpy(dirname, path);
	if (filename)
		strcpy(filename, lastSlash + 1);

	*lastSlash = tmpChar;
}

std::string CParser::GetAppDir()
{
	char buf[1024] = {0};
#ifdef WIN32
	char pathname[1024];
	char filename[1024];
	GetModuleFileName(g_hModule, buf, 1024);
	Splitpathex(buf, pathname, filename);
	strcat(pathname, "\\");
	return pathname;
#elif defined LINUX
	int i;
	int count = 1024;
	int rslt = readlink("/proc/self/exe", buf, count - 1);
	if (rslt < 0 || (rslt >= count - 1))
	{
		return NULL;
	}
	buf[rslt] = '\0';
	for (i = rslt; i >= 0; i--)
	{
		//printf("buf[%d] %c\n", i, buf[i]);
		if (buf[i] == '/')
		{
			buf[i + 1] = '\0';
			break;
		}
	}
	return buf;
#endif
}

//从INI文件读取字符串类型数据  
char* CParser::GetIniKeyString(const char *title, const char *key, const char *filename)
{
	FILE *fp;
	char szLine[1024];
	static char tmpstr[1024];
	int rtnval;
	int i = 0;
	int flag = 0;
	char *tmp;

	if ((fp = fopen(filename, "r")) == NULL)
	{
		printf("have   no   such   file \n");
		return "";
	}
	while (!feof(fp))
	{
		rtnval = fgetc(fp);
		if (rtnval == EOF)
		{
			break;
		}
		else
		{
			szLine[i++] = rtnval;
		}
		if (rtnval == '\n')
		{
#ifndef WIN32  
			i--;
#endif    
			szLine[--i] = '\0';
			i = 0;
			tmp = strchr(szLine, '=');

			if ((tmp != NULL) && (flag == 1))
			{
				if (strstr(szLine, key) != NULL)
				{
					//注释行  
					if ('#' == szLine[0])
					{
					}
					else if ('/' == szLine[0] && '/' == szLine[1])
					{

					}
					else
					{
						//找打key对应变量  
						strcpy(tmpstr, tmp + 1);
						fclose(fp);
						return tmpstr;
					}
				}
			}
			else
			{
				strcpy(tmpstr, "[");
				strcat(tmpstr, title);
				strcat(tmpstr, "]");
				if (strncmp(tmpstr, szLine, strlen(tmpstr)) == 0)
				{
					//找到title  
					flag = 1;
				}
			}
		}
	}
	fclose(fp);
	return "";
}

//从INI文件读取整类型数据  
int CParser::GetIniKeyInt(const char *title, const char *key, const char *filename)
{
	char* pBuf = GetIniKeyString(title, key, filename);
	if (pBuf != NULL)
		return atoi(GetIniKeyString(title, key, filename));
	return 0;
}
/*
int main(int argc, char* argv[])
{
	char buf[MAX_PATH];
	memset(buf, 0, sizeof(buf));
	GetCurrentPath(buf, CONF_FILE_PATH);
	strcpy(g_szConfigPath, buf);

	int iCatAge;
	char szCatName[32];

	iCatAge = GetIniKeyInt("CAT", "age", g_szConfigPath);
	char *a = GetIniKeyString("CAT", "name", g_szConfigPath);
	printf("%d\r\n", iCatAge);
	printf("%s\r\n", a);

	return 0;
}*/