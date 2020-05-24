#ifndef LIST_MPSYSMTU_H
#define LIST_MPSYSMTU_H

#include <iostream>
#include "DBconnector.h"


using std::string;


class CMpSysMtu
{
public:
	CMpSysMtu();
	virtual ~CMpSysMtu();

	unsigned int GetMPMaxMtu() { return m_MP_MaxMtu; }

	void SetMPMaxMtu(unsigned int mp_max_mtu) { m_MP_MaxMtu = mp_max_mtu; }

	bool SelectMPSysMtu();

private:
	unsigned int m_MP_MaxMtu;

	CCriticalSection m_csCCMpSysMtu;

};

#endif //LIST_MPSYSMTU_H