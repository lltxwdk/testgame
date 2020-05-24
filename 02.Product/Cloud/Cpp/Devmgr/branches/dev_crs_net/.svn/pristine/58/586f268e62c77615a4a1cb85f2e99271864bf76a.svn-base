
#ifndef LIST_COMPLINCENE_H
#define LIST_COMPLINCENE_H

#include <iostream>
#include <set>
#include "DBconnector.h"

using namespace std;

struct sLinceneInfo
{
	//int type;							//授权类型
	int amount;							//授权点数
	std::string starttime;				//授权开始时间
	std::string expiretime;				//授权截止时间
};

class CCompCapLincene
{
public:
	CCompCapLincene();
	virtual ~CCompCapLincene();
	//static CDeviceConfig* GetInstance();

	bool GetCompLinceneInfo(int compid, map<std::string, sLinceneInfo>& linceneInfo);

	std::set<int> GetCompIdList();

	bool SelectSingleCompLinceneInfo(int compid, map<std::string, sLinceneInfo>& linceneInfo);
	//
	bool InsertDB();
	bool DeleteDB();
	bool UpdateDB();
	bool SelectLinceneIdDB();
	bool SelectCompLinceneDB();

private:
	int m_compid;							//企业id
 
	//t_capbility表对应值,key-capid,value-col_ab

	std::map<int, string> mapCapbilityInfo;

	//企业授权信息,key-compid,value-map, keys-capid,value-sLinceneInfo
	std::map<int, map<std::string, sLinceneInfo> > m_mapCompLinceneInfo;

	//企业授权capid与redis 映射表
	std::map<int, std::string>mapCompCapRedisTable;

	//所有可用的企业id
	std::set<int> m_sCompId;

	CCriticalSection m_csCCompLincene;
};

#endif //LIST_DEVICECONFIG_H