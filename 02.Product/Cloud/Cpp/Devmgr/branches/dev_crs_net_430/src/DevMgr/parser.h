// RcParser.h: interface for the CRcParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSER_H__B63D0D63_9B7A_430E_AD1D_D097FA5E1EA1__INCLUDED_)
#define AFX_PARSER_H__B63D0D63_9B7A_430E_AD1D_D097FA5E1EA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include "string"
using std::string;

class CParser  
{
public:
	virtual ~CParser();
	
	static CParser* GetInstance();
	
	bool Parser();
	void Splitpathex(const char *path, char *dirname, char *filename);

	unsigned int GetDeviceNum() const { return m_nDeviceNum; }
	unsigned int GetOPDBNum() const { return m_nOPDBNum; }
	unsigned int GetWriteDBTimespan() const { return m_nWriteDBTimespan; }
	unsigned int GetDevHeartbeatTimeout() const { return m_nDevHeartbeatTimeout; }
	unsigned int GetConfDetailInfoTimeout() const { return m_nConfdtlinfoTimeout; }
	bool IsCloseTimeoutSocket() const { return m_bCloseTimeoutSocket; }

	unsigned int GetMySvrType() const { return m_nMySvrType; }
	std::string GetMySerialNumber()const { return m_szMySerialNumber; }
	std::string GetSelfRegSerialNO()const { return m_szSelfRegSerialNO; }

	std::string GetMyListenIpAddr()const { return m_szMyListenIpAddr; }
	unsigned int GetMyListenPort() const { return m_nMyListenPort; }
	unsigned int GetMyHTTPListenPort() const { return m_nMyHTTPListenPort; }
	//unsigned int GetDevicePort() const { return m_nDevicePort; }

	// master
	std::string GetMaster1IpAddr()const { return m_szMaster1IpAddr; }
	unsigned int GetMaster1Port()const { return m_nMaster1Port; }
	std::string GetMaster2IpAddr()const { return m_szMaster2IpAddr; }
	unsigned int GetMaster2Port()const { return m_nMaster2Port; }
	std::string GetMaster3IpAddr()const { return m_szMaster3IpAddr; }
	unsigned int GetMaster3Port()const { return m_nMaster3Port; }
	std::string GetMaster4IpAddr()const { return m_szMaster4IpAddr; }
	unsigned int GetMaster4Port()const { return m_nMaster4Port; }

	//mysql
	std::string GetMysqlIpAddr()const {return m_szMysqlIpAddr;}
	std::string GetMysqlServName()const {return m_szMysqlServName;}
	unsigned int GetMysqlPort()const { return m_nMysqlPort; }
	std::string GetMysqlUser()const {return m_szMysqlUser;}
	std::string GetMysqlPass()const {return m_szMysqlPass;}
	
	//redis
	std::string GetRedisIpAddr()const {return m_szRedisIpAddr;}
	std::string GetRedisServName()const {return m_szRedisServName;}
	unsigned int GetRedisPort()const { return m_nRedisPort; }
	std::string GetRedisUser()const {return m_szRedisUser;}
	std::string GetRedisPass()const {return m_szRedisPass;}

	// logconf
	unsigned int GetLoglevel()const { return m_nLoglevel; }
	bool GetIsStdout()const { return m_bIsStdout; }
	
protected:
	CParser();
	std::string GetAppDir();
	char* GetIniKeyString(const char *title, const char *key, const char *filename);
	int GetIniKeyInt(const char *title, const char *key, const char *filename);
private:	
	std::string m_szFileUrl;

	//deviceport
	unsigned int m_nDeviceNum;
	//unsigned int m_nDevicePort;

	// 每次操作（插入、更新）数据库次数，时间间隔
	unsigned int m_nOPDBNum;
	unsigned int m_nWriteDBTimespan;
	unsigned int m_nDevHeartbeatTimeout;
	unsigned int m_nConfdtlinfoTimeout;
	bool m_bCloseTimeoutSocket;

	// generate
	unsigned int m_nMySvrType;
	std::string m_szMySerialNumber;
	std::string m_szSelfRegSerialNO;

	// listenaddr
	std::string m_szMyListenIpAddr;
	unsigned int m_nMyListenPort;
	unsigned int m_nMyHTTPListenPort;

	// master
	std::string m_szMaster1IpAddr;
	unsigned int m_nMaster1Port;
	std::string m_szMaster2IpAddr;
	unsigned int m_nMaster2Port;
	std::string m_szMaster3IpAddr;
	unsigned int m_nMaster3Port;
	std::string m_szMaster4IpAddr;
	unsigned int m_nMaster4Port;
	
	//mysql
	std::string m_szMysqlIpAddr;
	unsigned int m_nMysqlPort;
	std::string m_szMysqlServName;
	std::string m_szMysqlUser;
	std::string m_szMysqlPass;
		
	//redis
	std::string m_szRedisIpAddr;
	unsigned int m_nRedisPort;
	std::string m_szRedisServName;
	std::string m_szRedisUser;
	std::string m_szRedisPass;

	// logconf
	unsigned int m_nLoglevel;
	bool m_bIsStdout;
}; 

#endif // !defined(AFX_PARSER_H__B63D0D63_9B7A_430E_AD1D_D097FA5E1EA1__INCLUDED_)
