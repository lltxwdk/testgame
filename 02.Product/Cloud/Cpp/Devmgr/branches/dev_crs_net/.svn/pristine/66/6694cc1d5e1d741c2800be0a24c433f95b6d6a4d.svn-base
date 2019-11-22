// sredisdll.cpp : Defines the exported functions for the DLL application.
//

#if defined WIN32
#include "stdafx.h"
#endif
#include "sredisdll.h"

#include "sredis.h"

// This is the constructor of a class that has been exported.
// see sredisdll.h for the class definition
namespace ISREDIS
{
	SREDISDLL_API ICsredis* CreateIRedis(ICsredisSink* pcSink)
	{
		if (pcSink == NULL)
			return NULL;
		return new CSRedis(pcSink);
	}

	SREDISDLL_API ICsredis** CreateRedisInstanceList(ICsredisSink* pcSink, unsigned int uiRedisConnNum)
	{
		if (pcSink == NULL)
			return NULL;

		ICsredis** pICsredis = new ICsredis*[uiRedisConnNum];

		for (unsigned int i = 0; i < uiRedisConnNum; i++)
		{
			pICsredis[i] = new CSRedis(pcSink);
		}

		if (pICsredis == NULL)
			return NULL;
		return pICsredis;
	}

	SREDISDLL_API void DeleteIRedis(ICsredis* pIRedis)
	{
		if (pIRedis)
		{
			delete pIRedis;
		}
	}

	SREDISDLL_API void DeleteRedisInstanceList(ICsredis** pIRedis, unsigned int uiRedisConnNum)
	{
		for (unsigned int i = 0; i < uiRedisConnNum; i++)
		{
			if (pIRedis[i] != NULL)
			{
				delete pIRedis[i];
			}
		}

		delete[] pIRedis;
	}
}
