#ifndef LIST_MPSYSMTU_H
#define LIST_MPSYSMTU_H

#include <iostream>
#include "DBconnector.h"


using std::string;


class CSysParameter
{
public:
	CSysParameter();
	virtual ~CSysParameter();

	unsigned int GetMPMaxMtu() { return m_MP_MaxMtu; }

	void SetMPMaxMtu(unsigned int mp_max_mtu) { m_MP_MaxMtu = mp_max_mtu; }

	bool SelectMPSysMtu();

private:
	unsigned int m_MP_MaxMtu;

	CCriticalSection m_csCCSysParameter;

};

#endif //LIST_MPSYSMTU_H