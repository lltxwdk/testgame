
#ifndef LIST_DATADICTIONARY_H
#define LIST_DATADICTIONARY_H

#include <iostream>
#include <string.h>
#include <list>
#include "DBconnector.h"

//#include "sr_timer.h"

using std::string;

class CDataDictionary
{
public:
	CDataDictionary();
	virtual ~CDataDictionary();

	//
	int GetDataDicID() { return m_datadic_id; }
	void SetDataDicID(int datadic_id) { m_datadic_id = datadic_id; }

	//
	bool SelectAudioProtocol(std::list<std::string> &audioProtocols);
	bool SelectVideoProtocol(std::list<std::string> &videoProtocols);

protected:
	//bool DeleteDB();

private:
	int m_datadic_id;

	CCriticalSection m_csDatadic;
};

#endif //LIST_DATADICTIONARY_H