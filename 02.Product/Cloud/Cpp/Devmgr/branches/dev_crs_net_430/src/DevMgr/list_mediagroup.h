
#ifndef LIST_MEDIAGROUP_H
#define LIST_MEDIAGROUP_H

#include <iostream>
#include "DBconnector.h"

using std::string;

class CMediaGroup
{
public:
	virtual ~CMediaGroup();
	static CMediaGroup* GetInstance();

	//
	int GetMediaGroupID() { return m_mediagroup_id; }
	std::string GetMediaGroupDomain() { return m_mediagroup_domain; }
	std::string GetMediaGroupDesc() { return m_mediagroup_desc; }

	void SetMediaGroupID(int mediagroup_id) { m_mediagroup_id = mediagroup_id; }
	void SetMediaGroupDomain(std::string mediagroup_domain) { m_mediagroup_domain = mediagroup_domain; }
	void SetMediaGroupDesc(std::string mediagroup_desc) { m_mediagroup_desc = mediagroup_desc; }

	//
	bool InsertDB();
	bool DeleteDB();
	bool UpdateDB();
	bool SelectDB();

protected:
	CMediaGroup();

private:
	int m_mediagroup_id;
	std::string m_mediagroup_domain;
	std::string m_mediagroup_desc;

	CCriticalSection m_csMediaGroup;
};

#endif //LIST_MEDIAGROUP_H