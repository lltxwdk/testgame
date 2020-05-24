#ifndef __AUTO_TRACE_H_
#define __AUTO_TRACE_H_
#include "BaseThread.h"

// CAutoTrace
class CAutoTrace
{
public:
	static CAutoTrace* GetInstance();
	~CAutoTrace();

	void AutoTrace(bool bTrace, const char* pFile, const char* pContent);

protected:
	CAutoTrace();

private:
	FILE* m_fp;
	CCriticalSection m_cs;
	char m_sFile[256];
	char m_sContent[2048];
	bool m_bTrace;
};

#endif
